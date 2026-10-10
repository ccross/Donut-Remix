#include "../src/GameIDEngine.h"
#include <iostream>
#include <cassert>

using namespace DonutRemix;

void testPayloadExtraction() {
    std::cout << "[Test] GameID Payload Extraction..." << std::endl;

    // JSON format from PS1Digital / N64Digital
    std::string jsonPayload = "{\"gameID\": \"SLUS-00067\", \"title\": \"Crash Bandicoot\"}";
    assert(GameIDEngine::extractGameIdFromPayload(jsonPayload) == "SLUS-00067");

    // JSON with spaces
    std::string jsonSpaces = "{\"status\":\"ok\", \"gameID\" : \"435a4c45\" }";
    assert(GameIDEngine::extractGameIdFromPayload(jsonSpaces) == "435a4c45");

    // Plain text payload
    std::string textPayload = "SLUS-00594\r\n";
    assert(GameIDEngine::extractGameIdFromPayload(textPayload) == "SLUS-00594");

    // Empty / invalid payload
    assert(GameIDEngine::extractGameIdFromPayload("").empty());
    assert(GameIDEngine::extractGameIdFromPayload("<html><body>404 Not Found</body></html>").empty());

    std::cout << "  -> Passed!" << std::endl;
}

int main() {
    testPayloadExtraction();
    std::cout << "All GameIDEngine tests passed successfully!" << std::endl;
    return 0;
}
