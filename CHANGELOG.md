# Changelog

All notable changes to **Donut Remix** are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-10-11

### Added
- **Dual-Hardware Auto-Detection (RT4K & SVS):**
  - Single unified codebase automatically identifies connected USB peripherals on hotplug:
    - **RetroTINK 4K / 4K Pro:** CDC-ACM (`0x0525`) or FTDI (`0x0403`) running at 2,000,000 baud.
    - **Scalable Video Switch (SVS):** CH340 (`0x1a86`) running at 9,600 baud.
- **Scalable Video Switch (SVS) Bridge Module:**
  - Full hardware control methods in `DeviceManager`: input switching (0–32), signal seek (up/down), power/standby toggle, attract mode, and manual mode.
  - SVS hardware banner parser extracting live firmware version, total inputs, and active channel.
  - Dedicated REST API endpoints:
    - `GET /api/v1/svs/status`: Hardware status, active channel, power, USB details, and circular buffer serial logs.
    - `POST /api/v1/svs/input`: Instant 1-click channel switching.
    - `POST /api/v1/svs/seek`: Active signal seek up/down.
    - `POST /api/v1/svs/power`: Hardware power and standby toggle.
    - `POST /api/v1/svs/command`: Guardrail-sanitized raw SVS serial commands with synchronous response.
- **SVS Output Module Transcoders & Sync Settings:**
  - Hardware abstraction and control for SVS modular expansion cards:
    - RGB to Component transcoders (`R590` / `R591`).
    - Component to RGB transcoders.
    - RGsB (Sync-on-Green) processing.
    - Sync Bypass modes.
  - REST endpoints `GET /api/v1/svs/modules` and `POST /api/v1/svs/modules` supporting explicit channel configuration and toggling.
  - Interactive channel settings modal and feature badges on the SVS web interface.
- **Dedicated SVS Web Dashboard with 100% Theme Parity:**
  - Responsive Single Page Application dashboard embedded in `src/SvsUI.h`.
  - 1:1 visual alignment with RetroTINK 4K WebCtl: identical 56px topbar, `Donut·Remix` typography, neon pink dot, status badge, and info chips.
  - Exact WebCtl CSS color token architecture (`--bg: #0b0e14`, `--card: #141926`, `--cyan`, `--pink`, `--amber`, `--grn`, `--line`).
  - **Glazed Light Theme:** Exact multi-point pastel radial gradient background, frosted translucent cards (`backdrop-filter: blur(16px)`), cyan card headers, and rainbow capsule indicators.
  - **Click-to-Open Theme Switcher:** Menu opens showing Dark and Glazed choices without premature switching.
  - **Dual Persistence:** Zero-flash first-paint restoration via browser `localStorage` and NVS persistence (`POST /webctl-theme`).
- **Live Serial Terminal Console:**
  - SVS dashboard `#conlog` card powered by a 30-message thread-safe FreeRTOS circular buffer in `DeviceManager`.
  - Quick action chips for instant testing (`Input 1–6`, `Attract On/Off`, `Status`, `Transcoder`).
- **Microcontroller REST API (CYD Dashboard):**
  - Ultra-lightweight JSON endpoints tailored for ESP32 Cheap Yellow Displays and external automation:
    - `GET /api/v1/rt4k/status`: Minimal status packet (connected, active profile, king console, heap, uptime).
    - `POST /api/v1/rt4k/profile`: Instant profile switching by profile index, SVS channel, remote button, or SD card path.
    - `POST /api/v1/rt4k/remote`: Remote button dispatcher (`menu`, `ok`, `back`, `dpad`, `prof1`–`prof12`).
    - `POST /api/v1/rt4k/exec`: Synchronous CDC command execution returning response text in JSON.
- **Native WebCtl GameID Redesign:**
  - Completely removed legacy `<iframe id="gameid-frame">` in favor of native responsive WebCtl cards.
  - Community Profile Browser indexing all 1,690 RT4K profiles with system category chips (SNES, Genesis, NES, PS1, PS2, N64, Saturn, Dreamcast, Arcade, SVS).
  - Searchable GameID mapping catalog with inline editing.
- **Security & Threat Mitigation (Addressing Upstream Vulnerabilities):**
  - Session-token authentication (`POST /api/v1/auth/login`, `POST /api/v1/auth/logout`) with SHA-256 salted hashes.
  - Brute-force rate limiting: 5 failed attempts locks out the client IP for 60 seconds.
  - Password-protected `ArduinoOTA` on port 3232.
  - SSRF guardrail rejecting loopback, cloud metadata (169.254.169.254), and dangerous ports.
  - RFC-compliant hostname validation preventing mDNS crashes.
  - Path traversal guardrail rejecting `..` and illegal characters on file manager endpoints.
  - Serial Bus Lock guardrail suspending GameID polling during firmware flashing and file transfers to prevent DMA collisions.
- **Comprehensive Verification Suite:**
  - Standalone C++17 unit test suite (`test_guardrails`, `test_security`, `test_config`, `test_gameid`) runnable via `make -C tests test`.
  - OpenAPI 3.0 specification (`docs/openapi.yaml`) and full markdown API guide (`docs/API.md`).

### Fixed
- **CH340 USB Host Reset Lock:** Deasserted DTR and RTS signals during USB Host enumeration to prevent holding the SVS ATmega328P microcontroller in reset.
- **SVS Command Line Terminator:** Transmit delimiter aligned to `\r\n\r\n` matching hardware command parser requirements.
- **SVS Serial Stream Filtering:** RX stream sanitized to printable ASCII characters to prevent JSON parsing errors.
- **Root URL Routing:** Fixed web server root handler (`handleRoot()`) to correctly detect active device mode and prevent serving SVS UI to RT4K nodes during USB enumeration.
- **Memory Safety:** Eliminated upstream unbounded JSON parsing loops and unchecked buffer allocations across console and GameDB management.

---

## [0.7.27] - Upstream Baseline
- Last closed-source binary release distributed by upstream DonutShop.
- Source code in public repository remained at v0.6.3 without GPL compliance.
- Unauthenticated web interface, open RCE `/update` endpoint, SSRF vulnerabilities, and FreeRTOS task concurrency defects.
