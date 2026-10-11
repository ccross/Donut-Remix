# 🗺️ Donut Remix - Roadmap & Architecture Status

This document tracks completed features, architectural decisions, and module designs.

---

## ✅ Completed: Modern Native Web UI Redesign
- [x] **Streamlined Native Interface (Iframe Eliminated):**
  - Removed legacy `<iframe id="gameid-frame">` completely; replaced with native WebCtl responsive card components.
  - Native integration of GameID console automation, polling status, and inline configuration.
  - Native GameID database catalog with instant live search filtering and inline editing.
  - Visual Community Profile Browser indexing the 1,690 installed RT4K SD card profiles with system category chips (SNES, Genesis, NES, PS1, PS2, N64, Saturn, Dreamcast, PCE, Arcade, SVS), text search, and 1-click loading.
  - Unified theming matching WebCtl's Dark, Light, and Glazed colorways with 0 cross-frame messaging overhead.
  - Validated with automated headless browser tests: 0 uncaught exceptions across all tab transitions.

---

## ✅ Completed: Microcontroller-Friendly REST API (CYD Dashboard)
- [x] **Touch Dashboard Endpoints:**
  - `GET /api/v1/rt4k/status`: Compact JSON reporting connection state, active profile, king console, uptime, free heap, and RT4K baud.
  - `POST /api/v1/rt4k/remote`: Simplified touch-button dispatcher for menu navigation, dpad, power, and remote button profiles (`prof1`–`prof12`).
  - `POST /api/v1/rt4k/profile`: Instant profile switching by profile index (`profile`), SVS number (`svs`), remote button (`button`), or SD card file path (`name`).
  - `POST /api/v1/rt4k/exec`: Synchronous command execution over USB CDC returning live response output lines directly in JSON.
  - Live tested and verified against RetroTINK 4K Pro live hardware.

---

## ✅ Completed: Unified Scalable Video Switch (SVS) Bridge Module
- [x] **Single Codebase & Dual-Device Hardware Auto-Detection:**
  - Auto-detects connected USB peripheral upon plug event:
    - **RetroTINK 4K:** CDC-ACM (`VID 0x0525`), runs at 2,000,000 baud with WebCtl canvas bridge, community profiles, and GameID polling.
    - **Scalable Video Switch (SVS):** CH340 (`VID 0x1a86`), runs at 9,600 baud with SVS matrix switcher protocol.
  - Native SVS control methods in `DeviceManager`:
    - Direct input switching (`SVS_Change_Input_<N>`), seeking (`SVS_Input_Seek_Up/Down`), stepping (`SVS_Input_Up/Down`), attract mode, and power toggle.
    - SVS banner parser extracting live firmware version (`SVS_FW_1.20`), total inputs (`SVS TOTAL INPUTS=6`), and active input.
  - SVS REST API endpoints:
    - `GET /api/v1/svs/status`: Reports connection state, firmware version, current input, total inputs, and power state.
    - `POST /api/v1/svs/input`: Instant 1-click input switching.
    - `POST /api/v1/svs/seek`: Active video signal seek (up/down).
    - `POST /api/v1/svs/power`: Toggle SVS power / standby.
    - `POST /api/v1/svs/command`: Guardrail-sanitized raw SVS commands.
  - Flashed directly onto standalone ESP32 Nano SVS bridge node.

---

*Updated: October 10, 2026*
