#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <cstdint>
#include <mutex>
#include "Guardrails.h"

namespace DonutRemix {

struct ConsoleEntry {
    std::string name;
    std::string url;
    int defaultProf;
    bool enabled;

    // Runtime state (not persisted)
    bool isOn{false};
    int currentProf{0};
    bool isKing{false};
    uint8_t order{0};
};

struct GameDBEntry {
    std::string name;
    std::string gameID;
    std::string profile; // stored as string for compatibility, parsed to int
};

struct SystemVars {
    bool s0_gameID{false};
    bool s0{false};
    uint8_t srs{1};
    uint16_t offset{0};
    uint8_t rt5x_ir{0};
    uint8_t ossc_ir{0};
    uint8_t mtv_ir{0};
    uint8_t tesmart_ir{0};
    uint8_t extron_video_output_sw1{1};
    uint8_t extron_video_output_sw2{1};
    bool automatrix_sw1{false};
    bool automatrix_sw2{false};
    std::array<uint8_t, 12> auxprof{};
    std::array<uint8_t, 65> vinMatrix{};
    std::string hostname{"donutshop"};
    bool auth_required{true};
    std::string password_hash;
    std::string password_salt;
};

class ConfigManager {
public:
    static constexpr size_t MAX_CONSOLES = 16;
    static constexpr size_t MAX_GAMEDB = 2000;

    ConfigManager();

    // Thread-safe access to consoles
    std::vector<ConsoleEntry> getConsoles();
    ValidationResult setConsoles(const std::vector<ConsoleEntry>& newConsoles);
    bool updateConsoleState(size_t index, bool isOn, int prof, bool isKing, uint8_t order);

    // Thread-safe access to GameDB
    std::vector<GameDBEntry> getGameDB();
    ValidationResult setGameDB(const std::vector<GameDBEntry>& newDB);
    int findProfileForGameID(const std::string& gameID, int defaultProfile);

    // Thread-safe access to SystemVars
    SystemVars getVars();
    void setVars(const SystemVars& vars);

    // Thread-safe access to SVS Input Names
    std::map<uint8_t, std::string> getSvsInputNames();
    void setSvsInputName(uint8_t input, const std::string& name);
    void setSvsInputNames(const std::map<uint8_t, std::string>& names);

    // JSON serialization / deserialization helpers
    std::string exportAllJson();
    ValidationResult importAllJson(const std::string& jsonString);

    // Lock for multi-step atomic operations
    std::mutex& getMutex() { return m_mutex; }

private:
    std::mutex m_mutex;
    std::vector<ConsoleEntry> m_consoles;
    std::vector<GameDBEntry> m_gameDB;
    SystemVars m_vars;
    std::map<uint8_t, std::string> m_svsInputNames;

    void loadDefaults();
    void loadSvsNamesFromStorage();
    void saveSvsNamesToStorage();
};

} // namespace DonutRemix
