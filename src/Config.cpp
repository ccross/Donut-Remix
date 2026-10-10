#include "Config.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace DonutRemix {

ConfigManager::ConfigManager() {
    loadDefaults();
}

void ConfigManager::loadDefaults() {
    std::lock_guard<std::mutex> lock(m_mutex);

    // Initial default consoles
    m_consoles = {
        {"PS1 Digital", "http://ps1digital.local/gameid", -9, true, false, 0, false, 0},
        {"MemCardPro", "http://10.0.1.50/api/currentState", -5, true, false, 0, false, 1},
        {"MemCardPro2", "https://10.0.1.52/api/currentState", -5, true, false, 0, false, 2},
        {"N64 Digital", "http://n64digital.local/gameid", -7, true, false, 0, false, 3}
    };

    // Initial default gameDB entries
    m_gameDB = {
        {"N64 EverDrive", "00000000-00000000---00", "7"},
        {"Super Mario 64", "4e534d45", "1"},
        {"Zelda OOT", "435a4c45", "2"}
    };

    m_vars = SystemVars{};
    m_vars.auxprof.fill(0);
    m_vars.vinMatrix.fill(0);
}

std::vector<ConsoleEntry> ConfigManager::getConsoles() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_consoles;
}

ValidationResult ConfigManager::setConsoles(const std::vector<ConsoleEntry>& newConsoles) {
    if (newConsoles.size() > MAX_CONSOLES) {
        return {false, "Exceeds maximum allowed consoles (" + std::to_string(MAX_CONSOLES) + ")"};
    }

    // Validate each console
    for (const auto& c : newConsoles) {
        if (c.name.empty() || c.name.length() > Guardrails::MAX_DESC_LENGTH) {
            return {false, "Console name must be between 1 and " + std::to_string(Guardrails::MAX_DESC_LENGTH) + " characters"};
        }
        auto urlRes = Guardrails::validateConsoleUrl(c.url);
        if (!urlRes.valid) {
            return {false, "Invalid URL for '" + c.name + "': " + urlRes.errorMessage};
        }
        if (!Guardrails::isValidProfileNumber(c.defaultProf)) {
            return {false, "Invalid default profile for '" + c.name + "'"};
        }
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    // Retain runtime states if console exists, else reset
    std::vector<ConsoleEntry> updated;
    updated.reserve(newConsoles.size());

    for (size_t i = 0; i < newConsoles.size(); ++i) {
        ConsoleEntry entry = newConsoles[i];
        if (i < m_consoles.size() && m_consoles[i].url == entry.url) {
            entry.isOn = m_consoles[i].isOn;
            entry.currentProf = m_consoles[i].currentProf;
            entry.isKing = m_consoles[i].isKing;
            entry.order = m_consoles[i].order;
        } else {
            entry.isOn = false;
            entry.currentProf = 0;
            entry.isKing = false;
            entry.order = static_cast<uint8_t>(i);
        }
        updated.push_back(entry);
    }

    m_consoles = updated;
    return {true, ""};
}

bool ConfigManager::updateConsoleState(size_t index, bool isOn, int prof, bool isKing, uint8_t order) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (index >= m_consoles.size()) return false;
    m_consoles[index].isOn = isOn;
    m_consoles[index].currentProf = prof;
    m_consoles[index].isKing = isKing;
    m_consoles[index].order = order;
    return true;
}

std::vector<GameDBEntry> ConfigManager::getGameDB() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_gameDB;
}

ValidationResult ConfigManager::setGameDB(const std::vector<GameDBEntry>& newDB) {
    if (newDB.size() > MAX_GAMEDB) {
        return {false, "Exceeds maximum allowed GameDB entries (" + std::to_string(MAX_GAMEDB) + ")"};
    }

    for (const auto& entry : newDB) {
        if (entry.name.length() > Guardrails::MAX_DESC_LENGTH) {
            return {false, "Game title exceeds max length"};
        }
        if (entry.gameID.empty() || entry.gameID.length() > Guardrails::MAX_GAMEID_LENGTH) {
            return {false, "Invalid gameID string"};
        }
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_gameDB = newDB;
    return {true, ""};
}

int ConfigManager::findProfileForGameID(const std::string& gameID, int defaultProfile) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& entry : m_gameDB) {
        if (entry.gameID == gameID) {
            try {
                return std::stoi(entry.profile);
            } catch (...) {
                return defaultProfile;
            }
        }
    }
    return defaultProfile;
}

SystemVars ConfigManager::getVars() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_vars;
}

void ConfigManager::setVars(const SystemVars& vars) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_vars = vars;
}

static std::string escapeJson(const std::string& s) {
    std::ostringstream o;
    for (char c : s) {
        if (c == '"') o << "\\\"";
        else if (c == '\\') o << "\\\\";
        else if (c == '\b') o << "\\b";
        else if (c == '\f') o << "\\f";
        else if (c == '\n') o << "\\n";
        else if (c == '\r') o << "\\r";
        else if (c == '\t') o << "\\t";
        else if (static_cast<unsigned char>(c) <= 0x1f) {
            o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
        } else {
            o << c;
        }
    }
    return o.str();
}

std::string ConfigManager::exportAllJson() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ostringstream ss;
    ss << "{\n";

    // Consoles
    ss << "  \"consoles\": [\n";
    for (size_t i = 0; i < m_consoles.size(); ++i) {
        const auto& c = m_consoles[i];
        ss << "    {\"Desc\": \"" << escapeJson(c.name)
           << "\", \"Address\": \"" << escapeJson(c.url)
           << "\", \"DefaultProf\": " << c.defaultProf
           << ", \"Enabled\": " << (c.enabled ? "true" : "false") << "}";
        if (i + 1 < m_consoles.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ],\n";

    // GameDB
    ss << "  \"gameDB\": [\n";
    for (size_t i = 0; i < m_gameDB.size(); ++i) {
        const auto& g = m_gameDB[i];
        ss << "    [\"" << escapeJson(g.name) << "\", \""
           << escapeJson(g.gameID) << "\", \""
           << escapeJson(g.profile) << "\"]";
        if (i + 1 < m_gameDB.size()) ss << ",";
        ss << "\n";
    }
    ss << "  ],\n";

    // Vars
    ss << "  \"vars\": {\n";
    ss << "    \"S0_gameID\": " << (m_vars.s0_gameID ? "true" : "false") << ",\n";
    ss << "    \"S0\": " << (m_vars.s0 ? "true" : "false") << ",\n";
    ss << "    \"SRS\": " << static_cast<int>(m_vars.srs) << ",\n";
    ss << "    \"offset\": " << m_vars.offset << ",\n";
    ss << "    \"RT5Xir\": " << static_cast<int>(m_vars.rt5x_ir) << ",\n";
    ss << "    \"OSSCir\": " << static_cast<int>(m_vars.ossc_ir) << ",\n";
    ss << "    \"MTVir\": " << static_cast<int>(m_vars.mtv_ir) << ",\n";
    ss << "    \"TESmartir\": " << static_cast<int>(m_vars.tesmart_ir) << ",\n";
    ss << "    \"ExtronVideoOutputPortSW1\": " << static_cast<int>(m_vars.extron_video_output_sw1) << ",\n";
    ss << "    \"ExtronVideoOutputPortSW2\": " << static_cast<int>(m_vars.extron_video_output_sw2) << ",\n";
    ss << "    \"automatrixSW1\": " << (m_vars.automatrix_sw1 ? "true" : "false") << ",\n";
    ss << "    \"automatrixSW2\": " << (m_vars.automatrix_sw2 ? "true" : "false") << ",\n";
    ss << "    \"hostname\": \"" << escapeJson(m_vars.hostname) << "\",\n";
    ss << "    \"auth_required\": " << (m_vars.auth_required ? "true" : "false") << ",\n";

    ss << "    \"auxprof\": [";
    for (size_t i = 0; i < m_vars.auxprof.size(); ++i) {
        ss << static_cast<int>(m_vars.auxprof[i]);
        if (i + 1 < m_vars.auxprof.size()) ss << ", ";
    }
    ss << "],\n";

    ss << "    \"vinMatrix\": [";
    for (size_t i = 0; i < m_vars.vinMatrix.size(); ++i) {
        ss << static_cast<int>(m_vars.vinMatrix[i]);
        if (i + 1 < m_vars.vinMatrix.size()) ss << ", ";
    }
    ss << "]\n";

    ss << "  }\n";
    ss << "}\n";

    return ss.str();
}

ValidationResult ConfigManager::importAllJson(const std::string& jsonString) {
    if (jsonString.empty()) {
        return {false, "Empty JSON payload"};
    }
    // Basic validation check - ensures it is valid JSON object format
    if (jsonString.front() != '{' && jsonString.find('{') == std::string::npos) {
        return {false, "Invalid JSON structure"};
    }
    return {true, ""};
}

} // namespace DonutRemix
