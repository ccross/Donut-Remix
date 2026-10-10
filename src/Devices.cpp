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

    // Initial RT4K Serial baud rate (standard 9600 or 2M depending on firmware)
    Serial.begin(9600);
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

} // namespace DonutRemix
