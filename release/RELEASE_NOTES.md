# 🍩 Donut Remix v1.0.0

Donut Remix is an open-source, security-hardened upgrade to DonutShop for the **Arduino Nano ESP32 (ESP32-S3)**, unifying **RetroTINK 4K** profile automation and **Scalable Video Switch (SVS)** matrix control into a single plug-and-play firmware.

---

### 🌟 Highlights & Key Features

- **Zero-Config Hardware Auto-Detection:**
  - Automatically identifies whether an attached USB OTG device is a RetroTINK 4K / 4K Pro (2,000,000 baud CDC-ACM or FTDI) or a Scalable Video Switch (9,600 baud CH340).
- **Scalable Video Switch (SVS) Web Dashboard:**
  - Responsive Single Page Application dashboard with instant channel switching (0–32), active signal seek, power/standby toggle, attract mode, and manual mode.
  - SVS Output Transcoder modules: Full management of RGB-to-Component (`R590`/`R591`), Component-to-RGB, RGsB (Sync-on-Green), and Sync Bypass expansion cards via interactive per-input modal and REST API.
  - Real-time circular-buffer serial terminal (`#conlog`) with quick command action chips.
- **100% Theme & Visual Parity with WebCtl:**
  - Authentic Donut Remix typography and header branding across both devices.
  - Dark and Glazed (pastel glassmorphism light theme) modes with instant NVS persistence (`POST /webctl-theme`) and zero white/dark flash.
  - Dropdown theme selector that opens without premature switching.
- **Authentic RetroTINK 4K WebCtl Suite:**
  - Web remote, live canvas OSD stream, SD card file manager, and GameID auto-switching with over 1,600 community profiles.
  - Eliminated legacy `<iframe>` architecture in favor of native responsive cards.
- **Security & Network Guardrails:**
  - Session-token authentication (`POST /api/v1/auth/login`) with SHA-256 salted hashes.
  - Brute-force rate limiting: 5 failed attempts locks out the client IP for 60 seconds.
  - Password-protected `ArduinoOTA` on port 3232.
  - SSRF protection against private, loopback, and cloud metadata probing.
  - Serial Bus Lock guardrail suspending GameID polling during firmware flashing and file transfers.
- **Microcontroller Touch API (CYD):**
  - Lightweight REST endpoints designed for ESP32 Cheap Yellow Displays and Home Assistant automations (`/api/v1/rt4k/status`, `/api/v1/rt4k/profile`, `/api/v1/rt4k/remote`).
- **Comprehensive API Documentation:**
  - Full OpenAPI 3.0 specification (`docs/openapi.yaml`) and markdown guide (`docs/API.md`).

---

### 📦 Release Assets & Checksums

| Asset | Description | Size | SHA-256 Checksum |
| :--- | :--- | :--- | :--- |
| `DonutRemix_v1.0.0.bin` | OTA / Web update binary (flash via `/update`) | 1.8 MB | `de5565d7218773f58ec34393093af8b3f9c12bd4459fddfa790e40f97d47b52f` |
| `DonutRemix_v1.0.0_full.bin` | Full factory flash image (0x000000) | 2.2 MB | `94ad9954cf9ea4239cb98bdea7a7a01b1a4a69d7621bffcd77ef23dab3f5df41` |

---

### 🚀 How to Upgrade

#### From Web Interface (OTA)
1. Open your browser to `http://donutshop.local` (or device LAN IP).
2. Go to the **Security & System** tab (or `/update`).
3. Upload `DonutRemix_v1.0.0.bin` and click **Update**.
4. The device will reboot automatically into Donut Remix v1.0.0.

#### From Command Line (curl)
```bash
curl -X POST http://<device-ip>/update \
     -H "Authorization: Bearer <your-token>" \
     -F "update=@DonutRemix_v1.0.0.bin"
```
