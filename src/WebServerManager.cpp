#include "WebServerManager.h"
#include "WebUI.h"
#include "WebCtlAssets.h"
#include "SvsUI.h"
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
    m_server.on("/", HTTP_HEAD, [this]() { handleRoot(); });
    m_server.on("/", HTTP_OPTIONS, [this]() { setSecurityHeaders(); m_server.send(204); });
    m_server.on("/svs", HTTP_GET, [this]() { handleSvsPage(); });
    m_server.on("/rt4k", HTTP_GET, [this]() {
        setSecurityHeaders();
        m_server.sendHeader("Content-Encoding", "gzip");
        m_server.send_P(200, "text/html", (PGM_P)WEBCTL_HTML_GZ, WEBCTL_HTML_GZ_LEN);
    });
    m_server.on("/gameid", HTTP_GET, [this]() { handleGameID(); });
    m_server.on("/term", HTTP_GET, [this]() { handleGameID(); });
    m_server.on("/donut-memory", HTTP_GET, [this]() { handleDonutMemory(); });
    m_server.on("/favicon.ico", HTTP_GET, [this]() { m_server.send(204); });
    m_server.on("/rt4k-fw-cleanup", HTTP_POST, [this]() {
        setSecurityHeaders();
        m_server.send(200, "application/json", "{\"ok\":true}");
    });
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

    // CYD (Cheap Yellow Display) & Embedded Dashboard REST API
    m_server.on("/api/v1/rt4k/status", HTTP_GET, [this]() { handleCydStatus(); });
    m_server.on("/api/v1/rt4k/remote", HTTP_POST, [this]() { handleCydRemote(); });
    m_server.on("/api/v1/rt4k/profile", HTTP_POST, [this]() { handleCydProfile(); });
    m_server.on("/api/v1/rt4k/exec", HTTP_POST, [this]() { handleCydExec(); });

    // SVS (Scalable Video Switch) REST API
    m_server.on("/api/v1/svs/status", HTTP_GET, [this]() { handleSvsStatus(); });
    m_server.on("/api/v1/svs/input", HTTP_POST, [this]() { handleSvsInput(); });
    m_server.on("/api/v1/svs/seek", HTTP_POST, [this]() { handleSvsSeek(); });
    m_server.on("/api/v1/svs/power", HTTP_POST, [this]() { handleSvsPower(); });
    m_server.on("/api/v1/svs/command", HTTP_POST, [this]() { handleSvsCommand(); });
    m_server.on("/api/v1/svs/modules", HTTP_GET, [this]() { handleSvsModulesGet(); });
    m_server.on("/api/v1/svs/modules", HTTP_POST, [this]() { handleSvsModulesPost(); });

    // CORS preflight OPTIONS handlers
    auto sendOptions = [this]() {
        setSecurityHeaders();
        m_server.send(204);
    };
    m_server.on("/api/v1/auth/login", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/command", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/rt4k/status", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/rt4k/remote", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/rt4k/profile", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/rt4k/exec", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/status", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/input", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/seek", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/power", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/command", HTTP_OPTIONS, sendOptions);
    m_server.on("/api/v1/svs/modules", HTTP_OPTIONS, sendOptions);

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
    if (m_devices.isSvsConnected() || m_devices.getRT4KBaud() == 9600) {
        m_server.send(200, "text/html", SVS_UI_HTML);
        return;
    }
    m_server.sendHeader("Content-Encoding", "gzip");
    m_server.send_P(200, "text/html", (PGM_P)WEBCTL_HTML_GZ, WEBCTL_HTML_GZ_LEN);
}

void WebServerManager::handleSvsPage() {
    setSecurityHeaders();
    m_server.send(200, "text/html", SVS_UI_HTML);
}

void WebServerManager::handleGameID() {
    setSecurityHeaders();
    m_server.sendHeader("Content-Encoding", "gzip");
    m_server.send_P(200, "text/html", (PGM_P)GAMEID_HTML_GZ, GAMEID_HTML_GZ_LEN);
}

void WebServerManager::handleDonutMemory() {
    setSecurityHeaders();
    JsonDocument doc;
#ifdef ARDUINO
    doc["ok"] = true;
    doc["resetReason"] = (int)esp_reset_reason();
    doc["ddReady"] = true;
    doc["ddStackMinFree"] = 4096;
    doc["ddStackAlloc"] = 8192;
    doc["gidReady"] = true;
    doc["gidStackMinFree"] = 4096;
    doc["gidStackAlloc"] = 8192;
    doc["rxReady"] = true;
    doc["rxStackMinFree"] = 4096;
    doc["rxStackAlloc"] = 8192;
    doc["httpReady"] = true;
    doc["httpStackMinFree"] = 4096;
    doc["httpStackAlloc"] = 8192;
    doc["ackLastReady"] = false;

    doc["internalFree"] = ESP.getFreeHeap();
    doc["internalMin"] = ESP.getMinFreeHeap();
    doc["internalLargest"] = ESP.getMaxAllocHeap();

    doc["psramFree"] = ESP.getFreePsram();
    doc["psramMin"] = ESP.getMinFreePsram();
    doc["psramLargest"] = ESP.getMaxAllocPsram();

    doc["bridgePeak"] = 0;
    doc["bridgeCapacity"] = 16384;
    doc["uptimeMs"] = millis();

    doc["freeHeap"] = ESP.getFreeHeap();
    doc["minFreeHeap"] = ESP.getMinFreeHeap();
    doc["psramSize"] = ESP.getPsramSize();
#else
    doc["ok"] = true;
    doc["resetReason"] = 1;
    doc["ddReady"] = true;
    doc["ddStackMinFree"] = 4096;
    doc["ddStackAlloc"] = 8192;
    doc["gidReady"] = true;
    doc["gidStackMinFree"] = 4096;
    doc["gidStackAlloc"] = 8192;
    doc["rxReady"] = true;
    doc["rxStackMinFree"] = 4096;
    doc["rxStackAlloc"] = 8192;
    doc["httpReady"] = true;
    doc["httpStackMinFree"] = 4096;
    doc["httpStackAlloc"] = 8192;
    doc["ackLastReady"] = false;
    doc["internalFree"] = 240000;
    doc["internalMin"] = 200000;
    doc["internalLargest"] = 180000;
    doc["psramFree"] = 8000000;
    doc["psramMin"] = 7800000;
    doc["psramLargest"] = 7500000;
    doc["bridgePeak"] = 0;
    doc["bridgeCapacity"] = 16384;
    doc["uptimeMs"] = 120000;
    doc["freeHeap"] = 240000;
#endif
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
    if (m_server.method() == HTTP_POST) {
        String body = "";
        if (m_server.hasArg("plain")) {
            body = m_server.arg("plain");
        } else if (m_server.args() > 0) {
            body = m_server.argName(0) + "=" + m_server.arg(0);
        }
        int eq = body.indexOf('=');
        if (eq > 0) {
            String opt = body.substring(0, eq);
            bool val = body.substring(eq + 1).toInt() > 0 || body.substring(eq + 1).equalsIgnoreCase("true");
            if (opt == "struct") m_optStruct = val;
            else if (opt == "console") m_optConsole = val;
            else if (opt == "expert") m_optExpert = val;
            else if (opt == "diag") m_optDiag = val;
            else if (opt == "memory") m_optMemory = val;
            else if (opt == "fwtest") m_optFwTest = val;
        }
    }
    JsonDocument doc;
    doc["ok"] = true;
    doc["struct"] = m_optStruct;
    doc["console"] = m_optConsole;
    doc["expert"] = m_optExpert;
    doc["diag"] = m_optDiag;
    doc["memory"] = m_optMemory;
    doc["fwtest"] = m_optFwTest;
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
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
    if (m_server.method() == HTTP_POST) {
        String body = "";
        if (m_server.hasArg("plain")) {
            body = m_server.arg("plain");
        } else if (m_server.hasArg("theme")) {
            body = m_server.arg("theme");
        } else if (m_server.args() > 0) {
            body = m_server.argName(0) + "=" + m_server.arg(0);
        }
        if (body.indexOf("light") >= 0 || body.indexOf("glazed") >= 0) {
            m_webctlTheme = "light";
        } else if (body.indexOf("dark") >= 0) {
            m_webctlTheme = "dark";
        }
    }
    JsonDocument doc;
    doc["ok"] = true;
    doc["theme"] = m_webctlTheme;
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
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

void WebServerManager::handleCydStatus() {
    setSecurityHeaders();
    JsonDocument doc;
    doc["connected"] = m_devices.isUsbConnected();
    doc["active_profile"] = m_devices.getCurrentProfile();

    std::string king = "None";
    auto consoles = m_config.getConsoles();
    for (const auto& c : consoles) {
        if (c.isOn && c.isKing) {
            king = c.name;
            break;
        }
    }
    doc["active_console"] = king;
    doc["uptime"] = millis() / 1000;
    doc["free_heap"] = ESP.getFreeHeap();
    doc["rt4k_baud"] = m_devices.getRT4KBaud();

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleCydRemote() {
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing request body\"}");
        return;
    }

    std::string button;
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (!err) {
        if (doc["button"].is<const char*>()) {
            button = doc["button"].as<const char*>();
        } else if (doc["key"].is<const char*>()) {
            button = doc["key"].as<const char*>();
        }
    }
    if (button.empty()) {
        button = m_server.arg("plain").c_str();
    }

    while (!button.empty() && (button.front() == ' ' || button.front() == '"')) button.erase(0, 1);
    while (!button.empty() && (button.back() == ' ' || button.back() == '"' || button.back() == '\r' || button.back() == '\n')) button.pop_back();

    if (button.empty()) {
        m_server.send(400, "application/json", "{\"error\":\"Empty button key\"}");
        return;
    }

    if (button.rfind("prof", 0) == 0 && button.length() > 4) {
        int p = atoi(button.c_str() + 4);
        if (p >= 1 && p <= 12) {
            m_devices.sendRemoteButtonProfile(p);
            m_server.send(200, "application/json", "{\"ok\":true,\"button\":\"" + String(button.c_str()) + "\"}");
            return;
        }
    }

    std::string fullCmd = "remote " + button;
    std::string cleanCmd;
    auto res = Guardrails::sanitizeCommand(fullCmd, cleanCmd);
    if (!res.valid) {
        res = Guardrails::sanitizeCommand(button, cleanCmd);
        if (!res.valid) {
            m_server.send(400, "application/json", "{\"error\":\"Invalid remote key\"}");
            return;
        }
    }

    m_devices.sendRawCommand(cleanCmd);
    m_server.send(200, "application/json", "{\"ok\":true,\"command\":\"" + String(cleanCmd.c_str()) + "\"}");
}

void WebServerManager::handleCydProfile() {
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing request body\"}");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (err) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    if (doc["profile"].is<int>()) {
        int prof = doc["profile"].as<int>();
        m_devices.sendProfile(prof);
        m_server.send(200, "application/json", "{\"ok\":true,\"profile\":" + String(prof) + "}");
        return;
    }

    if (doc["svs"].is<int>()) {
        int svs = doc["svs"].as<int>();
        m_devices.sendSvsInput(svs);
        m_server.send(200, "application/json", "{\"ok\":true,\"svs\":" + String(svs) + "}");
        return;
    }

    if (doc["button"].is<int>()) {
        int btn = doc["button"].as<int>();
        m_devices.sendRemoteButtonProfile(btn);
        m_server.send(200, "application/json", "{\"ok\":true,\"button\":" + String(btn) + "}");
        return;
    }

    if (doc["name"].is<const char*>()) {
        std::string name = doc["name"].as<const char*>();
        std::string cmd = "load " + name;
        std::string clean;
        auto res = Guardrails::sanitizeCommand(cmd, clean);
        if (!res.valid) {
            m_server.send(400, "application/json", "{\"error\":\"Unsafe profile path\"}");
            return;
        }
        m_devices.sendRawCommand(clean);
        m_server.send(200, "application/json", "{\"ok\":true,\"loaded\":\"" + String(name.c_str()) + "\"}");
        return;
    }

    m_server.send(400, "application/json", "{\"error\":\"Missing profile, svs, button, or name field\"}");
}

void WebServerManager::handleCydExec() {
    if (!authenticateRequest()) return;
    setSecurityHeaders();

    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing request body\"}");
        return;
    }

    std::string cmd;
    uint32_t timeoutMs = 250;
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
    if (!err && doc["cmd"].is<const char*>()) {
        cmd = doc["cmd"].as<const char*>();
        if (doc["timeout"].is<uint32_t>()) {
            timeoutMs = doc["timeout"].as<uint32_t>();
            if (timeoutMs > 2000) timeoutMs = 2000;
        }
    } else {
        cmd = m_server.arg("plain").c_str();
    }

    std::string clean;
    auto res = Guardrails::sanitizeCommand(cmd, clean);
    if (!res.valid) {
        m_server.send(400, "application/json", "{\"error\":\"Command rejected by guardrail\"}");
        return;
    }

    std::string output = m_devices.executeCommandSync(clean, timeoutMs);
    JsonDocument resp;
    resp["ok"] = true;
    resp["command"] = clean;
    resp["output"] = output;
    String out;
    serializeJson(resp, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleSvsStatus() {
    setSecurityHeaders();
    JsonDocument doc;
    doc["connected"] = m_devices.isSvsConnected();
    doc["firmware"] = m_devices.getSvsFirmware();
    doc["current_input"] = m_devices.getSvsCurrentInput();
    doc["total_inputs"] = m_devices.getSvsTotalInputs();
    doc["power"] = m_devices.getSvsPower();
    doc["attract"] = m_devices.getSvsAttract();
    doc["manual"] = m_devices.getSvsManualMode();
    doc["uptime"] = (millis() / 1000);
#if defined(USB_MODE) && USB_MODE
    doc["cdc_connected"] = CdcSerial.connected();
    doc["usb_host_ready"] = usbHost.ready();
    doc["svs_connected"] = m_devices.isSvsConnected();
    doc["rt4k_baud"] = m_devices.getRT4KBaud();
    EspUsbHostDeviceInfo devs[4];
    size_t count = usbHost.getDevices(devs, 4);
    doc["usb_device_count"] = count;
    JsonArray arr = doc["usb_devices"].to<JsonArray>();
    for (size_t i = 0; i < count; i++) {
        JsonObject d = arr.add<JsonObject>();
        d["vid"] = devs[i].vid;
        d["pid"] = devs[i].pid;
        d["addr"] = devs[i].address;
        if (devs[i].manufacturer) d["mfg"] = devs[i].manufacturer;
        if (devs[i].product) d["prod"] = devs[i].product;
    }
    doc["last_cmd"] = m_devices.getLastSvsCmd().c_str();
    doc["last_bytes_written"] = m_devices.getLastSvsBytesWritten();
    doc["last_rx"] = m_devices.getLastSvsRx().c_str();
#endif
    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleSvsInput() {
    setSecurityHeaders();
    if (!m_server.hasArg("plain") && !m_server.hasArg("input")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing input specification\"}");
        return;
    }
    int inVal = 0;
    if (m_server.hasArg("input")) {
        inVal = m_server.arg("input").toInt();
    } else {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
        if (!err && doc["input"].is<int>()) {
            inVal = doc["input"].as<int>();
        } else {
            inVal = m_server.arg("plain").toInt();
        }
    }
    if (inVal < 0 || inVal > 32) {
        m_server.send(400, "application/json", "{\"error\":\"Input out of range (0-32)\"}");
        return;
    }
    m_devices.switchSvsInput((uint8_t)inVal);
    m_server.send(200, "application/json", "{\"ok\":true,\"input\":" + String(inVal) + "}");
}

void WebServerManager::handleSvsSeek() {
    setSecurityHeaders();
    bool up = true;
    if (m_server.hasArg("plain")) {
        String p = m_server.arg("plain");
        if (p.indexOf("down") >= 0 || p.indexOf("prev") >= 0) up = false;
    }
    if (m_server.hasArg("dir")) {
        if (m_server.arg("dir").equalsIgnoreCase("down")) up = false;
    }
    m_devices.seekSvsInput(up);
    m_server.send(200, "application/json", "{\"ok\":true,\"direction\":\"" + String(up ? "up" : "down") + "\"}");
}

void WebServerManager::handleSvsPower() {
    setSecurityHeaders();
    bool on = true;
    if (m_server.hasArg("plain")) {
        String p = m_server.arg("plain");
        if (p.indexOf("false") >= 0 || p.indexOf("off") >= 0 || p.indexOf("0") >= 0) on = false;
    }
    if (m_server.hasArg("power")) {
        if (m_server.arg("power").equalsIgnoreCase("false") || m_server.arg("power").equalsIgnoreCase("off")) on = false;
    }
    m_devices.setSvsPower(on);
    m_server.send(200, "application/json", "{\"ok\":true,\"power\":" + String(on ? "true" : "false") + "}");
}

void WebServerManager::handleSvsCommand() {
    setSecurityHeaders();
    if (!m_server.hasArg("plain") && !m_server.hasArg("cmd")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing command\"}");
        return;
    }
    std::string cmd;
    if (m_server.hasArg("cmd")) {
        cmd = m_server.arg("cmd").c_str();
    } else {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, m_server.arg("plain"));
        if (!err && doc["cmd"].is<const char*>()) {
            cmd = doc["cmd"].as<const char*>();
        } else if (!err && doc["command"].is<const char*>()) {
            cmd = doc["command"].as<const char*>();
        } else {
            cmd = m_server.arg("plain").c_str();
        }
    }
    std::string clean;
    auto res = Guardrails::sanitizeCommand(cmd, clean);
    if (!res.valid) {
        m_server.send(400, "application/json", "{\"error\":\"Command rejected by guardrail\"}");
        return;
    }
    std::string rawResponse = m_devices.executeSvsCommandSync(clean, 500);
    JsonDocument resp;
    resp["ok"] = true;
    resp["command"] = clean.c_str();
    resp["response"] = rawResponse.c_str();
    resp["bytesWritten"] = m_devices.getLastSvsBytesWritten();
    String out;
    serializeJson(resp, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleSvsModulesGet() {
    setSecurityHeaders();
    bool refresh = m_server.hasArg("refresh");
    m_devices.querySvsModules(refresh);
    const auto& mods = m_devices.getSvsModules();

    JsonDocument doc;
    doc["ok"] = true;
    uint8_t total = m_devices.getSvsTotalInputs() > 0 ? m_devices.getSvsTotalInputs() : 6;
    doc["total_inputs"] = total;
    doc["current_input"] = m_devices.getSvsCurrentInput();

    JsonArray arrY = doc["rgb_to_comp"].to<JsonArray>();
    JsonArray arrG = doc["comp_to_rgb"].to<JsonArray>();
    JsonArray arrR = doc["rgsb"].to<JsonArray>();
    JsonArray arrS = doc["sync_bypass"].to<JsonArray>();

    for (uint8_t i = 0; i < total; i++) {
        arrY.add(mods.rgbToComp[i]);
        arrG.add(mods.compToRgb[i]);
        arrR.add(mods.rgsb[i]);
        arrS.add(mods.syncBypass[i]);
    }

    String out;
    serializeJson(doc, out);
    m_server.send(200, "application/json", out);
}

void WebServerManager::handleSvsModulesPost() {
    setSecurityHeaders();
    if (!m_server.hasArg("plain")) {
        m_server.send(400, "application/json", "{\"error\":\"Missing body\"}");
        return;
    }

    JsonDocument req;
    DeserializationError err = deserializeJson(req, m_server.arg("plain"));
    if (err) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
        return;
    }

    uint8_t cur = m_devices.getSvsCurrentInput();
    uint8_t input = req["input"].is<int>() ? (uint8_t)req["input"].as<int>() : cur;
    if (input < 1 || input > 32) {
        m_server.send(400, "application/json", "{\"error\":\"Invalid input index\"}");
        return;
    }

    if (req["type"].is<const char*>()) {
        std::string type = req["type"].as<std::string>();
        bool enable = req["enable"].as<bool>();
        if (type == "rgb_to_comp") {
            m_devices.setSvsRgbToComp(input, enable);
        } else if (type == "comp_to_rgb") {
            m_devices.setSvsCompToRgb(input, enable);
        } else if (type == "rgsb") {
            m_devices.setSvsRgsb(input, enable);
        } else if (type == "sync_bypass") {
            m_devices.setSvsSyncBypass(input, enable);
        } else {
            m_server.send(400, "application/json", "{\"error\":\"Unknown type\"}");
            return;
        }
    } else if (req["action"].is<const char*>()) {
        std::string action = req["action"].as<std::string>();
        const auto& mods = m_devices.getSvsModules();
        if (action == "toggle_rgb_to_comp") {
            bool curr = (input >= 1 && input <= 32) ? mods.rgbToComp[input - 1] : false;
            m_devices.setSvsRgbToComp(input, !curr);
        } else if (action == "toggle_comp_to_rgb") {
            bool curr = (input >= 1 && input <= 32) ? mods.compToRgb[input - 1] : false;
            m_devices.setSvsCompToRgb(input, !curr);
        } else if (action == "toggle_rgsb") {
            bool curr = (input >= 1 && input <= 32) ? mods.rgsb[input - 1] : false;
            m_devices.setSvsRgsb(input, !curr);
        } else if (action == "toggle_sync_bypass") {
            bool curr = (input >= 1 && input <= 32) ? mods.syncBypass[input - 1] : true;
            m_devices.setSvsSyncBypass(input, !curr);
        }
    }

    handleSvsModulesGet();
}
#endif

} // namespace DonutRemix
