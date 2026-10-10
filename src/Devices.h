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

    int getCurrentProfile() const { return m_currentProfile; }

private:
    std::mutex m_ioMutex;
    int m_currentProfile{0};
    int m_prevProfile{0};
    uint32_t m_rt4kBaud{9600};

    bool m_serialLocked{false};
    std::string m_lockOwner;
    uint32_t m_lockToken{0};
};

} // namespace DonutRemix
