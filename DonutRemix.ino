/*
 * Donut Remix - Modern, Secure RetroTINK 4K & GameID Automation
 * Forked from DonutShop (GNU GPLv3)
 *
 * Security Hardened:
 *  - Full REST API with Authentication & Session Management
 *  - SSRF & Input Guardrails
 *  - Password-Protected OTA (Port 3232) & Web Firmware Flashing
 *  - Memory Bounds Checking & FreeRTOS Mutex Synchronization
 *  - Decoupled Modular Architecture
 */

#define FIRMWARE_VERSION "1.0.0-remix"
#define USB_MODE 1

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <ESPmDNS.h>

#if USB_MODE
#include <EspUsbHost.h>
EspUsbHost usbHost;
EspUsbHostCdcSerial CdcSerial(usbHost);
#endif

#include "src/Config.h"
#include "src/Security.h"
#include "src/Guardrails.h"
#include "src/Devices.h"
#include "src/GameIDEngine.h"
#include "src/WebServerManager.h"
#include "src/WebCtlBridge.h"

// Global component instances
DonutRemix::ConfigManager configMgr;
DonutRemix::SecurityManager securityMgr;
DonutRemix::DeviceManager deviceMgr;
DonutRemix::GameIDEngine gameIdEngine(configMgr, deviceMgr);
DonutRemix::WebServerManager webServer(configMgr, securityMgr, deviceMgr, gameIdEngine);
DonutRemix::WebCtlBridge wsBridge(deviceMgr, configMgr);

// FreeRTOS GameID background polling task
void gameIdTask(void *pvParameters) {
    (void)pvParameters;
    for (;;) {
        gameIdEngine.pollStep();
        vTaskDelay(pdMS_TO_TICKS(gameIdEngine.getPollIntervalMs()));
    }
}

void setup() {
    // 1. Initialize Hardware & LED status
    deviceMgr.init();

    // 2. Initialize LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println(F("[ERROR] LittleFS mount failed!"));
    }

    // 3. Connect WiFi using WiFiManager
    auto vars = configMgr.getVars();
    WiFi.setHostname(vars.hostname.c_str());
    esp_wifi_set_ps(WIFI_PS_NONE); // Disable power saving to maximize AP broadcast signal

    WiFiManager wm;
    std::vector<const char *> menu = {"wifi"};
    wm.setMenu(menu);
    // Open setup AP (no password needed to join portal, avoiding phone handshake drops)
    // No timeout: stays active indefinitely until user configures home WiFi
    if (!wm.autoConnect("DonutRemix_Setup")) {
        Serial.println(F("[WARN] WiFi failed to connect, continuing in offline mode"));
    } else {
        Serial.print(F("[INFO] WiFi connected: "));
        Serial.println(WiFi.localIP());
    }

    // 4. mDNS
    if (MDNS.begin(vars.hostname.c_str())) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("[INFO] mDNS responder started: http://%s.local\n", vars.hostname.c_str());
    }

    // 5. Secure ArduinoOTA Setup (Fixes VULN-02)
    ArduinoOTA.setHostname(vars.hostname.c_str());
    ArduinoOTA.setPassword(securityMgr.getPasswordHash().substr(0, 16).c_str());
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]\n", error);
    });
    ArduinoOTA.begin();

    // 6. Start Web Server & REST API
    webServer.begin();

    // 7. Start WebCtl WebSocket Bridge on Port 81
    wsBridge.begin();

#if USB_MODE
    // Only switch USB PHY to Host mode if WiFi is connected or actively on OTG
    CdcSerial.begin(2000000);
    usbHost.begin();
#endif

    // 8. Spawn GameID Polling Task
    xTaskCreate(gameIdTask, "GameIDTask", 8192, NULL, 1, NULL);

    deviceMgr.setLed(DonutRemix::LedState::QueryingBlue);
    Serial.println(F("[READY] Donut Remix initialized successfully."));
}

void loop() {
    ArduinoOTA.handle();
    webServer.handleClient();
    wsBridge.loop();
    vTaskDelay(pdMS_TO_TICKS(1)); // yield to FreeRTOS scheduler
}
