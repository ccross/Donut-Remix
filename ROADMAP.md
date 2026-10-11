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

## ⏸️ On Hold / Future: Scalable Video Switch (SVS) Module Architecture
- **Decision:** To prevent resource starvation and avoid pushing the RT4K controller beyond memory/timing limits (USB CDC + OSD canvas streaming + WebCtl + WiFi + GameID polling), SVS support will be developed as a standalone architecture:
  - **Option A (Dedicated Hardware):** Run SVS control on its own dedicated ESP32 Nano / SVS Bridge node on the network.
  - **Option B (Separate Specialized Sketch):** A dedicated firmware build/sketch that targets the SVS hardware specifically while reusing the core Donut Remix network & security libraries.
  - **Option C (Hardware Autodetection):** Optional runtime auto-detection if running in single-device mode.

---

*Updated: October 10, 2026*
