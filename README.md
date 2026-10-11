# 🍩 Donut Remix

**Donut Remix** is a modern, security-hardened, contract-first fork of [DonutShop](https://github.com/svirant/DonutShop) for the **Arduino Nano ESP32 (ESP32-S3)**, the **RetroTINK 4K**, and the **Scalable Video Switch (SVS)**. It automates RetroTINK 4K profiles based on `gameID` queries across network-connected retro consoles (such as PS1Digital, N64Digital, RetroGEM, and MemCardPro 2), hosts the authentic RetroTINK 4K WebCtl remote management suite, and provides a full-featured web dashboard and REST API controller for the Scalable Video Switch.

> [!NOTE]
> **AI-Assisted Project:** Donut Remix is a Gemini-assisted project, engineered using contract-first modular architecture, automated security threat modeling, and test-driven verification.

---

## Why Donut Remix?

The upstream DonutShop project grew popular in late 2026, but subsequent releases shifted towards distributing precompiled closed binary releases (up to `v0.7.27`) without updating the public GNU GPLv3 source repository (which remained stuck at `v0.6.3`).

A security and architecture audit of both the published source code and the `v0.7.27` release binary revealed critical network security risks to home networks:
- **Zero Authentication:** The web interface, configuration endpoints, and raw hardware command channels were open to anyone on the LAN.
- **Trivial Remote Code Execution (RCE):** The `/update` web endpoint and `ArduinoOTA` (port 3232) accepted arbitrary firmware flashes without passwords, tokens, or CSRF protection.
- **Server-Side Request Forgery (SSRF):** Unauthenticated console addresses were continuously polled every 2 seconds without URL validation, enabling attackers to probe internal routers and IoT devices.
- **Memory Corruption Flaws:** Unchecked loops over incoming JSON arrays caused buffer overflows on fixed RAM allocations.
- **Thread Concurrency Issues:** Unsynchronized FreeRTOS tasks concurrently mutated shared globals without mutex locks.

For full technical details, see the complete [SECURITY_AUDIT.md](file:///home/ccross/code/donut-remix/SECURITY_AUDIT.md).

---

## Key Features in Donut Remix

- 🛡️ **Secure by Default:**
  - Mandatory admin login and session token management (`Authorization: Bearer <token>`).
  - Salted SHA-256 password storage with timing-safe comparisons.
  - Brute-force rate limiting (locks out aggressive attackers after 5 failed attempts).
  - Port 3232 `ArduinoOTA` protected by cryptographic password authentication.
- 🔀 **Scalable Video Switch (SVS) Bridge & Matrix Controller:**
  - **Zero-Configuration USB Auto-Detection:** Automatically detects whether an SVS (CH340 @ 9,600 baud) or RT4K (CDC-ACM / FTDI @ 2,000,000 baud) is plugged into USB OTG.
  - **Dedicated SVS Web Dashboard:** Clean, responsive Single Page Application with instant channel switching (0–32), active video seek, power toggle, attract mode, and manual mode.
  - **Output Module Transcoders & Sync Processing:** Full control over RGB-to-Component (`R590`/`R591`), Component-to-RGB, RGsB (Sync-on-Green), and Sync Bypass cards via interactive per-input modal and REST API.
  - **Real-Time Serial Console:** Built-in terminal log streaming from a thread-safe circular buffer with quick command chips.
- 🎨 **100% Theme & Visual Parity:**
  - Unified **Donut Remix** branding across both RT4K WebCtl and SVS dashboards.
  - Authentic WebCtl design tokens: Dark and Glazed modes with frosted translucent cards (`backdrop-filter: blur(16px)`), cyan card headers, and multi-radial pastel backdrops.
  - Click-to-open theme switcher popover preventing premature switching, with dual `localStorage` and NVS persistence (`POST /webctl-theme`).
- 🚦 **Network & Input Guardrails:**
  - SSRF protection: Rejects loopback (`127.0.0.1`, `localhost`), cloud metadata (`169.254.169.254`), multicast, and dangerous ports.
  - Hostname validation: Strictly enforces lowercase `a-z`, `0-9`, and `-` (no edge hyphens, max 32 chars) preventing mDNS/DNS crashes (upstream v0.7.14 regression).
  - Path traversal prevention: Rejects `..` and illegal filesystem characters in file manager operations (`/api/files/*`).
  - Command sanitization: Strips null bytes and control escapes before serial/IR dispatch.
  - Safe bounds checking on all collections (`MAX_CONSOLES` = 16, `MAX_GAMEDB` = 2,000).
- 🔒 **Serial Bus Lock & Concurrency Guardrail:**
  - Automatically suspends background GameID polling whenever firmware updates or file transfers occur, eliminating serial packet collisions and FreeRTOS DMA crashes (upstream v0.7.5 / v0.7.9 regression).
- 📡 **Well-Documented REST API & Full WebCtl Compatibility:**
  - Modern, versioned REST endpoints (`/api/v1/status`, `/api/v1/consoles`, `/api/v1/gamedb`, `/api/v1/command`).
  - Microcontroller touch endpoints (`/api/v1/rt4k/status`, `/api/v1/rt4k/profile`, `/api/v1/rt4k/remote`, `/api/v1/rt4k/exec`) for Cheap Yellow Displays (CYD).
  - Complete SVS endpoints (`/api/v1/svs/status`, `/api/v1/svs/input`, `/api/v1/svs/seek`, `/api/v1/svs/power`, `/api/v1/svs/modules`, `/api/v1/svs/command`).
  - Drop-in support for the REST API introduced in upstream v0.7.25 (`/api/info`, `/api/status`, `/api/command`, `/api/files/*`, `/api/files/mkdir`, `/api/files/move`), secured with authentication.
  - Complete [OpenAPI 3.0 Specification](file:///home/ccross/code/donut-remix/docs/openapi.yaml).
  - Comprehensive human-readable [API Documentation](file:///home/ccross/code/donut-remix/docs/API.md).
- 🧩 **Modular Architecture & Memory Safety:**
  - Deconstructed the fragile 6,000-line monolithic `.ino` into clean, decoupled C++ modules with FreeRTOS mutex synchronization.
  - Fixed-capacity data structures and streaming serialization prevent ESP32-S3 SRAM heap exhaustion.
- 🔄 **Backward Compatibility:**
  - Legacy endpoints (`/cmd`, `/getConsoles`, `/getGameDB`, `/getVars`, `/term`) remain functional for existing automations while being protected by authentication.

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
