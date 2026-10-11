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
#ifndef ESP_USB_HOST_CDC_RX_BUFFER_SIZE
#define ESP_USB_HOST_CDC_RX_BUFFER_SIZE 16384
#endif
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

#if USB_MODE
    CdcSerial.setRxBufferSize(16384);
    usbHost.onDeviceConnected([](const EspUsbHostDeviceInfo& info) {
        if (info.vid == 0x1a86) {
            // Scalable Video Switch (CH340)
            CdcSerial.begin(9600);
            CdcSerial.setDtr(false);
            CdcSerial.setRts(false);
            deviceMgr.setRT4KBaud(9600);
            deviceMgr.setSvsConnected(true);
            Serial.println(F("[USB Host] Detected SVS Switch (CH340 9600 baud)"));

            auto v = configMgr.getVars();
            if (v.hostname == "donutshop" || v.hostname == "donutremix" || v.hostname.empty() || v.hostname == "donutremix-rt4k") {
                MDNS.end();
                if (MDNS.begin("donutremix-svs")) {
                    MDNS.addService("http", "tcp", 80);
                    Serial.println(F("[INFO] mDNS updated to http://donutremix-svs.local"));
                }
            }
        } else if (info.vid == 0x1209 || info.vid == 0x0483 || info.vid == 0x0403) {
            // RetroTINK 4K Pro (CDC-ACM or FTDI)
            CdcSerial.begin(2000000);
            deviceMgr.setRT4KBaud(2000000);
            deviceMgr.setSvsConnected(false);
            Serial.println(F("[USB Host] Detected RT4K (2M baud)"));

            auto v = configMgr.getVars();
            if (v.hostname == "donutshop" || v.hostname == "donutremix" || v.hostname.empty() || v.hostname == "donutremix-svs") {
                MDNS.end();
                if (MDNS.begin("donutremix-rt4k")) {
                    MDNS.addService("http", "tcp", 80);
                    Serial.println(F("[INFO] mDNS updated to http://donutremix-rt4k.local"));
                }
            }
        }
    });
    usbHost.onDeviceDisconnected([](const EspUsbHostDeviceInfo& info) {
        if (info.vid == 0x1a86) {
            deviceMgr.setSvsConnected(false);
            Serial.println(F("[USB Host] SVS Switch Disconnected"));
        }
    });
    CdcSerial.begin(9600);
    CdcSerial.setDtr(false);
    CdcSerial.setRts(false);
    usbHost.begin();
    delay(500); // Allow USB enumeration to detect connected device before WiFi / mDNS setup
#endif

    // 2. Initialize LittleFS
    if (!LittleFS.begin(true)) {
        Serial.println(F("[ERROR] LittleFS mount failed!"));
    }

    // 3. Connect WiFi using WiFiManager
    auto vars = configMgr.getVars();
    std::string activeHostname = vars.hostname;

    // Device-aware default hostname to prevent mDNS collisions on multi-device networks
    if (activeHostname == "donutshop" || activeHostname == "donutremix" || activeHostname.empty()) {
#if USB_MODE
        if (deviceMgr.isSvsConnected()) {
            activeHostname = "donutremix-svs";
        } else {
            activeHostname = "donutremix-rt4k";
        }
#else
        activeHostname = "donutremix";
#endif
    }

    WiFi.setHostname(activeHostname.c_str());
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
    if (MDNS.begin(activeHostname.c_str())) {
        MDNS.addService("http", "tcp", 80);
        Serial.printf("[INFO] mDNS responder started: http://%s.local\n", activeHostname.c_str());
    }

    // 5. Secure ArduinoOTA Setup (Fixes VULN-02)
    ArduinoOTA.setHostname(activeHostname.c_str());
    ArduinoOTA.setPassword(securityMgr.getPasswordHash().substr(0, 16).c_str());
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("[OTA] Error[%u]\n", error);
    });
    ArduinoOTA.begin();

    // 6. Start Web Server & REST API
    webServer.begin();

    // 7. Start WebCtl WebSocket Bridge on Port 81
    wsBridge.begin();

    // 8. Spawn GameID Polling Task
    xTaskCreate(gameIdTask, "GameIDTask", 8192, NULL, 1, NULL);

    deviceMgr.setLed(DonutRemix::LedState::QueryingBlue);
    Serial.println(F("[READY] Donut Remix initialized successfully."));
}

void loop() {
    ArduinoOTA.handle();
    webServer.handleClient();

#if USB_MODE
    static uint32_t lastUsbCheck = 0;
    if (millis() - lastUsbCheck > 1000) {
        lastUsbCheck = millis();
        EspUsbHostDeviceInfo devs[2];
        size_t count = usbHost.getDevices(devs, 2);
        for (size_t i = 0; i < count; i++) {
            if (devs[i].vid == 0x1a86) {
                if (!deviceMgr.isSvsConnected() || deviceMgr.getRT4KBaud() != 9600) {
                    deviceMgr.setSvsConnected(true);
                    deviceMgr.setRT4KBaud(9600);
                    CdcSerial.setDtr(false);
                    CdcSerial.setRts(false);
                    Serial.println(F("[USB Host] SVS Switch active (9600 baud)"));
                }
                break;
            } else if (devs[i].vid == 0x1209 || devs[i].vid == 0x0483 || devs[i].vid == 0x0403) {
                if (deviceMgr.isSvsConnected() || deviceMgr.getRT4KBaud() != 2000000) {
                    deviceMgr.setSvsConnected(false);
                    deviceMgr.setRT4KBaud(2000000);
                    CdcSerial.begin(2000000);
                    Serial.println(F("[USB Host] RT4K active (2M baud)"));
                }
                break;
            }
        }
    }

    if (deviceMgr.isSvsConnected()) {
        static String svsLine = "";
        while (CdcSerial.available()) {
            char c = (char)CdcSerial.read();
            deviceMgr.appendSvsRx(c);
            if (c == '\n' || c == '\r') {
                if (svsLine.length() > 0) {
                    deviceMgr.parseSvsLine(svsLine.c_str());
                    svsLine = "";
                }
            } else if (c >= 32 && c <= 126 && svsLine.length() < 128) {
                svsLine += c;
            }
        }
    } else {
        wsBridge.loop();
    }
#else
    wsBridge.loop();
#endif

    vTaskDelay(pdMS_TO_TICKS(1)); // yield to FreeRTOS scheduler
}
