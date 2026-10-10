#include "../src/Config.h"
#include <iostream>
#include <cassert>

using namespace DonutRemix;

void testConfigDefaults() {
    std::cout << "[Test] Config Defaults..." << std::endl;
    ConfigManager cfg;

    auto consoles = cfg.getConsoles();
    assert(consoles.size() == 4);
    assert(consoles[0].name == "PS1 Digital");

    auto db = cfg.getGameDB();
    assert(db.size() >= 3);

    auto vars = cfg.getVars();
    assert(vars.hostname == "donutshop");
    assert(vars.srs == 1);

    std::cout << "  -> Passed!" << std::endl;
}

void testConsoleBoundsAndValidation() {
    std::cout << "[Test] Console Bounds & Validation..." << std::endl;
    ConfigManager cfg;

    // Try setting 17 consoles (exceeds MAX_CONSOLES of 16)
    std::vector<ConsoleEntry> tooMany;
    for (size_t i = 0; i < 17; ++i) {
        tooMany.push_back({"Console " + std::to_string(i), "http://192.168.1." + std::to_string(i + 10) + "/gameid", 1, true, false, 0, false, 0});
    }
    auto resTooMany = cfg.setConsoles(tooMany);
    assert(!resTooMany.valid);
    assert(resTooMany.errorMessage.find("Exceeds maximum") != std::string::npos);

    // Try setting console with invalid SSRF loopback URL
    std::vector<ConsoleEntry> badUrlList = {
        {"Hacker Console", "http://127.0.0.1/gameid", 1, true, false, 0, false, 0}
    };
    auto resBadUrl = cfg.setConsoles(badUrlList);
    assert(!resBadUrl.valid);

    // Valid list of 2 consoles
    std::vector<ConsoleEntry> validList = {
        {"PS1", "http://192.168.1.50/gameid", 2, true, false, 0, false, 0},
        {"N64", "http://192.168.1.60/gameid", 4, true, false, 0, false, 1}
    };
    auto resValid = cfg.setConsoles(validList);
    assert(resValid.valid);
    assert(cfg.getConsoles().size() == 2);

    std::cout << "  -> Passed!" << std::endl;
}

void testGameDBLookup() {
    std::cout << "[Test] GameDB Lookup..." << std::endl;
    ConfigManager cfg;

    // Default profile lookup
    int prof = cfg.findProfileForGameID("00000000-00000000---00", -1);
    assert(prof == 7);

    // Unknown gameID returns default
    int unknown = cfg.findProfileForGameID("unknown-id-1234", 99);
    assert(unknown == 99);

    std::cout << "  -> Passed!" << std::endl;
}

void testJsonExport() {
    std::cout << "[Test] JSON Export..." << std::endl;
    ConfigManager cfg;

    std::string json = cfg.exportAllJson();
    assert(!json.empty());
    assert(json.find("\"consoles\":") != std::string::npos);
    assert(json.find("\"gameDB\":") != std::string::npos);
    assert(json.find("\"vars\":") != std::string::npos);

    std::cout << "  -> Passed!" << std::endl;
}

int main() {
    testConfigDefaults();
    testConsoleBoundsAndValidation();
    testGameDBLookup();
    testJsonExport();
    std::cout << "All Config tests passed successfully!" << std::endl;
    return 0;
}
