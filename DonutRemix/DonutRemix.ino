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

// Global component instances
DonutRemix::ConfigManager configMgr;
DonutRemix::SecurityManager securityMgr;
DonutRemix::DeviceManager deviceMgr;
DonutRemix::GameIDEngine gameIdEngine(configMgr, deviceMgr);
DonutRemix::WebServerManager webServer(configMgr, securityMgr, deviceMgr, gameIdEngine);

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
    WiFiManager wm;
    wm.setConfigPortalTimeout(180); // 3-minute timeout guard
    // Protected fallback access point
    if (!wm.autoConnect("DonutRemix_Setup", "donutremix")) {
        Serial.println(F("[WARN] WiFi failed to connect, continuing in offline mode"));
    } else {
        Serial.print(F("[INFO] WiFi connected: "));
        Serial.println(WiFi.localIP());
    }

    // 4. mDNS
    auto vars = configMgr.getVars();
    if (MDNS.begin(vars.hostname.c_str())) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("[INFO] mDNS responder started: http://%s.local\n", vars.hostname.c_str());
    }

    // 5. Secure ArduinoOTA Setup (Fixes VULN-02)
    ArduinoOTA.setHostname(vars.hostname.c_str());
    // Require password authentication for port 3232 OTA
    ArduinoOTA.setPassword(securityMgr.getPasswordHash().substr(0, 16).c_str());
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]\n", error);
    });
    ArduinoOTA.begin();

    // 6. Start Web Server & REST API
    webServer.begin();

#if USB_MODE
    CdcSerial.begin(2000000);
    if (!usbHost.begin()) {
        Serial.printf("[WARN] usbHost.begin failed: %s\n", usbHost.lastErrorName());
    }
#endif

    // 7. Spawn GameID Polling Task
    xTaskCreate(gameIdTask, "GameIDTask", 8192, NULL, 1, NULL);

    deviceMgr.setLed(DonutRemix::LedState::QueryingBlue);
    Serial.println(F("[READY] Donut Remix initialized successfully."));
}

void loop() {
    ArduinoOTA.handle();
    webServer.handleClient();
    vTaskDelay(pdMS_TO_TICKS(2)); // yield to FreeRTOS scheduler
}
