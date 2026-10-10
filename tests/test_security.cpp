#include "../src/Security.h"
#include <iostream>
#include <cassert>

using namespace DonutRemix;

void testSha256() {
    std::cout << "[Test] SHA-256 Hashing..." << std::endl;
    // Known test vector: "abc"
    // sha256("abc") = ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
    std::string hash = SecurityManager::sha256Hex("abc");
    assert(hash == "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");

    // Empty string
    // sha256("") = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
    std::string emptyHash = SecurityManager::sha256Hex("");
    assert(emptyHash == "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

    std::cout << "  -> Passed!" << std::endl;
}

void testPasswordAuthentication() {
    std::cout << "[Test] Password & Salt Authentication..." << std::endl;
    SecurityManager sec;

    // Default password
    assert(sec.checkPassword("donutremix"));
    assert(!sec.checkPassword("wrongpassword"));
    assert(!sec.checkPassword(""));

    // Change password
    sec.setPassword("SuperSecretPassword123!");
    assert(sec.checkPassword("SuperSecretPassword123!"));
    assert(!sec.checkPassword("donutremix"));

    // Timing-safe equals
    assert(SecurityManager::timingSafeEquals("testtoken123", "testtoken123"));
    assert(!SecurityManager::timingSafeEquals("testtoken123", "testtoken124"));
    assert(!SecurityManager::timingSafeEquals("testtoken123", "short"));

    std::cout << "  -> Passed!" << std::endl;
}

void testSessionManagement() {
    std::cout << "[Test] Session Management..." << std::endl;
    SecurityManager sec;
    uint32_t now = 100000;

    std::string token1 = sec.createSession(now);
    assert(!token1.empty());
    assert(sec.isValidSession(token1, now + 1000));

    // Token should expire after SESSION_TTL_MS
    assert(!sec.isValidSession(token1, now + SecurityManager::SESSION_TTL_MS + 5000));

    // Invalidate session
    std::string token2 = sec.createSession(now);
    assert(sec.isValidSession(token2, now + 100));
    sec.invalidateSession(token2);
    assert(!sec.isValidSession(token2, now + 200));

    std::cout << "  -> Passed!" << std::endl;
}

void testRateLimiting() {
    std::cout << "[Test] Brute Force Rate Limiting..." << std::endl;
    SecurityManager sec;
    std::string ip = "192.168.1.100";
    uint32_t now = 50000;

    assert(!sec.isRateLimited(ip, now));

    // 4 failed attempts - not yet locked out
    for (int i = 0; i < 4; ++i) {
        sec.recordFailedLogin(ip, now);
        assert(!sec.isRateLimited(ip, now));
    }

    // 5th failed attempt - should be locked out!
    sec.recordFailedLogin(ip, now);
    assert(sec.isRateLimited(ip, now));
    assert(sec.isRateLimited(ip, now + 30000)); // 30s later, still locked out

    // After lockout duration expires
    assert(!sec.isRateLimited(ip, now + SecurityManager::LOCKOUT_DURATION_MS + 1000));

    // Successful login clears rate limit
    sec.recordFailedLogin(ip, now + 70000);
    sec.recordSuccessfulLogin(ip);
    assert(!sec.isRateLimited(ip, now + 70000));

    std::cout << "  -> Passed!" << std::endl;
}

int main() {
    testSha256();
    testPasswordAuthentication();
    testSessionManagement();
    testRateLimiting();
    std::cout << "All Security tests passed successfully!" << std::endl;
    return 0;
}
