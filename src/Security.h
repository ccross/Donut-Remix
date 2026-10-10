#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace DonutRemix {

struct Session {
    std::string token;
    uint32_t createdAtMs;
    uint32_t lastActiveMs;
};

struct RateLimitEntry {
    uint32_t failedAttempts;
    uint32_t lockoutUntilMs;
    uint32_t lastAttemptMs;
};

class SecurityManager {
public:
    static constexpr uint32_t SESSION_TTL_MS = 24 * 3600 * 1000UL; // 24 hours
    static constexpr uint32_t MAX_SESSIONS = 8;
    static constexpr uint32_t MAX_FAILED_ATTEMPTS = 5;
    static constexpr uint32_t LOCKOUT_DURATION_MS = 60 * 1000UL; // 60s

    SecurityManager();

    // Password Management
    bool checkPassword(const std::string& inputPassword) const;
    void setPassword(const std::string& newPassword);
    std::string getPasswordHash() const { return m_passwordHash; }
    std::string getPasswordSalt() const { return m_passwordSalt; }
    void loadCredentials(const std::string& hash, const std::string& salt);
    bool isAuthRequired() const { return m_authRequired; }
    void setAuthRequired(bool required) { m_authRequired = required; }

    // Session Management
    std::string createSession(uint32_t nowMs);
    bool isValidSession(const std::string& token, uint32_t nowMs);
    void invalidateSession(const std::string& token);
    void cleanExpiredSessions(uint32_t nowMs);

    // Rate Limiting
    bool isRateLimited(const std::string& clientIp, uint32_t nowMs) const;
    void recordFailedLogin(const std::string& clientIp, uint32_t nowMs);
    void recordSuccessfulLogin(const std::string& clientIp);

    // Utilities
    static std::string sha256Hex(const std::string& input);
    static std::string generateRandomHex(size_t byteCount);
    static bool timingSafeEquals(const std::string& a, const std::string& b);

private:
    std::string m_passwordHash;
    std::string m_passwordSalt;
    bool m_authRequired;

    std::vector<Session> m_activeSessions;
    std::unordered_map<std::string, RateLimitEntry> m_rateLimits;
};

} // namespace DonutRemix
