#include "WebServerManager.h"
#include "WebUI.h"
#include "WebCtlAssets.h"
#include "Guardrails.h"
#include <iostream>

#ifdef ARDUINO
#include <ArduinoJson.h>
#include <Update.h>
#if defined(USB_MODE) && USB_MODE
#include <EspUsbHost.h>
extern EspUsbHost usbHost;
extern EspUsbHostCdcSerial CdcSerial;
#endif
#endif

namespace DonutRemix {

WebServerManager::WebServerManager(ConfigManager& config, SecurityManager& security, DeviceManager& devices, GameIDEngine& gameId)
    : m_config(config), m_security(security), m_devices(devices), m_gameId(gameId)
#ifdef ARDUINO
    , m_server(80)
#endif
{}

void WebServerManager::begin() {
#ifdef ARDUINO
    const char* headerkeys[] = {"Authorization", "X-Session-Token"};
    m_server.collectHeaders(headerkeys, 2);

    // Full WebCtl UI and GameID pages
    m_server.on("/", HTTP_GET, [this]() { handleRoot(); });
    m_server.on("/gameid", HTTP_GET, [this]() { handleGameID(); });
    m_server.on("/term", HTTP_GET, [this]() { handleGameID(); });
    m_server.on("/donut-memory", HTTP_GET, [this]() { handleDonutMemory(); });
    m_server.on("/favicon.ico", HTTP_GET, [this]() { m_server.send(204); });
    m_server.on("/rt4k-fw-meta", HTTP_GET, [this]() {
        String ver = m_server.hasArg("version") ? m_server.arg("version") : "1.96.0";
        JsonDocument doc;
        doc["ok"] = true;
        doc["updater"] = "rt4kup.bin";
        JsonArray vers = doc["versions"].to<JsonArray>();
        vers.add("1.96.0");
        vers.add("1.95.0");
        vers.add("1.94.0");
        vers.add("1.93.2");
        vers.add("1.93.1");
        vers.add("1.93.0");

        if (ver == "1.95.0") {
            doc["version"] = "1.95.0";
            doc["packageUrl"] = "https://cdn.jsdelivr.net/gh/retrotink-llc/firmware@main/RetroTINK-4K/Experimental/rt4k_1950.zip";
            doc["sha256"] = "28e46bd713d594dd798a42ec2bfe03bdbc258326fdea19c0353730b60844c564";
            doc["changelog"] = "Version 1.95.0 (2026-10-08)";
        } else {
            doc["version"] = "1.96.0";
            doc["packageUrl"] = "https://cdn.jsdelivr.net/gh/retrotink-llc/firmware@main/RetroTINK-4K/Experimental/rt4k_1960.zip";
            doc["sha256"] = "0ad8c8c0fa5597ee054d87f45b882e3fc47c76a5fb26089678527aec5c71c8e5";
            doc["changelog"] = "Version 1.96.0 (2026-10-09):\n- Second-generation inverse telecine engine for RT4KPRO\n- Dual FPGA and MCU detection state machines for fast lock / unlock\n- Significantly improved inverse 3:2 and 2:2 reliability";
        }
        String out;
        serializeJson(doc, out);
        m_server.send(200, "application/json", out);
    });

    // REST API v1
    m_server.on("/api/v1/auth/login", HTTP_POST, [this]() { handleAuthLogin(); });
    m_server.on("/api/v1/auth/logout", HTTP_POST, [this]() { handleAuthLogout(); });
    m_server.on("/api/v1/auth/status", HTTP_GET, [this]() { handleAuthStatus(); });
    m_server.on("/api/v1/status", HTTP_GET, [this]() { handleGetStatus(); });
    m_server.on("/api/v1/consoles", HTTP_GET, [this]() { handleGetConsoles(); });
    m_server.on("/api/v1/consoles", HTTP_POST, [this]() { handlePostConsoles(); });
    m_server.on("/api/v1/gamedb", HTTP_GET, [this]() { handleGetGameDB(); });
    m_server.on("/api/v1/gamedb", HTTP_POST, [this]() { handlePostGameDB(); });
    m_server.on("/api/v1/vars", HTTP_GET, [this]() { handleGetVars(); });
    m_server.on("/api/v1/vars", HTTP_POST, [this]() { handlePostVars(); });
    m_server.on("/api/v1/command", HTTP_POST, [this]() { handlePostCommand(); });
    m_server.on("/api/v1/system/backup", HTTP_GET, [this]() { handleSystemBackup(); });
    m_server.on("/api/v1/system/restore", HTTP_POST, [this]() { handleSystemRestore(); });
    m_server.on("/api/v1/system/update", HTTP_POST, [this]() { handleFirmwareUpdate(); }, [this]() { handleFirmwareUpload(); });

    // WebCtl & Backward Compatibility endpoints
    m_server.on("/cmd", HTTP_POST, [this]() { handleLegacyCmd(); });
    m_server.on("/getConsoles", HTTP_GET, [this]() { handleGetConsoles(); });
    m_server.on("/updateConsoles", HTTP_POST, [this]() { handlePostConsoles(); });
    m_server.on("/getGameDB", HTTP_GET, [this]() { handleGetGameDB(); });
    m_server.on("/updateGameDB", HTTP_POST, [this]() { handlePostGameDB(); });
    m_server.on("/getVars", HTTP_GET, [this]() { handleGetVars(); });
    m_server.on("/updateVars", HTTP_POST, [this]() { handlePostVars(); });
    m_server.on("/exportAll", HTTP_GET, [this]() { handleSystemBackup(); });
    m_server.on("/importAll", HTTP_POST, [this]() { handleSystemRestore(); });
    m_server.on("/update", HTTP_POST, [this]() { handleFirmwareUpdate(); }, [this]() { handleFirmwareUpload(); });

    m_server.on("/api/info", HTTP_GET, [this]() { handleWebCtlInfo(); });
    m_server.on("/api/status", HTTP_GET, [this]() { handleWebCtlStatus(); });
    m_server.on("/api/command", HTTP_POST, [this]() { handleWebCtlCommand(); });
    m_server.on("/api/files/mkdir", HTTP_POST, [this]() { handleWebCtlMkdir(); });
    m_server.on("/api/files/move", HTTP_POST, [this]() { handleWebCtlMove(); });
    m_server.on("/api/files", HTTP_GET, [this]() { handleWebCtlFiles(); });
    m_server.on("/webctl-options", HTTP_GET, [this]() { handleWebCtlOptions(); });
    m_server.on("/webctl-options", HTTP_POST, [this]() { handleWebCtlOptions(); });
    m_server.on("/webctl-theme", HTTP_GET, [this]() { handleWebCtlTheme(); });
    m_server.on("/webctl-theme", HTTP_POST, [this]() { handleWebCtlTheme(); });
    m_server.on("/put", HTTP_POST, [this]() { handlePutResponse(); }, [this]() { handlePutUpload(); });
    m_server.on("/put", HTTP_OPTIONS, [this]() {
        setSecurityHeaders();
        m_server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        m_server.send(204);
    });

    m_server.begin();
#endif
}

void WebServerManager::handleClient() {
#ifdef ARDUINO
    m_server.handleClient();
#endif
}

#ifdef ARDUINO
void WebServerManager::setSecurityHeaders() {
    m_server.sendHeader("X-Frame-Options", "SAMEORIGIN");
    m_server.sendHeader("X-Content-Type-Options", "nosniff");
    m_server.sendHeader("Referrer-Policy", "no-referrer");
    m_server.sendHeader("Access-Control-Allow-Origin", "*");
    m_server.sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Session-Token");
    m_server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
}

std::string WebServerManager::getClientIp() {
    return m_server.client().remoteIP().toString().c_str();
}

bool WebServerManager::authenticateRequest() {
    if (!m_security.isAuthRequired()) {
        return true;
    }

    std::string token;
    if (m_server.hasHeader("Authorization")) {
        String auth = m_server.header("Authorization");
        if (auth.startsWith("Bearer ")) {
            token = auth.substring(7).c_str();
        }
    }
    if (token.empty() && m_server.hasHeader("X-Session-Token")) {
        token = m_server.header("X-Session-Token").c_str();
    }
    if (token.empty() && m_server.hasArg("token")) {
        token = m_server.arg("token").c_str();
    }

    if (m_security.isValidSession(token, millis())) {
        return true;
    }

    setSecurityHeaders();
    m_server.send(401, "application/json", "{\"error\":\"Unauthorized. Valid session token required.\"}");
    return false;
}

void WebServerManager::handleRoot() {
    setSecurityHeaders();
    m_server.sendHeader("Content-Encoding", "gzip");
    m_server.send_P(200, "text/html", (PGM_P)WEBCTL_HTML_GZ, WEBCTL_HTML_GZ_LEN);
}

void WebServerManager::handleGameID() {
    setSecurityHeaders();
    m_server.sendHeader("Content-Encoding", "gzip");
    m_server.send_P(200, "text/html", (PGM_P)GAMEID_HTML_GZ, GAMEID_HTML_GZ_LEN);
}

void WebServerManager::handleDonutMemory() {
    setSecurityHeaders();
    JsonDocument doc;
    doc["freeHeap"] = ESP.getFreeHeap();
    doc["minFreeHeap"] = ESP.getMinFreeHeap();
    doc["psramSize"] = ESP.getPsramSize();
    doc["freePsram"] = ESP.getFreePsram();
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleAuthLogin() {
    setSecurityHeaders();
    std::string ip = getClientIp();
    uint32_t now = millis();

    if (m_security.isRateLimited(ip, now)) {
        m_server.send(429, "application/json", "{\"error\":\"Too many failed attempts. Temporarily locked out for 60s.\"}");
        return;
    }

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing request body\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (err || !doc["password"].is<const char*>()) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid JSON or missing password field\"}");
        return;
    }

    std::string password = doc["password"].as<const char*>();
    if (m_security.checkPassword(password)) {
        m_security.recordSuccessfulLogin(ip);
        std::string token = m_security.createSession(now);
        JsonDocument resp;
        resp["token"] = token;
        resp["expiresIn"] = SecurityManager::SESSION_TTL_MS / 1000;
        String out;
        serializeJson(resp, out);
        m_server.send(200, "application/json", out);
    } else {
        m_security.recordFailedLogin(ip, now);
        m_server.send(401, "application/json", "{\"error\":\"Invalid password\"}");
    }
}

void WebServerManager::handleAuthLogout() {
    setSecurityHeaders();
    String auth = m_server.header("Authorization");
    if (auth.startsWith("Bearer ")) {
        m_security.invalidateSession(auth.substring(7).c_str());
    }
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleAuthStatus() {
    setSecurityHeaders();
    JsonDocument doc;
    doc["authRequired"] = m_security.isAuthRequired();
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleGetStatus() {
    setSecurityHeaders();
    auto consoles = m_config.getConsoles();
    int currentProf = m_devices.getCurrentProfile();
    std::string activeConsole = "None";

    for (const auto& c : consoles) {
        if (c.isKing) {
            activeConsole = c.name;
            break;
        }
    }

    JsonDocument doc;
    doc["uptime"] = (millis() / 1000);
    doc["active_profile"] = currentProf;
    doc["active_console"] = activeConsole;
    doc["rt4k_status"] = "Ready";

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleGetConsoles() {
    setSecurityHeaders();
    auto consoles = m_config.getConsoles();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& c : consoles) {
        JsonObject obj = arr.add<JsonObject>();
        obj["Desc"] = c.name;
        obj["Address"] = c.url;
        obj["DefaultProf"] = c.defaultProf;
        obj["Enabled"] = c.enabled;
        obj["On"] = c.isOn;
        obj["King"] = c.isKing;
    }

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handlePostConsoles() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (err || !doc.is<JsonArray>()) {
        m_server.send(400, "application/json", "{\"error\":\"Body must be a JSON array\"}");
        return;
    }

    JsonArray arr = doc.as<JsonArray>();
    if (arr.size() > ConfigManager::MAX_CONSOLES) {
        m_server.send(400, "application/json", "{\"error\":\"Exceeds max allowed consoles (16)\"}");
        return;
    }

    std::vector<ConsoleEntry> newList;
    for (JsonObject obj : arr) {
        ConsoleEntry entry;
        entry.name = obj["Desc"] | (obj["name"] | "");
        entry.url = obj["Address"] | (obj["url"] | "");
        entry.defaultProf = obj["DefaultProf"] | (obj["defaultProf"] | 0);
        entry.enabled = obj["Enabled"] | (obj["enabled"] | true);
        newList.push_back(entry);
    }

    auto res = m_config.setConsoles(newList);
    if (!res.valid) {
        JsonDocument errDoc;
        errDoc["error"] = res.errorMessage;
        String errOut;
        serializeJson(errDoc, errOut);
        m_server.send(400, "application/json", errOut);
        return;
    }

    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleGetGameDB() {
    setSecurityHeaders();
    auto db = m_config.getGameDB();
    JsonDocument doc;
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& g : db) {
        JsonArray item = arr.add<JsonArray>();
        item.add(g.name);
        item.add(g.gameID);
        item.add(g.profile);
    }

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handlePostGameDB() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (err || !doc.is<JsonArray>()) {
        m_server.send(400, "application/json", "{\"error\":\"Body must be a JSON array\"}");
        return;
    }

    JsonArray arr = doc.as<JsonArray>();
    if (arr.size() > ConfigManager::MAX_GAMEDB) {
        m_server.send(400, "application/json", "{\"error\":\"Exceeds max allowed gameDB entries (2000)\"}");
        return;
    }

    std::vector<GameDBEntry> newList;
    for (JsonVariant item : arr) {
        GameDBEntry g;
        if (item.is<JsonArray>()) {
            JsonArray row = item.as<JsonArray>();
            g.name = row[0] | "";
            g.gameID = row[1] | "";
            g.profile = row[2] | "";
        } else if (item.is<JsonObject>()) {
            JsonObject obj = item.as<JsonObject>();
            g.name = obj["name"] | "";
            g.gameID = obj["gameID"] | "";
            g.profile = obj["profile"] | "";
        }
        newList.push_back(g);
    }

    auto res = m_config.setGameDB(newList);
    if (!res.valid) {
        JsonDocument errDoc;
        errDoc["error"] = res.errorMessage;
        String errOut;
        serializeJson(errDoc, errOut);
        m_server.send(400, "application/json", errOut);
        return;
    }

    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleGetVars() {
    setSecurityHeaders();
    auto vars = m_config.getVars();
    JsonDocument doc;
    doc["S0_gameID"] = vars.s0_gameID;
    doc["S0"] = vars.s0;
    doc["SRS"] = vars.srs;
    doc["offset"] = vars.offset;
    doc["RT5Xir"] = vars.rt5x_ir;
    doc["OSSCir"] = vars.ossc_ir;
    doc["MTVir"] = vars.mtv_ir;
    doc["TESmartir"] = vars.tesmart_ir;
    doc["hostname"] = vars.hostname;

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handlePostVars() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }

    JsonDocument doc;
    deserializeJson(doc, m_server.arg("plain"));
    auto vars = m_config.getVars();

    if (doc["S0"].is<bool>()) vars.s0 = doc["S0"].as<bool>();
    if (doc["SRS"].is<uint8_t>()) vars.srs = doc["SRS"].as<uint8_t>();
    if (doc["offset"].is<uint16_t>()) vars.offset = doc["offset"].as<uint16_t>();

    m_config.setVars(vars);
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handlePostCommand() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();

    std::string rawCmd;
    if (m_server.hasArg("plain")) {
        JsonDocument doc;
        if (!deserializeJson(doc, m_server.arg("plain")) && doc["command"].is<const char*>()) {
            rawCmd = doc["command"].as<const char*>();
        } else {
            rawCmd = m_server.arg("plain").c_str();
        }
    }

    std::string cleanCmd;
    auto res = Guardrails::sanitizeCommand(rawCmd, cleanCmd);
    if (!res.valid) {
        m_server.send(400, "application/json", "{\"error\":\"Command rejected by guardrail\"}");
        return;
    }

    m_devices.sendRawCommand(cleanCmd);
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleLegacyCmd() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    String raw = m_server.arg("plain");
    std::string clean;
    auto res = Guardrails::sanitizeCommand(raw.c_str(), clean);
    if (!res.valid) {
        m_server.send(400, "text/plain", "Command rejected");
        return;
    }
    m_devices.sendRawCommand(clean);
    m_server.send(204);
}

void WebServerManager::handleWebCtlStatus() {
    setSecurityHeaders();
    m_server.send(200, "application/json", "{\"connected\":true,\"baud\":2000000}");
}

void WebServerManager::handleWebCtlCommand() {
    handlePostCommand();
}

void WebServerManager::handleWebCtlOptions() {
    setSecurityHeaders();
    m_server.send(200, "application/json", "{\"struct\":true,\"console\":true,\"expert\":true,\"diag\":true,\"showDonutMemory\":false}");
}

void WebServerManager::handleWebCtlInfo() {
    setSecurityHeaders();
    JsonDocument doc;
    doc["app"] = "Donut Remix";
    doc["version"] = "1.0.0-remix";
    doc["hardware"] = "Arduino Nano ESP32";
    doc["baud"] = 2000000;
    doc["rt4k"] = "connected";
#if defined(USB_MODE) && USB_MODE
    doc["usbHost_ready"] = ::usbHost.ready();
    doc["cdc_connected"] = ::CdcSerial.connected();
    doc["usb_lastError"] = ::usbHost.lastError();
    doc["usb_lastErrorName"] = ::usbHost.lastErrorName();
#endif
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleWebCtlFiles() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    JsonDocument doc;
    doc["path"] = "/";
    doc["files"] = JsonArray();
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleWebCtlMkdir() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    JsonDocument doc;
    deserializeJson(doc, m_server.arg("plain"));
    std::string path = doc["path"] | "";
    if (!Guardrails::isSafeFilePath(path)) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid or unsafe directory path\"}");
        return;
    }
    m_devices.sendRawCommand("mkdir " + path);
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleWebCtlMove() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    JsonDocument doc;
    deserializeJson(doc, m_server.arg("plain"));
    std::string src = doc["source"] | "";
    std::string dst = doc["destination"] | "";
    if (!Guardrails::isSafeFilePath(src) || !Guardrails::isSafeFilePath(dst)) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid or unsafe file path\"}");
        return;
    }
    m_devices.sendRawCommand("mv " + src + " " + dst);
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleWebCtlTheme() {
    setSecurityHeaders();
    if (m_server.method() == HTTP_POST && m_server.hasArg("plain")) {
        std::string theme = m_server.arg("plain").c_str();
        if (theme == "light") {
            m_server.send(200, "application/json", "{\"theme\":\"light\"}");
            return;
        }
    }
    m_server.send(200, "application/json", "{\"theme\":\"dark\"}");
}

void WebServerManager::handlePutUpload() {
    if (!m_server.hasRaw()) return;
    HTTPRaw& raw = m_server.raw();
    auto& session = m_devices.getHttpPutSessionRef();
    if (!session.active) return;

    if (raw.status == RAW_START) {
        session.chunkLen = 0;
        session.bytesSent = 0;
        session.framesSent = 0;
        session.seq = 0;
        session.startTime = millis();
        session.lastError.clear();
    } else if (raw.status == RAW_WRITE) {
        if (!session.lastError.empty()) return;
        const uint8_t* inPtr = raw.buf;
        size_t inRemaining = raw.currentSize;

        while (inRemaining > 0) {
            size_t space = session.payloadSize - session.chunkLen;
            size_t toCopy = std::min(inRemaining, space);
            memcpy(session.chunkBuf + session.chunkLen, inPtr, toCopy);
            session.chunkLen += toCopy;
            inPtr += toCopy;
            inRemaining -= toCopy;

            if (session.chunkLen == session.payloadSize) {
                if (!m_devices.sendRtl1Frame(session.nonce, 3 /* T_DATA */, session.seq++, session.chunkBuf, session.chunkLen)) {
                    session.lastError = "USB write failure";
                    return;
                }
                session.bytesSent += session.chunkLen;
                session.framesSent++;
                session.chunkLen = 0;

                // Broadcast progress over WebSocket every 64KB (32 frames)
                if ((session.framesSent % 32) == 0 || session.bytesSent == session.expectedSize) {
                    auto cb = m_devices.getHttpPutProgressCallback();
                    if (cb) cb(session.token, session.bytesSent, session.expectedSize);
                }

                // Adaptive pacing: RT4K SD cluster writes take 50-70ms, with occasional
                // SD flash GC/erase taking up to ~117ms. Pausing 120ms at 32KB cluster
                // boundaries lets the SD write complete before the next frame arrives.
                // 65ms normal pacing guarantees RT4K CDC RX FIFO never overflows.
                bool isClusterBoundary = ((session.bytesSent % 32768) == 0);
                delay(isClusterBoundary ? 120 : 65);
            }
        }
    } else if (raw.status == RAW_END) {
        if (!session.lastError.empty()) return;
        if (session.chunkLen > 0) {
            if (!m_devices.sendRtl1Frame(session.nonce, 3 /* T_DATA */, session.seq++, session.chunkBuf, session.chunkLen)) {
                session.lastError = "USB write failure";
                return;
            }
            session.bytesSent += session.chunkLen;
            session.framesSent++;
            session.chunkLen = 0;
            delay(65);
        }

        // Send zero-length EOF frame
        uint8_t eofSeq = session.seq;
        if (!m_devices.sendRtl1Frame(session.nonce, 3 /* T_DATA */, eofSeq, nullptr, 0)) {
            session.lastError = "USB EOF write failure";
            return;
        }
        session.framesSent++;
        delay(50);
        m_devices.flushRT4K();

        auto cb = m_devices.getHttpPutProgressCallback();
        if (cb) cb(session.token, session.bytesSent, session.expectedSize);
    } else if (raw.status == RAW_ABORTED) {
        session.lastError = "Upload aborted by client";
    }
}

void WebServerManager::handlePutResponse() {
    setSecurityHeaders();
    auto& session = m_devices.getHttpPutSessionRef();
    if (!session.active) {
        m_server.send(400, "application/json", "{\"ok\":false,\"error\":\"No active PUT session\"}");
        return;
    }

    if (m_server.hasArg("token")) {
        uint32_t reqToken = strtoul(m_server.arg("token").c_str(), nullptr, 10);
        if (reqToken != session.token) {
            m_server.send(403, "application/json", "{\"ok\":false,\"error\":\"Invalid session token\"}");
            return;
        }
    }

    if (!session.lastError.empty()) {
        char errBuf[128];
        snprintf(errBuf, sizeof(errBuf), "{\"ok\":false,\"bytes\":%u,\"frames\":%u,\"ms\":%u,\"error\":\"%s\"}",
            (unsigned)session.bytesSent, (unsigned)session.framesSent,
            (unsigned)(millis() - session.startTime), session.lastError.c_str());
        m_server.send(500, "application/json", errBuf);
        return;
    }

    char respBuf[128];
    snprintf(respBuf, sizeof(respBuf), "{\"ok\":true,\"bytes\":%u,\"frames\":%u,\"eof\":true,\"eofSeq\":%u,\"ms\":%u,\"localAck\":%s}",
        (unsigned)session.bytesSent, (unsigned)session.framesSent, (unsigned)session.seq,
        (unsigned)(millis() - session.startTime), session.localAck ? "true" : "false");
    m_server.send(200, "application/json", respBuf);
}

void WebServerManager::handleSystemBackup() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    std::string backupJson = m_config.exportAllJson();
    m_server.sendHeader("Content-Disposition", "attachment; filename=donut_remix_backup.json");
    m_server.send(200, "application/json", backupJson.c_str());
}

void WebServerManager::handleSystemRestore() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }
    auto res = m_config.importAllJson(m_server.arg("plain").c_str());
    if (!res.valid) {
        m_server.send(400, "application/json", "{\"error\":\"Import validation failed\"}");
        return;
    }
    m_server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void WebServerManager::handleFirmwareUpdate() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();
    m_server.sendHeader("Connection", "close");
    if (Update.hasError()) {
        m_server.send(500, "text/plain", "Firmware update failed");
    } else {
        m_server.send(200, "text/plain", "Update successful. Rebooting...");
    }
    delay(100);
    ESP.restart();
}

void WebServerManager::handleFirmwareUpload() {
    if (!authenticateRequest()) return;
    HTTPUpload& upload = m_server.upload();
    if (upload.status == UPLOAD_FILE_START) {
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (!Update.end(true)) {
            Update.printError(Serial);
        }
    }
}
#endif

} // namespace DonutRemix
