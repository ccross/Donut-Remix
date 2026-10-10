#pragma once

#include <string>
#include <mutex>
#include <cstdint>

#ifdef ARDUINO
#ifndef USB_MODE
#define USB_MODE 1
#endif
#include <Arduino.h>
#if USB_MODE
#ifndef ESP_USB_HOST_CDC_RX_BUFFER_SIZE
#define ESP_USB_HOST_CDC_RX_BUFFER_SIZE 16384
#endif
#include <EspUsbHost.h>
#endif
#endif

namespace DonutRemix {

enum class LedState {
    SetupOrange,     // Connecting / AP setup mode
    QueryingBlue,    // Active WiFi, querying consoles
    ProfileSentGreen,// Profile switched
    Off,
    ErrorRed
};

class DeviceManager {
public:
    DeviceManager();

    void init();

    // LED Status
    void setLed(LedState state);

    // RetroTINK 4K Commands (Thread-safe)
    void sendProfile(int profile, uint16_t offset = 0);
    void sendSvsInput(uint16_t svsNumber);
    void sendRemoteButtonProfile(int rbpNumber);
    void sendRawCommand(const std::string& cmd);

    // Extron Matrix Serial
    void sendExtron1(const std::string& cmd);
    void sendExtron2(const std::string& cmd);

    // IR Remote
    void sendIR(const std::string& target, uint8_t code, uint8_t repeats = 1);

    // Serial Bridge Lock (for WebCtl file transfers)
    bool acquireSerialLock(const std::string& owner, uint32_t token);
    void releaseSerialLock(uint32_t token);
    bool isSerialLocked() const { return m_serialLocked; }

    // Direct RT4K Byte Stream for WebCtl Bridge
    void writeRT4KBytes(const uint8_t* data, size_t len);
    size_t readRT4KBytes(uint8_t* buffer, size_t maxLen);
    void setRT4KBaud(uint32_t baud);
    uint32_t getRT4KBaud() const { return m_rt4kBaud; }
    bool isUsbConnected() const;
    void wakeRT4K();

    // HTTP Bulk PUT Session for WebCtl
    struct HttpPutSession {
        bool active{false};
        uint32_t token{0};
        uint16_t nonce{0};
        size_t expectedSize{0};
        size_t bytesSent{0};
        uint32_t framesSent{0};
        uint8_t seq{0};
        uint16_t payloadSize{2048};
        bool localAck{false};
        uint32_t startTime{0};
        uint8_t chunkBuf[2048];
        size_t chunkLen{0};
        std::string lastError;
    };

    void beginHttpPutSession(uint32_t token, uint16_t nonce, size_t size, bool localAck, uint16_t payload);
    void endHttpPutSession();
    HttpPutSession& getHttpPutSessionRef() { return m_putSession; }
    bool sendRtl1Frame(uint16_t nonce, uint8_t type, uint8_t seq, const uint8_t* payload, size_t len);

    int getCurrentProfile() const { return m_currentProfile; }

private:
    std::mutex m_ioMutex;
    int m_currentProfile{0};
    int m_prevProfile{0};
    uint32_t m_rt4kBaud{9600};

    bool m_serialLocked{false};
    std::string m_lockOwner;
    uint32_t m_lockToken{0};

    HttpPutSession m_putSession;
};

} // namespace DonutRemix
