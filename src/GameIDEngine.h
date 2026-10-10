#pragma once

#include <string>
#include <memory>
#include "Config.h"
#include "Devices.h"
#include "Guardrails.h"

namespace DonutRemix {

class GameIDEngine {
public:
    GameIDEngine(ConfigManager& config, DeviceManager& devices);

    // Single polling step over all enabled consoles
    void pollStep();

    // Query single console endpoint safely (returns raw gameID or empty)
    std::string queryConsole(const std::string& url);

    // Extract gameID from HTTP response payload
    static std::string extractGameIdFromPayload(const std::string& payload);

    void setPollIntervalMs(uint32_t intervalMs) { m_pollIntervalMs = intervalMs; }
    uint32_t getPollIntervalMs() const { return m_pollIntervalMs; }

private:
    ConfigManager& m_config;
    DeviceManager& m_devices;
    uint32_t m_pollIntervalMs{2000};
    uint32_t m_lastPollMs{0};
};

} // namespace DonutRemix
