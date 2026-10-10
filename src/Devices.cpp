#include "Devices.h"
#include <iostream>

#ifdef ARDUINO
#include <Arduino.h>
#define IR_SEND_PIN 11
#define LED_PIN_BUILTIN 13

// Extron hardware serial ports
#define ExtronSerial1 Serial1
#define ExtronSerial2 Serial2

#if defined(USB_MODE) && USB_MODE
extern EspUsbHost usbHost;
extern EspUsbHostCdcSerial CdcSerial;
#endif
#endif

namespace DonutRemix {

DeviceManager::DeviceManager() = default;

void DeviceManager::init() {
#ifdef ARDUINO
    pinMode(LED_PIN_BUILTIN, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_RED, OUTPUT);

    // Initial RT4K Serial baud rate on hardware pins 0/1 (Serial0)
    Serial0.begin(m_rt4kBaud);
    Serial.begin(115200);
    ExtronSerial1.begin(9600, SERIAL_8N1, 3, 4);
    ExtronSerial1.setTimeout(50);
    ExtronSerial2.begin(9600, SERIAL_8N1, 8, 9);
    ExtronSerial2.setTimeout(50);
#endif
    setLed(LedState::SetupOrange);
}

void DeviceManager::setLed(LedState state) {
#ifdef ARDUINO
    switch (state) {
        case LedState::SetupOrange:
            analogWrite(LED_RED, 220);
            analogWrite(LED_GREEN, 245);
            analogWrite(LED_BLUE, 255);
            break;
        case LedState::QueryingBlue:
            analogWrite(LED_RED, 255);
            analogWrite(LED_GREEN, 255);
            analogWrite(LED_BLUE, 222);
            break;
        case LedState::ProfileSentGreen:
            analogWrite(LED_RED, 255);
            analogWrite(LED_GREEN, 0);
            analogWrite(LED_BLUE, 255);
            break;
        case LedState::ErrorRed:
            analogWrite(LED_RED, 0);
            analogWrite(LED_GREEN, 255);
            analogWrite(LED_BLUE, 255);
            break;
        case LedState::Off:
        default:
            analogWrite(LED_RED, 255);
            analogWrite(LED_GREEN, 255);
            analogWrite(LED_BLUE, 255);
            break;
    }
#else
    // Mock LED transition for host builds
    (void)state;
#endif
}

void DeviceManager::sendRawCommand(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
    std::string formatted = "\r" + cmd + "\r\n";
    Serial0.print(formatted.c_str());
    Serial.print(formatted.c_str());
#if defined(USB_MODE) && USB_MODE
    CdcSerial.print(formatted.c_str());
#endif
#else
    std::cout << "[Device Mock RT4K] Raw Command: " << cmd << std::endl;
#endif
}

void DeviceManager::sendSvsInput(uint16_t svsNumber) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    std::string cmd = "\rSVS NEW INPUT=" + std::to_string(svsNumber) + "\r\n";
#ifdef ARDUINO
    digitalWrite(LED_PIN_BUILTIN, HIGH);
    Serial0.print(cmd.c_str());
    Serial.print(cmd.c_str());
#if defined(USB_MODE) && USB_MODE
    CdcSerial.print(cmd.c_str());
#endif
    digitalWrite(LED_PIN_BUILTIN, LOW);
#else
    std::cout << "[Device Mock RT4K] SVS Input: " << svsNumber << std::endl;
#endif
    m_prevProfile = m_currentProfile;
    m_currentProfile = svsNumber;
}

void DeviceManager::sendRemoteButtonProfile(int rbpNumber) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    std::string cmd = "\rremote prof" + std::to_string(rbpNumber) + "\r\n";
#ifdef ARDUINO
    Serial0.print(cmd.c_str());
    Serial.print(cmd.c_str());
#if defined(USB_MODE) && USB_MODE
    CdcSerial.print(cmd.c_str());
#endif
#else
    std::cout << "[Device Mock RT4K] Remote Button Profile: " << rbpNumber << std::endl;
#endif
    m_prevProfile = m_currentProfile;
    m_currentProfile = -1 * rbpNumber;
}

void DeviceManager::sendProfile(int profile, uint16_t offset) {
    if (profile == m_currentProfile) return;

    if (profile < 0) {
        sendRemoteButtonProfile(-1 * profile);
    } else {
        uint16_t svs = (profile > 0) ? (profile + offset) : 0;
        sendSvsInput(svs);
    }

    setLed(LedState::ProfileSentGreen);
#ifdef ARDUINO
    delay(100);
#endif
    setLed(LedState::QueryingBlue);
}

void DeviceManager::sendExtron1(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
    ExtronSerial1.print(cmd.c_str());
#else
    std::cout << "[Device Mock Extron1] " << cmd << std::endl;
#endif
}

void DeviceManager::sendExtron2(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
    ExtronSerial2.print(cmd.c_str());
#else
    std::cout << "[Device Mock Extron2] " << cmd << std::endl;
#endif
}

void DeviceManager::sendIR(const std::string& target, uint8_t code, uint8_t repeats) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
    // IrSender.send...
    (void)target;
    (void)code;
    (void)repeats;
#else
    std::cout << "[Device Mock IR] Target: " << target << " Code: " << static_cast<int>(code) << " Repeats: " << static_cast<int>(repeats) << std::endl;
#endif
}

bool DeviceManager::acquireSerialLock(const std::string& owner, uint32_t token) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    if (m_serialLocked && m_lockToken != token) {
        return false;
    }
    m_serialLocked = true;
    m_lockOwner = owner;
    m_lockToken = token;
    return true;
}

void DeviceManager::releaseSerialLock(uint32_t token) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    if (m_serialLocked && m_lockToken == token) {
        m_serialLocked = false;
        m_lockOwner.clear();
        m_lockToken = 0;
    }
}

static uint16_t rtl1_crc16_update(uint16_t crc, uint8_t b) {
    crc ^= (uint16_t)b << 8;
    for (int i = 0; i < 8; i++) {
        if (crc & 0x8000) {
            crc = (crc << 1) ^ 0x1021;
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

static uint16_t rtl1_crc16(const uint8_t* data, size_t len, uint16_t init = 0xFFFF) {
    uint16_t crc = init;
    for (size_t i = 0; i < len; i++) {
        crc = rtl1_crc16_update(crc, data[i]);
    }
    return crc;
}

void DeviceManager::writeRT4KBytes(const uint8_t* data, size_t len) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
#if defined(USB_MODE) && USB_MODE
    if (CdcSerial.connected()) {
        CdcSerial.write(data, len);
        return;
    }
#endif
    Serial0.write(data, len);
#else
    (void)data;
    (void)len;
#endif
}

bool DeviceManager::sendRtl1Frame(uint16_t nonce, uint8_t type, uint8_t seq, const uint8_t* payload, size_t len) {
    if (len > 2048) return false;
    uint8_t frame[2060];
    frame[0] = 0xA5;
    frame[1] = 0x5A;
    frame[2] = nonce & 0xFF;
    frame[3] = (nonce >> 8) & 0xFF;
    frame[4] = len & 0xFF;
    frame[5] = (len >> 8) & 0xFF;
    frame[6] = type;
    frame[7] = seq;

    uint16_t crc = rtl1_crc16(&frame[2], 6);
    if (payload && len > 0) {
        memcpy(&frame[8], payload, len);
        crc = rtl1_crc16(payload, len, crc);
    }
    frame[8 + len] = crc & 0xFF;
    frame[9 + len] = (crc >> 8) & 0xFF;

    writeRT4KBytes(frame, 10 + len);
    return true;
}

void DeviceManager::beginHttpPutSession(uint32_t token, uint16_t nonce, size_t size, bool localAck, uint16_t payload) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    m_putSession.active = true;
    m_putSession.token = token;
    m_putSession.nonce = nonce;
    m_putSession.expectedSize = size;
    m_putSession.localAck = localAck;
    m_putSession.payloadSize = payload;
    m_putSession.bytesSent = 0;
    m_putSession.framesSent = 0;
    m_putSession.seq = 0;
    m_putSession.chunkLen = 0;
    m_putSession.startTime = 0;
    m_putSession.lastError.clear();
}

void DeviceManager::endHttpPutSession() {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    m_putSession.active = false;
    m_putSession.chunkLen = 0;
}

size_t DeviceManager::readRT4KBytes(uint8_t* buffer, size_t maxLen) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
#if defined(USB_MODE) && USB_MODE
    int avail = CdcSerial.available();
    if (avail > 0) {
        size_t toRead = (size_t)avail;
        if (toRead > maxLen) toRead = maxLen;
        return CdcSerial.readBytes(buffer, toRead);
    }
#endif
    int avail0 = Serial0.available();
    if (avail0 > 0) {
        size_t toRead = (size_t)avail0;
        if (toRead > maxLen) toRead = maxLen;
        return Serial0.readBytes(buffer, toRead);
    }
    return 0;
#else
    (void)buffer;
    (void)maxLen;
    return 0;
#endif
}

void DeviceManager::setRT4KBaud(uint32_t baud) {
    std::lock_guard<std::mutex> lock(m_ioMutex);
    m_rt4kBaud = baud;
#ifdef ARDUINO
    Serial0.begin(baud);
#if defined(USB_MODE) && USB_MODE
    CdcSerial.setBaudRate(baud);
#endif
#endif
}

bool DeviceManager::isUsbConnected() const {
#if defined(USB_MODE) && USB_MODE
    return CdcSerial.connected() || usbHost.ready();
#else
    return false;
#endif
}

void DeviceManager::wakeRT4K() {
    std::lock_guard<std::mutex> lock(m_ioMutex);
#ifdef ARDUINO
    Serial0.print("\rpwr on\r\n");
#if defined(USB_MODE) && USB_MODE
    CdcSerial.print("\rpwr on\r\n");
#endif
#else
    std::cout << "[Device Mock RT4K] Wake sent" << std::endl;
#endif
}

} // namespace DonutRemix
