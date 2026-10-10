#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace DonutRemix {

struct ValidationResult {
    bool valid;
    std::string errorMessage;
};

class Guardrails {
public:
    // Max lengths
    static constexpr size_t MAX_URL_LENGTH = 256;
    static constexpr size_t MAX_COMMAND_LENGTH = 256;
    static constexpr size_t MAX_GAMEID_LENGTH = 64;
    static constexpr size_t MAX_DESC_LENGTH = 64;
    static constexpr size_t MAX_FILENAME_LENGTH = 128;

    // URL & SSRF Validation
    static ValidationResult validateConsoleUrl(const std::string& url);

    // Hostname Validation (per v0.7.14 rules)
    static ValidationResult validateHostname(const std::string& hostname);

    // Command Validation & Sanitization
    static ValidationResult sanitizeCommand(const std::string& rawCmd, std::string& cleanCmd);

    // Profile & Number Validation
    static bool isValidProfileNumber(int profile);

    // Helper: Checks if an IPv4 string belongs to restricted/dangerous networks
    static bool isRestrictedIp(const std::string& ipStr);

    // Helper: Validates path strings to prevent directory traversal
    static bool isSafeFilePath(const std::string& path);
};

} // namespace DonutRemix
