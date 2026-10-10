#include "../src/Guardrails.h"
#include <iostream>
#include <cassert>

using namespace DonutRemix;

void testUrlValidation() {
    std::cout << "[Test] URL Validation..." << std::endl;

    // Valid URLs
    assert(Guardrails::validateConsoleUrl("http://192.168.1.50/gameid").valid);
    assert(Guardrails::validateConsoleUrl("https://10.0.1.52/api/currentState").valid);
    assert(Guardrails::validateConsoleUrl("http://ps1digital.local/gameid").valid);
    assert(Guardrails::validateConsoleUrl("http://192.168.1.100:8080/status").valid);

    // Empty URL
    assert(!Guardrails::validateConsoleUrl("").valid);

    // Invalid scheme
    assert(!Guardrails::validateConsoleUrl("ftp://192.168.1.50/").valid);
    assert(!Guardrails::validateConsoleUrl("javascript:alert(1)").valid);
    assert(!Guardrails::validateConsoleUrl("file:///etc/passwd").valid);

    // Loopback / SSRF
    assert(!Guardrails::validateConsoleUrl("http://127.0.0.1/gameid").valid);
    assert(!Guardrails::validateConsoleUrl("http://localhost/gameid").valid);
    assert(!Guardrails::validateConsoleUrl("http://::1/gameid").valid);

    // Cloud metadata SSRF
    assert(!Guardrails::validateConsoleUrl("http://169.254.169.254/latest/meta-data").valid);

    // Multicast / Broadcast
    assert(!Guardrails::validateConsoleUrl("http://224.0.0.1/").valid);
    assert(!Guardrails::validateConsoleUrl("http://255.255.255.255/").valid);

    // Disallowed ports
    assert(!Guardrails::validateConsoleUrl("http://192.168.1.1:22/").valid);
    assert(!Guardrails::validateConsoleUrl("http://192.168.1.1:23/").valid);

    std::cout << "  -> Passed!" << std::endl;
}

void testHostnameValidation() {
    std::cout << "[Test] Hostname Validation..." << std::endl;

    assert(Guardrails::validateHostname("donutshop").valid);
    assert(Guardrails::validateHostname("donut-remix-1").valid);
    assert(Guardrails::validateHostname("rt4k-bridge").valid);

    // Invalid hostnames
    assert(!Guardrails::validateHostname("").valid);
    assert(!Guardrails::validateHostname("-donut").valid);
    assert(!Guardrails::validateHostname("donut-").valid);
    assert(!Guardrails::validateHostname("DonutShop").valid); // uppercase disallowed per rules
    assert(!Guardrails::validateHostname("donut.shop").valid); // dots disallowed
    assert(!Guardrails::validateHostname("donut_shop").valid); // underscores disallowed

    std::cout << "  -> Passed!" << std::endl;
}

void testCommandSanitization() {
    std::cout << "[Test] Command Sanitization..." << std::endl;

    std::string clean;
    // Valid commands
    assert(Guardrails::sanitizeCommand("remote prof1", clean).valid);
    assert(clean == "remote prof1");

    assert(Guardrails::sanitizeCommand("SVS NEW INPUT=5\r\n", clean).valid);

    // Empty command
    assert(!Guardrails::sanitizeCommand("", clean).valid);

    // Control characters (null byte injection)
    std::string badCmd = "pwr on\0rm -rf";
    badCmd.push_back('\0');
    assert(!Guardrails::sanitizeCommand(badCmd, clean).valid);

    // File path traversal
    assert(Guardrails::isSafeFilePath("profile.rt4"));
    assert(Guardrails::isSafeFilePath("profile/SVS/S1_SNES.rt4"));
    assert(Guardrails::isSafeFilePath("dir/file.txt"));
    assert(!Guardrails::isSafeFilePath("../../../etc/passwd"));
    assert(!Guardrails::isSafeFilePath("dir/../file.txt"));
    assert(!Guardrails::isSafeFilePath("file*?.txt"));

    std::cout << "  -> Passed!" << std::endl;
}

int main() {
    testUrlValidation();
    testHostnameValidation();
    testCommandSanitization();
    std::cout << "All Guardrails tests passed successfully!" << std::endl;
    return 0;
}
