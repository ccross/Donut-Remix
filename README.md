# 🍩 Donut Remix

**Donut Remix** is an open-source, security-hardened upgrade to [DonutShop](https://github.com/svirant/DonutShop) for the **Arduino Nano ESP32 (ESP32-S3)**. It unifies **RetroTINK 4K** profile automation and **Scalable Video Switch (SVS)** control into a single plug-and-play firmware.

Connect it to your RetroTINK 4K for automatic `gameID` profile switching and full WebCtl remote management, or plug it into your Scalable Video Switch for a modern web dashboard with input switching, transcoder options, and a live serial console.

---

## What's Different?

Upstream DonutShop became popular for retro console automation, but development stalled and shifted toward unauthenticated binary-only releases. Donut Remix restores an open, modular codebase while adding essential security and hardware capabilities:

| Feature | Upstream DonutShop | Donut Remix |
| :--- | :--- | :--- |
| **Hardware Support** | RetroTINK 4K only | **Auto-detects RetroTINK 4K or Scalable Video Switch (SVS)** |
| **Security** | None (open on local network) | **Admin login, session tokens, brute-force lockout** |
| **SVS Matrix Control** | ❌ None | **Web dashboard, transcoder controls, live serial terminal** |
| **WebCtl Interface** | Iframe-based / precompiled | **Native responsive suite with Dark & Glazed (light) themes** |
| **Network Safety** | Unrestricted polling | **SSRF protection, safe URL filters, bus collision locks** |
| **Automation APIs** | Ad-hoc / undocumented | **REST API v1, OpenAPI 3.0, and touch screen (CYD) endpoints** |

> For the in-depth security analysis and vulnerability assessment, see [SECURITY_AUDIT.md](file:///home/ccross/code/donut-remix/SECURITY_AUDIT.md).

---

## Key Features

- 🔀 **Zero-Config Hardware Auto-Detection:** Automatically detects whether an SVS (9,600 baud CH340) or RetroTINK 4K (2,000,000 baud CDC-ACM/FTDI) is plugged in.
- 🎛️ **Scalable Video Switch (SVS) Web Dashboard:** Instant channel switching (0–32), active signal seek, power/standby toggle, attract mode, and real-time serial terminal.
- 🎚️ **SVS Transcoder & Sync Options:** Full web and API control for RGB-to-Component (`R590`/`R591`), Component-to-RGB, RGsB (Sync-on-Green), and Sync Bypass cards.
- 📺 **Authentic RetroTINK 4K WebCtl Suite:** Web remote, live canvas OSD stream, SD card file manager, and GameID auto-switching with 1,600+ community profiles.
- 🎨 **1:1 Visual & Theme Parity:** Identical Dark and Glazed (pastel glassmorphism) themes across both devices with instant NVS persistence.
- 🛡️ **Secure by Default:** Password authentication, session tokens, rate limiting, and network guardrails protecting against SSRF and unauthorized firmware flashes.
- 📱 **Microcontroller & Automation APIs:** Lightweight REST endpoints designed for ESP32 Cheap Yellow Displays (CYD), Home Assistant, and scripting.
- 🔄 **Backward Compatibility:** Legacy endpoints remain supported for existing automations behind security middleware.

---

## Architecture Overview

```
donut-remix/
├── DonutRemix.ino            # Main sketch, USB Host auto-detection & task coordinator
├── SECURITY_AUDIT.md         # Full security vulnerability assessment
├── docs/
│   ├── API.md                # Comprehensive REST API guide with curl examples
│   └── openapi.yaml          # Validated OpenAPI 3.0 specification
├── src/
│   ├── Config.h / .cpp       # Thread-safe LittleFS configuration manager
│   ├── Security.h / .cpp     # SHA-256 auth, session tokens, brute-force rate limiter
│   ├── Guardrails.h / .cpp   # SSRF filter, URL validator, command sanitizer
│   ├── Devices.h / .cpp      # Hardware abstraction (RT4K CDC/FTDI, SVS CH340, Extron, IR)
│   ├── GameIDEngine.h / .cpp # Resilient console polling engine with timeout guards
│   ├── WebServerManager.h/.cpp # HTTP server, REST API v1, WebCtl bridge, SVS handlers
│   ├── SvsUI.h               # Embedded responsive SVS Single Page App dashboard
│   ├── WebCtlAssets.h        # Authentic RetroTINK 4K WebCtl and GameID web apps
│   ├── WebCtlBridge.h / .cpp # WebSocket port 81 bridge for WebCtl canvas/serial
│   └── WebUI.h               # Fallback standalone responsive web interface
├── tests/                    # C++ unit test suite (runs standalone with g++)
│   ├── test_guardrails.cpp
│   ├── test_security.cpp
│   ├── test_config.cpp
│   ├── test_gameid.cpp
│   └── Makefile
└── platformio.ini            # PlatformIO build configuration
```

---

## Hardware Requirements

- **Microcontroller:** Arduino Nano ESP32 (ESP32-S3 / NORA-W106) or WaveShare Nano ESP32.
- **OTG Adapter:** USB-C to USB OTG adapter (e.g. Jadebones or compatible).
- **Supported Hardware (Automatically Detected via USB OTG):**
  - **RetroTINK 4K / 4K Pro:** Connect via USB OTG for WebCtl remote management, live canvas/OSD stream, community profile browser, and automatic GameID switching (2,000,000 baud CDC-ACM or FTDI).
  - **Scalable Video Switch (SVS):** Connect via USB OTG for dedicated responsive matrix switcher dashboard, output transcoder modules (RGB/Component/RGsB/Sync), real-time serial terminal, and REST API (9,600 baud CH340).
- **Optional Video Switches & IR:** Extron Matrix switch (via 9600 baud UART), MT-Viki, TESmart, IR Emitter (pin D11) for RT5X / OSSC / TV power.

---

## Getting Started

### 1. Building with Arduino IDE
1. Open the Arduino IDE (v2.x recommended).
2. Install the **esp32 by Espressif Systems** board package:
   - In Settings, add: `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
3. Install required libraries via Library Manager:
   - `ArduinoJson` (v7.x)
   - `WiFiManager` (by tzapu)
   - `IRremote` (by Armin Joachimsmeyer)
   - `TinyIRReceiver`
   - `EspUsbHost`
4. Under **Tools**, configure:
   - **Board:** `esp32` -> `Arduino Nano ESP32`
   - **USB Mode:** `Debug mode (Hardware CDC)`
   - **Partition Scheme:** `With SPIFFS partition (advanced)` or `Default`
5. Open `DonutRemix.ino` and select **Sketch -> Upload**.

### 2. Building with PlatformIO
Run in your terminal:
```bash
pio run -e nano_esp32 -t upload
```

### 3. Running Unit Tests
All core logic (security, guardrails, config bounds, GameID parsing) includes a standalone C++17 test suite that compiles on Linux/macOS:
```bash
make -C tests test
```

---

## Initial Setup & Login

1. On first boot, connect your smartphone or computer to the temporary WiFi network:  
   **SSID:** `DonutRemix_Setup` | **Password:** `donutremix`
2. Configure your home 2.4 GHz WiFi credentials in the captive portal.
3. Open your browser to:  
   **`http://donutshop.local`** (or your device's LAN IP).
4. Default Administrator Credentials:  
   **Password:** `donutremix`
5. Visit the **Security & System** tab to change your administrator password immediately.

---

## REST API Quick Reference

### Authentication
```bash
curl -X POST http://donutshop.local/api/v1/auth/login \
     -H "Content-Type: application/json" \
     -d '{"password": "yourPassword"}'
# Returns: {"token": "4a2b9f01e7c538209ad43bf1e02947ac", "expiresIn": 86400}
```

### RetroTINK 4K Control
```bash
# Get lightweight microcontroller telemetry (CYD dashboard)
curl http://donutshop.local/api/v1/rt4k/status

# Switch RT4K profile by name or index
curl -X POST http://donutshop.local/api/v1/rt4k/profile \
     -H "Authorization: Bearer <token>" \
     -H "Content-Type: application/json" \
     -d '{"name": "0:/profile/SNES/FirebrandX_1080p.cfg"}'

# Dispatch remote button action
curl -X POST http://donutshop.local/api/v1/rt4k/remote \
     -H "Authorization: Bearer <token>" \
     -H "Content-Type: application/json" \
     -d '{"button": "prof1"}'
```

### Scalable Video Switch (SVS) Control
```bash
# Check SVS telemetry, active channel, and serial logs
curl http://donutshop.local/api/v1/svs/status

# Switch to SVS Input 1
curl -X POST http://donutshop.local/api/v1/svs/input \
     -H "Content-Type: application/json" \
     -d '{"input": 1}'

# Query transcoder output modules (RGB->Comp, Comp->RGB, RGsB, Sync Bypass)
curl http://donutshop.local/api/v1/svs/modules

# Toggle RGB to Component transcoder on Input 1
curl -X POST http://donutshop.local/api/v1/svs/modules \
     -H "Content-Type: application/json" \
     -d '{"input": 1, "action": "toggle_rgb_to_comp"}'
```

### UI Theme Customization
```bash
# Switch theme to Glazed (light) mode and persist to NVS
curl -X POST http://donutshop.local/webctl-theme \
     -d "theme=light"
```

For full documentation, see [docs/API.md](file:///home/ccross/code/donut-remix/docs/API.md) and [docs/openapi.yaml](file:///home/ccross/code/donut-remix/docs/openapi.yaml).

---

## Acknowledgments

- **Gemini**: Architectural design, vulnerability auditing, security guardrails, and test-driven refactoring assistance.
- **RetroTINK LLC (Mike Chi)**: Creator of the RetroTINK 4K, firmware documentation, and serial command protocols.
- **svirant**: Original author of the open-source DonutShop v0.6.3 sketch.

---

## License

Licensed under the [GNU General Public License v3.0](file:///home/ccross/code/donut-remix/LICENSE).
