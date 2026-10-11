#pragma once

#include <memory>
#include <string>
#include "Config.h"
#include "Security.h"
#include "Devices.h"
#include "GameIDEngine.h"

#ifdef ARDUINO
#include <WebServer.h>
#endif

namespace DonutRemix {

class WebServerManager {
public:
    WebServerManager(ConfigManager& config, SecurityManager& security, DeviceManager& devices, GameIDEngine& gameId);

    void begin();
    void handleClient();

private:
    ConfigManager& m_config;
    SecurityManager& m_security;
    DeviceManager& m_devices;
    GameIDEngine& m_gameId;

#ifdef ARDUINO
    WebServer m_server;

    // WebCtl State
    std::string m_webctlTheme{"dark"};
    bool m_optStruct{true};
    bool m_optConsole{true};
    bool m_optExpert{true};
    bool m_optDiag{true};
    bool m_optMemory{true};
    bool m_optFwTest{false};

    // Middleware & Security
    void setSecurityHeaders();
    bool authenticateRequest();
    std::string getClientIp();

    // Route Handlers
    void handleRoot();
    void handleAuthLogin();
    void handleAuthLogout();
    void handleAuthStatus();
    void handleGetStatus();
    void handleGetConsoles();
    void handlePostConsoles();
    void handleGetGameDB();
    void handlePostGameDB();
    void handleGetVars();
    void handlePostVars();
    void handlePostCommand();
    void handleSystemBackup();
    void handleSystemRestore();
    void handleFirmwareUpdate();
    void handleFirmwareUpload();

    // Legacy & WebCtl Compatibility Handlers
    void handleLegacyCmd();
    void handleWebCtlInfo();
    void handleWebCtlStatus();
    void handleWebCtlCommand();
    void handleWebCtlFiles();
    void handleWebCtlMkdir();
    void handleWebCtlMove();
    void handleWebCtlOptions();
    void handleWebCtlTheme();
    void handlePutResponse();
    void handlePutUpload();
    void handleGameID();
    void handleDonutMemory();

    // CYD (Cheap Yellow Display) & Embedded Dashboard Handlers
    void handleCydStatus();
    void handleCydRemote();
    void handleCydProfile();
    void handleCydExec();

    // SVS (Scalable Video Switch) Handlers
    void handleSvsPage();
    void handleSvsStatus();
    void handleSvsInput();
    void handleSvsSeek();
    void handleSvsPower();
    void handleSvsCommand();
    void handleSvsModulesGet();
    void handleSvsModulesPost();
#endif
};

} // namespace DonutRemix
