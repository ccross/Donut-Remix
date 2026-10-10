# Security Audit & Threat Assessment: DonutShop & Firmware v0.7.27

**Target Repository:** `https://github.com/svirant/DonutShop`  
**Audited Targets:**
1. Published Source Code: `DonutShop.ino` (commit `48942eb`, v0.6.3)
2. Release Firmware Binary: `DonutShop_v0.7.27_full.bin` (SHA-256 `bdbea36d5a0b594c31207400d590ca969cf4a83d989480877f71b50043bef3f6`)
**Date:** October 2026  
**Auditor:** Donut Remix Security Initiative  

---

## 1. Executive Summary

DonutShop is an ESP32-S3 (Arduino Nano ESP32) bridge designed to automate profile switching and remote control for the RetroTINK 4K (RT4K) and video matrix switches based on `gameID` queries across local network consoles (such as PS1Digital, N64Digital, MemCardPro 2).

An audit was conducted on both the published source code and the release firmware binary (`v0.7.27`). The audit revealed **critical vulnerabilities** that pose direct security risks to a user's home network:
- **Zero Authentication & Authorization:** Neither the web interface nor the REST API endpoints enforce authentication.
- **Trivial Remote Code Execution (RCE):** Anyone on the local network (or a malicious website visited by a user via Cross-Site Request Forgery) can flash arbitrary firmware onto the ESP32.
- **Arbitrary Hardware & Peripheral Control:** Unauthenticated endpoints expose raw serial, USB CDC, and IR remote commands to attached home theater gear.
- **Server-Side Request Forgery (SSRF) & Unrestricted Probing:** The device can be commanded to continuously poll internal network devices without URL validation or safety guardrails.
- **Disabled TLS Certificate Verification:** Outbound HTTPS requests explicitly disable certificate verification (`setInsecure()`).
- **Memory Safety Flaws:** Multiple buffer overflows exist due to missing bounds checks on fixed-size array allocations.
- **Supply Chain & Licensing Discrepancy:** The repository source code was left stranded at version 0.6.3, while binary releases (up to v0.7.27) contain substantial undocumented features (WebCtl, REST file manager, WebSockets) distributed solely in precompiled binary form in violation of the project's GNU GPLv3 license.

---

## 2. Threat Model for Local IoT Dongles

Home network devices like the Arduino Nano ESP32 occupy a trusted position behind the home router's firewall. When an IoT dongle lacks authentication and web security headers (CORS, CSRF protection):

```
+-----------------------------------------------------------+
|                      Home LAN                             |
|                                                           |
|  [Attacker / Malicious Webpage]                           |
|            |                                              |
|            | CSRF / Direct HTTP                           |
|            v                                              |
|  [DonutShop ESP32] <--------------------+                 |
|            |                            |                 |
|            +---> Unauthenticated Flashing (RCE)           |
|            +---> Raw Serial / IR to RT4K / Consoles       |
|            +---> Continuous Internal HTTP Probing (SSRF)  |
|            +---> RT4K SD Card File Tampering              |
|                                                           |
+-----------------------------------------------------------+
```

1. **Ambient Credential & CSRF Abuse:** A user browsing the web who visits an untrusted website can execute JavaScript that makes blind `POST` requests to `http://donutshop.local/update` or `http://donutshop.local/cmd`. Because browsers on the LAN can communicate with `.local` or internal IP addresses and the device sets no CORS or CSRF restrictions, the external site can reprogram the dongle or tamper with connected equipment.
2. **Network Pivot / Persistent LAN Foothold:** Once arbitrary firmware is flashed onto the ESP32, it becomes a persistent, low-power backdoor inside the user's home network, capable of sniffing traffic, attacking local devices, or tunneling traffic.

---

## 3. Vulnerability Findings

### VULN-01: Unauthenticated Firmware Upload / Web RCE (Severity: CRITICAL)
- **Affected Endpoints:** `POST /update`
- **Location in Source:** `DonutShop.ino` lines 3024–3058 (`handleUpdate`, `handleUpdateUpload`)
- **Firmware Status:** Present in v0.7.27 binary (`/update`, `/update?size=`)
- **Description:**  
  The web server registers a firmware upload handler using Arduino's `Update` library:
  ```cpp
  server.on("/update", HTTP_POST, handleUpdate, handleUpdateUpload);
  ```
  The upload handler accepts raw `.bin` multipart uploads and writes them straight to flash memory without:
  1. Requiring an admin password, session token, or API key.
  2. Verifying a cryptographic signature or hash.
  3. Verifying a CSRF token or origin header.
- **Impact:** Any entity on the LAN, or any malicious site visited by a user on the LAN via CSRF, can replace the device's firmware.

---

### VULN-02: Unauthenticated ArduinoOTA on Port 3232 (Severity: CRITICAL)
- **Location in Source:** `DonutShop.ino` lines 577–588 (`OTAsetup`)
- **Description:**  
  The sketch initializes `ArduinoOTA` as follows:
  ```cpp
  void OTAsetup(){
    ArduinoOTA.setHostname(donuthostname);
    ArduinoOTA.onError([](ota_error_t error){ ... });
    ArduinoOTA.begin();
  }
  ```
  `ArduinoOTA.setPassword(...)` is never called.
- **Impact:** Standard Arduino OTA port `3232` is left completely unauthenticated. Any device running `espota.py` or the Arduino IDE can overwrite the firmware over the air without credentials.

---

### VULN-03: Unauthenticated Hardware & Peripheral Control (Severity: HIGH)
- **Affected Endpoints:** `POST /cmd`, `POST /api/command`
- **Location in Source:** `DonutShop.ino` lines 3059–3088 (`handleSendCMD`)
- **Firmware Status:** Present in v0.7.27 binary (`/cmd`, `/api/command`)
- **Description:**  
  The `/cmd` endpoint executes raw serial strings to the RetroTINK 4K, video matrix switches, and fires arbitrary IR codes:
  ```cpp
  void handleSendCMD(){
    String cmd = server.arg("plain");
    if(cmd.substring(0,6) == "tv pwr") { sendIR(auxpower,0,1); }
    ...
    else { dualSerialPrint(cmd); }
  }
  ```
  In the v0.7.27 binary, `/api/command` forwards raw text commands directly to the serial bridge.
- **Impact:** Allows unauthenticated callers to manipulate video output, power cycles, and settings on connected physical hardware.

---

### VULN-04: Unauthenticated SD Card & File System Operations (Severity: HIGH)
- **Affected Endpoints:** `GET /api/files/*`, `POST /api/files/mkdir`, `POST /api/files/move`
- **Firmware Status:** Present in v0.7.27 binary
- **Description:**  
  The v0.7.27 firmware adds REST endpoints to manipulate files on the RetroTINK 4K's SD card over serial. None of these endpoints require authentication or authorization.
- **Impact:** Unauthenticated LAN clients can delete, overwrite, read, or corrupt profile configurations, firmware staging files, and SD card content on the user's RetroTINK 4K.

---

### VULN-05: Server-Side Request Forgery (SSRF) & Unrestricted Polling (Severity: HIGH)
- **Affected Endpoints:** `POST /updateConsoles`, `POST /importAll`
- **Location in Source:** `DonutShop.ino` lines 590–697 (`readGameID`)
- **Description:**  
  The `readGameID()` function loops indefinitely every 2 seconds over the `consoles` array, issuing `HTTP GET` requests:
  ```cpp
  if(consoles[i].Address.substring(0,5) == "https") http.begin(https, consoles[i].Address);
  else http.begin(consoles[i].Address);
  int httpCode = http.GET();
  ```
  Because `/updateConsoles` is unauthenticated and accepts arbitrary addresses, an attacker can configure URLs targeting sensitive internal services (e.g. `http://192.168.1.1/setup.cgi`, cloud metadata addresses `http://169.254.169.254`, or internal cameras). The ESP32 will continuously hammer these endpoints.
- **Impact:** Internal network mapping, abuse of internal HTTP endpoints, and denial of service.

---

### VULN-06: Complete Insecurity of Outbound TLS (Severity: HIGH)
- **Location in Source:** `DonutShop.ino` line 601 (`readGameID`) & line 2976 (`handleGithubAssetProxy`)
- **Description:**  
  Every HTTPS connection explicitly disables certificate verification:
  ```cpp
  WiFiClientSecure https;
  https.setInsecure();
  ```
- **Impact:** When proxying firmware downloads or querying HTTPS consoles, connections are vulnerable to local ARP poisoning or DNS spoofing, enabling Man-in-the-Middle (MITM) attackers to serve malicious payloads.

---

### VULN-07: Memory Corruption & Array Out-of-Bounds Write (Severity: MEDIUM)
- **Location in Source:** `DonutShop.ino` lines 2633–2641 (`handleUpdateConsoles`), lines 2566–2572 (`handleUpdateGameDB`)
- **Description:**  
  Fixed-size arrays are populated from incoming JSON payloads without checking against maximum capacity:
  ```cpp
  #define MAX_CONSOLES 10
  Console consoles[MAX_CONSOLES];

  void handleUpdateConsoles(){
    ...
    int newSize = 0;
    for(JsonObject obj : arr){
      consoles[newSize].Desc = obj["Desc"].as<String>();
      ...
      newSize++; // NO BOUNDS CHECK against MAX_CONSOLES!
    }
  }
  ```
- **Impact:** Sending more than 10 console objects or more than 1,000 GameDB objects triggers a buffer overflow in RAM, corrupting adjacent heap/BSS data structures and crashing or destabilizing the microcontroller.

---

### VULN-08: Concurrency Race Conditions across FreeRTOS Tasks (Severity: MEDIUM)
- **Location in Source:** `DonutShop.ino` lines 526–527
- **Description:**  
  The sketch starts two preemptive FreeRTOS tasks:
  ```cpp
  xTaskCreate(DDloop,"DDloop",16384,NULL,1,NULL);
  xTaskCreate(GIDloop,"GIDloop",16384,NULL,1,NULL);
  ```
  Both tasks, along with the main web server task, read and write shared globals (`consoles`, `gameDB`, `mswitch`, `currentProf`, `payload`) without any mutexes or semaphores.
- **Impact:** Unsynchronized concurrent reads and writes cause race conditions, data tearing, and sporadic FreeRTOS watchdog panics.

---

### VULN-09: Unencrypted Captive Portal / Open AP Fallback (Severity: LOW)
- **Location in Source:** `DonutShop.ino` lines 472–484
- **Description:**  
  If WiFi connection drops, `WiFiManager` spawns an open AP (`DonutShop_Setup`) with no encryption or password.
- **Impact:** Anyone in physical WiFi range can connect to the access point and view or alter WiFi credentials.

---

### VULN-10: GPLv3 Non-Compliance & Closed Firmware Releases (Supply Chain)
- **Description:**  
  `DonutShop.ino` is licensed under the GNU General Public License v3.0 (GPLv3). However:
  1. The public repository's `.ino` file has not been updated since v0.6.3 (August 2026).
  2. Binary releases through v0.7.27 distribute compiled versions containing closed modifications (WebCtl, serial bridge, SD card file manager, theme engine).
  3. Multiple community inquiries (e.g. Issue #8) asking for the source code to v0.7.x remain unaddressed.
- **Impact:** Users are forced to flash opaque precompiled binaries without the ability to inspect source code, verify build integrity, or ensure security compliance.

---

## 4. Remediation Architecture in Donut Remix

To resolve all discovered security flaws, **Donut Remix** replaces the monolithic sketch with a clean, contract-first modular architecture:

| Component | Responsibility & Security Controls |
|---|---|
| **`src/security/`** | **Authentication & Sessions:** Enforces password-protected admin access, cryptographic session tokens, timing-safe string comparison, and brute-force rate limiting. |
| **`src/guardrails/`** | **Network & Input Guardrails:** Validates all URLs before polling, blocks loopback and cloud metadata SSRF targets, sanitizes command inputs, and validates array sizes. |
| **`src/config/`** | **Safe Configuration Store:** Atomic LittleFS configuration saves, strict schema parsing with size limits, preventing memory corruption. |
| **`src/devices/`** | **Hardware Abstraction & Concurrency:** Serial/USB/IR peripheral access guarded by FreeRTOS mutexes to eliminate race conditions. |
| **`src/gameid/`** | **Resilient Game Poller:** Safe HTTP client with strict connection timeouts and backoff. |
| **`src/web/`** | **Modern REST API & UI:** Clean REST endpoints with CORS control, security headers (`X-Frame-Options: DENY`, `X-Content-Type-Options: nosniff`), and CSRF token protection. |
| **`src/ota/`** | **Protected Updates:** Authenticated web updates requiring admin session and ArduinoOTA protected by password hash. |
