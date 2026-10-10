#include "Guardrails.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace DonutRemix {

ValidationResult Guardrails::validateConsoleUrl(const std::string& url) {
    if (url.empty()) {
        return {false, "URL cannot be empty"};
    }
    if (url.length() > MAX_URL_LENGTH) {
        return {false, "URL exceeds maximum length of 256 characters"};
    }

    bool isHttp = (url.rfind("http://", 0) == 0);
    bool isHttps = (url.rfind("https://", 0) == 0);

    if (!isHttp && !isHttps) {
        return {false, "URL must start with http:// or https://"};
    }

    size_t schemeEnd = isHttps ? 8 : 7;
    std::string remaining = url.substr(schemeEnd);

    // Extract host and optional port
    size_t pathStart = remaining.find('/');
    std::string hostPort = (pathStart == std::string::npos) ? remaining : remaining.substr(0, pathStart);

    if (hostPort.empty()) {
        return {false, "URL missing host"};
    }

    std::string host = hostPort;
    int port = isHttps ? 443 : 80;

    size_t colonPos = hostPort.find(':');
    if (colonPos != std::string::npos) {
        host = hostPort.substr(0, colonPos);
        std::string portStr = hostPort.substr(colonPos + 1);
        try {
            port = std::stoi(portStr);
            if (port < 1 || port > 65535) {
                return {false, "Port must be between 1 and 65535"};
            }
        } catch (...) {
            return {false, "Invalid port format"};
        }
    }

    // Convert host to lower case
    std::transform(host.begin(), host.end(), host.begin(), [](unsigned char c) { return std::tolower(c); });

    // Reject localhost / loopback
    if (host == "localhost" || host == "127.0.0.1" || host == "::1") {
        return {false, "Loopback addresses are disallowed for security"};
    }

    // Reject cloud metadata & link-local discovery SSRF targets
    if (host == "169.254.169.254" || host == "metadata.google.internal") {
        return {false, "Cloud metadata endpoints are disallowed"};
    }

    // Check IP restrictions
    if (isRestrictedIp(host)) {
        return {false, "Host IP is in a restricted or reserved range"};
    }

    // Disallow dangerous ports often abused in SSRF (e.g. SSH, Telnet, SMTP)
    if (port == 22 || port == 23 || port == 25 || port == 110 || port == 143 || port == 3389) {
        return {false, "Target port is disallowed for safety"};
    }

    return {true, ""};
}

ValidationResult Guardrails::validateHostname(const std::string& hostname) {
    if (hostname.empty()) {
        return {false, "Hostname cannot be empty"};
    }
    if (hostname.length() > 32) {
        return {false, "Hostname exceeds maximum length of 32 characters"};
    }
    if (hostname.front() == '-' || hostname.back() == '-') {
        return {false, "Hostname cannot start or end with a hyphen"};
    }
    for (char c : hostname) {
        bool isAlpha = (c >= 'a' && c <= 'z');
        bool isDigit = (c >= '0' && c <= '9');
        bool isHyphen = (c == '-');
        if (!isAlpha && !isDigit && !isHyphen) {
            return {false, "Hostname can only contain lowercase letters 'a-z', '0-9', and '-'"};
        }
    }
    return {true, ""};
}

bool Guardrails::isRestrictedIp(const std::string& ipStr) {
    // Check if string is an IPv4 address
    int octets[4];
    char dot1, dot2, dot3;
    std::istringstream ss(ipStr);
    if ((ss >> octets[0] >> dot1 >> octets[1] >> dot2 >> octets[2] >> dot3 >> octets[3]) &&
        dot1 == '.' && dot2 == '.' && dot3 == '.' && ss.eof()) {
        for (int i = 0; i < 4; ++i) {
            if (octets[i] < 0 || octets[i] > 255) return true;
        }

        // Loopback 127.0.0.0/8
        if (octets[0] == 127) return true;

        // Current network 0.0.0.0/8
        if (octets[0] == 0) return true;

        // Link local / Auto-IP 169.254.0.0/16
        if (octets[0] == 169 && octets[1] == 254) return true;

        // Multicast 224.0.0.0/4
        if (octets[0] >= 224 && octets[0] <= 239) return true;

        // Broadcast / Future use 240.0.0.0/4
        if (octets[0] >= 240) return true;
    }
    return false;
}

ValidationResult Guardrails::sanitizeCommand(const std::string& rawCmd, std::string& cleanCmd) {
    if (rawCmd.empty()) {
        cleanCmd.clear();
        return {false, "Command cannot be empty"};
    }
    if (rawCmd.length() > MAX_COMMAND_LENGTH) {
        cleanCmd.clear();
        return {false, "Command exceeds maximum length of 256 characters"};
    }

    cleanCmd.clear();
    cleanCmd.reserve(rawCmd.length());

    for (char c : rawCmd) {
        unsigned char uc = static_cast<unsigned char>(c);
        // Disallow null bytes and non-printable control characters (except newline, carriage return, tab)
        if (uc < 32 && uc != '\r' && uc != '\n' && uc != '\t') {
            return {false, "Command contains illegal control characters"};
        }
        cleanCmd.push_back(c);
    }

    return {true, ""};
}

bool Guardrails::isValidProfileNumber(int profile) {
    // SVS profiles: 0 to 999. Remote profiles: -12 to -1.
    return (profile >= -12 && profile <= 999);
}

bool Guardrails::isSafeFilePath(const std::string& path) {
    if (path.empty() || path.length() > MAX_FILENAME_LENGTH) return false;
    // Disallow directory traversal
    if (path.find("..") != std::string::npos) return false;
    if (path.find('\\') != std::string::npos) return false;
    // Disallow illegal characters in filenames
    for (char c : path) {
        if (c < 32 || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            return false;
        }
    }
    return true;
}

} // namespace DonutRemix
