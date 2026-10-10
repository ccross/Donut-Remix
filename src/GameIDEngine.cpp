#include "GameIDEngine.h"
#include <iostream>
#include <algorithm>

#ifdef ARDUINO
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#endif

namespace DonutRemix {

GameIDEngine::GameIDEngine(ConfigManager& config, DeviceManager& devices)
    : m_config(config), m_devices(devices) {}

std::string GameIDEngine::extractGameIdFromPayload(const std::string& payload) {
    if (payload.empty()) return "";

    // Search for "gameID" in JSON format
    size_t keyPos = payload.find("\"gameID\"");
    if (keyPos != std::string::npos) {
        size_t colonPos = payload.find(':', keyPos);
        if (colonPos != std::string::npos) {
            size_t valStart = payload.find_first_not_of(" \t\r\n", colonPos + 1);
            if (valStart != std::string::npos) {
                if (payload[valStart] == '"') {
                    size_t valEnd = payload.find('"', valStart + 1);
                    if (valEnd != std::string::npos) {
                        return payload.substr(valStart + 1, valEnd - valStart - 1);
                    }
                } else {
                    size_t valEnd = payload.find_first_of(",}\r\n ", valStart);
                    if (valEnd == std::string::npos) valEnd = payload.length();
                    return payload.substr(valStart, valEnd - valStart);
                }
            }
        }
    }

    // If not JSON with "gameID", check if payload itself is a clean ID string (no braces/HTML)
    if (payload.front() != '{' && payload.front() != '<' && payload.length() <= Guardrails::MAX_GAMEID_LENGTH) {
        // Strip trailing whitespace
        std::string s = payload;
        while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ' || s.back() == '\t')) {
            s.pop_back();
        }
        return s;
    }

    return "";
}

std::string GameIDEngine::queryConsole(const std::string& url) {
    auto valid = Guardrails::validateConsoleUrl(url);
    if (!valid.valid) {
        return "";
    }

#ifdef ARDUINO
    if (WiFi.status() != WL_CONNECTED) {
        return "";
    }

    HTTPClient http;
    WiFiClientSecure https;
    bool isHttps = (url.rfind("https://", 0) == 0);

    http.setConnectTimeout(2000); // 2 second max connection timeout guard
    http.setTimeout(2500);

    if (isHttps) {
        https.setInsecure(); // Some home consoles use self-signed certs (e.g. MemCardPro)
        https.setTimeout(3);
        if (!http.begin(https, url.c_str())) return "";
    } else {
        if (!http.begin(url.c_str())) return "";
    }

    int httpCode = http.GET();
    std::string response;
    if (httpCode == HTTP_CODE_OK) {
        response = http.getString().c_str();
    }
    http.end();
    return response;
#else
    // Host mock
    return "";
#endif
}

void GameIDEngine::pollStep() {
    // If WebCtl has acquired serial lock for file transfer, suspend profile switches
    if (m_devices.isSerialLocked()) {
        return;
    }

    auto consoles = m_config.getConsoles();
    auto vars = m_config.getVars();

    int bestConsoleIndex = -1;
    uint8_t lowestOrder = 255;
    int targetProfile = 0;

    for (size_t i = 0; i < consoles.size(); ++i) {
        auto& c = consoles[i];
        if (!c.enabled) {
            m_config.updateConsoleState(i, false, 0, false, c.order);
            continue;
        }

        std::string rawResponse = queryConsole(c.url);
        if (!rawResponse.empty()) {
            std::string gameID = extractGameIdFromPayload(rawResponse);
            int resolvedProf = c.defaultProf;

            if (!gameID.empty()) {
                resolvedProf = m_config.findProfileForGameID(gameID, c.defaultProf);
            }

            m_config.updateConsoleState(i, true, resolvedProf, false, c.order);

            if (c.order < lowestOrder) {
                lowestOrder = c.order;
                bestConsoleIndex = static_cast<int>(i);
                targetProfile = resolvedProf;
            }
        } else {
            m_config.updateConsoleState(i, false, 0, false, c.order);
        }
    }

    // Set King console and send profile
    if (bestConsoleIndex != -1) {
        m_config.updateConsoleState(bestConsoleIndex, true, targetProfile, true, lowestOrder);
        m_devices.sendProfile(targetProfile, vars.offset);
    } else {
        // All consoles off: If S0 is enabled, load S0 profile
        if (vars.s0) {
            m_devices.sendProfile(0, vars.offset);
        }
    }
}

} // namespace DonutRemix
