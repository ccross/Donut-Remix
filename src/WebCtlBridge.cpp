#include "WebCtlBridge.h"
#include <iostream>

#ifdef ARDUINO
#include <ArduinoJson.h>
#endif

namespace DonutRemix {

WebCtlBridge::WebCtlBridge(DeviceManager& devices, ConfigManager& config)
    : m_devices(devices), m_config(config)
#ifdef ARDUINO
    , m_wsServer(81)
#endif
{}

void WebCtlBridge::begin() {
#ifdef ARDUINO
    m_wsServer.begin();
    m_wsServer.onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
        this->onWebSocketEvent(num, type, payload, length);
    });
    m_devices.setHttpPutProgressCallback([this](uint32_t token, size_t sent, size_t total) {
        JsonDocument prog;
        prog["type"] = "rt4k-bridge";
        prog["op"] = "httpTxProgress";
        prog["token"] = token;
        prog["bytes"] = (uint32_t)sent;
        prog["total"] = (uint32_t)total;
        String s;
        serializeJson(prog, s);
        m_wsServer.broadcastTXT(s.c_str());
    });
#endif
}

void WebCtlBridge::loop() {
#ifdef ARDUINO
    m_wsServer.loop();

    // Drain all available serial bytes from RT4K to connected WebCtl browser clients
    while (true) {
        size_t bytesRead = m_devices.readRT4KBytes(m_rxBuffer, sizeof(m_rxBuffer));
        if (bytesRead > 0) {
            m_wsServer.broadcastBIN(m_rxBuffer, bytesRead);
        } else {
            break;
        }
    }
#endif
}

#ifdef ARDUINO
void WebCtlBridge::onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            break;

        case WStype_CONNECTED:
            break;

        case WStype_TEXT: {
            std::string msg((char*)payload, length);
            if (msg.find("\"type\":\"rt4k-bridge\"") != std::string::npos ||
                msg.find("\"op\":") != std::string::npos) {
                handleControlJson(num, msg);
            } else if (msg == "ping" || msg.find("\"type\":\"ping\"") != std::string::npos) {
                m_wsServer.sendTXT(num, "{\"type\":\"pong\"}");
            } else {
                handleRawTextCommand(num, msg);
            }
            break;
        }

        case WStype_BIN: {
            // Forward raw binary bytes directly to the RT4K
            m_devices.writeRT4KBytes(payload, length);

            // Flow control acknowledgement required by WebCtl writeAndDrain
            char txDone[80];
            snprintf(txDone, sizeof(txDone), "{\"type\":\"rt4k-bridge\",\"op\":\"txDone\",\"ok\":true,\"bytes\":%u}", (unsigned)length);
            m_wsServer.sendTXT(num, txDone);
            break;
        }

        default:
            break;
    }
}

void WebCtlBridge::handleControlJson(uint8_t clientNum, const std::string& jsonStr) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, jsonStr);
    if (err) return;

    const char* op = doc["op"] | "";

    if (strcmp(op, "hello") == 0) {
        bool usb = m_devices.isUsbConnected();
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "hello";
        res["ok"] = true;
        res["baud"] = usb ? 2000000 : m_devices.getRT4KBaud();
        res["preferredHd15Baud"] = 2000000;
        res["hostname"] = "donutshop";
        res["usbConnected"] = usb;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "getBaud") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "getBaud";
        res["ok"] = true;
        res["baud"] = m_devices.getRT4KBaud();
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "setBaud") == 0) {
        uint32_t baud = doc["baud"] | m_devices.getRT4KBaud();
        m_devices.setRT4KBaud(baud);
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "setBaud";
        res["ok"] = true;
        res["baud"] = baud;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "getPreferredHd15Baud") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "getPreferredHd15Baud";
        res["ok"] = true;
        res["baud"] = 2000000;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "setPreferredHd15Baud") == 0) {
        uint32_t baud = doc["baud"] | 2000000;
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "setPreferredHd15Baud";
        res["ok"] = true;
        res["baud"] = baud;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "detectBaud") == 0) {
        // Probe common RT4K baud rates: 2M, 9600, 38400, 115200
        uint32_t candidateBauds[] = {2000000, 9600, 38400, 115200};
        uint32_t foundBaud = 0;
        uint8_t probeBuf[128];

        for (uint32_t b : candidateBauds) {
            m_devices.setRT4KBaud(b);
            while (m_devices.readRT4KBytes(probeBuf, sizeof(probeBuf)) > 0); // flush
            m_devices.writeRT4KBytes((const uint8_t*)"\rver\r\n", 6);
            delay(120);
            size_t n = m_devices.readRT4KBytes(probeBuf, sizeof(probeBuf) - 1);
            if (n > 0) {
                probeBuf[n] = '\0';
                std::string resp((char*)probeBuf);
                if (resp.find("FW") != std::string::npos ||
                    resp.find("Build") != std::string::npos ||
                    resp.find("RetroTink") != std::string::npos ||
                    resp.find("RetroTINK") != std::string::npos) {
                    foundBaud = b;
                    break;
                }
            }
        }

        if (foundBaud == 0) {
            foundBaud = 9600;
            m_devices.setRT4KBaud(foundBaud);
        }

        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "detectBaud";
        res["ok"] = true;
        res["baud"] = foundBaud;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "wake") == 0) {
        m_devices.setRT4KBaud(9600);
        m_devices.wakeRT4K();
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "wake";
        res["ok"] = true;
        res["baud"] = 9600;
        res["usbConnected"] = m_devices.isUsbConnected();
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "rxStreamBegin") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "rxStreamBegin";
        res["ok"] = true;
        res["bulk"] = "websocket";
        res["localAck"] = false;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "rxStreamEnd") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "rxStreamEnd";
        res["ok"] = true;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "putSessionBegin") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "putSessionBegin";
        res["ok"] = true;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "putSessionEnd") == 0) {
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "putSessionEnd";
        res["ok"] = true;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "httpTxStreamBegin") == 0) {
        uint16_t nonce = doc["nonce"] | 0;
        size_t size = doc["size"] | 0;
        bool localAck = doc["localAck"] | false;
        uint16_t payload = doc["payload"] | 2048;
        if (payload == 0 || payload > 2048) payload = 2048;

        uint32_t token = (uint32_t)random(0x10000000, 0x7FFFFFFF);
        m_devices.beginHttpPutSession(token, nonce, size, localAck, payload);

        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "httpTxStreamBegin";
        res["ok"] = true;
        res["bulk"] = "http";
        res["port"] = 80;
        res["token"] = token;
        res["localAck"] = true; // Signal browser to disable upload timeout and listen to httpTxProgress
        res["payload"] = payload;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "httpTxStreamEnd") == 0) {
        auto& session = m_devices.getHttpPutSessionRef();
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "httpTxStreamEnd";
        res["ok"] = true;
        if (!session.lastError.empty()) {
            res["transferError"] = true;
            res["error"] = session.lastError.c_str();
            res["bytes"] = (uint32_t)session.bytesSent;
            res["frames"] = (uint32_t)session.framesSent;
            res["ms"] = (uint32_t)(millis() - session.startTime);
        }
        m_devices.endHttpPutSession();
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    } else if (strcmp(op, "serialLockAck") == 0) {
        uint32_t token = doc["token"] | 0;
        JsonDocument res;
        res["type"] = "rt4k-bridge";
        res["op"] = "serialLockAck";
        res["ok"] = true;
        res["token"] = token;
        String out;
        serializeJson(res, out);
        m_wsServer.sendTXT(clientNum, out.c_str());
    }
}

void WebCtlBridge::handleRawTextCommand(uint8_t clientNum, const std::string& cmd) {
    (void)clientNum;
    if (cmd == "remote pwr") {
        m_devices.sendRawCommand("remote pwr");
        m_wsServer.broadcastTXT("{\"type\":\"webctl-reload\",\"reason\":\"remote pwr\"}");
    } else {
        m_devices.sendRawCommand(cmd);
    }
}
#endif

} // namespace DonutRemix
