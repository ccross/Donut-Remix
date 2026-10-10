#pragma once

#include "Devices.h"
#include "Config.h"
#include <string>
#include <cstdint>

#ifdef ARDUINO
#include <WebSocketsServer.h>
#endif

namespace DonutRemix {

class WebCtlBridge {
public:
    WebCtlBridge(DeviceManager& devices, ConfigManager& config);

    void begin();
    void loop();

#ifdef ARDUINO
    void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
#endif

private:
    DeviceManager& m_devices;
    ConfigManager& m_config;

#ifdef ARDUINO
    WebSocketsServer m_wsServer;
    uint8_t m_rxBuffer[1024];
#endif

    void handleControlJson(uint8_t clientNum, const std::string& jsonStr);
    void handleRawTextCommand(uint8_t clientNum, const std::string& cmd);
};

} // namespace DonutRemix
