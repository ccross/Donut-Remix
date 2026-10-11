# Donut Remix REST API Documentation

Donut Remix provides a secure, versioned, RESTful API (`/api/v1/`) for monitoring, configuring, and controlling the RetroTINK 4K, video switches, and console gameID polling.

---

## 1. Authentication & Security

### Overview
All state-modifying requests (`POST`, `PUT`, `DELETE`) and sensitive endpoints require an authenticated session token.

### Authentication Flow
1. Send a POST request to `/api/v1/auth/login` with your admin password.
2. Receive a 32-character session token.
3. Pass the session token in subsequent requests using either:
   - **Header:** `Authorization: Bearer <token>`
   - **Alternative Header:** `X-Session-Token: <token>`

### Brute Force Protection
- Failed login attempts are tracked per client IP.
- After **5 failed attempts**, the client IP is locked out for **60 seconds** (HTTP `429 Too Many Requests`).

---

## 2. API Endpoints

### `POST /api/v1/auth/login`
Authenticate as administrator and obtain a session token.

**Request Body:**
```json
{
  "password": "yourAdminPassword"
}
```

**Response (200 OK):**
```json
{
  "token": "4a2b9f01e7c538209ad43bf1e02947ac",
  "expiresIn": 86400
}
```

---

### `POST /api/v1/auth/logout`
Invalidate the current session token.

**Headers:** `Authorization: Bearer <token>`

**Response (200 OK):**
```json
{
  "status": "ok"
}
```

---

### `GET /api/v1/status`
Retrieve real-time system, profile, and active console telemetry.

**Response (200 OK):**
```json
{
  "uptime": 14250,
  "active_profile": 7,
  "active_console": "PS1 Digital",
  "rt4k_status": "Ready"
}
```

---

### `GET /api/v1/consoles`
List all configured consoles and their live operational status.

**Response (200 OK):**
```json
[
  {
    "Desc": "PS1 Digital",
    "Address": "http://ps1digital.local/gameid",
    "DefaultProf": -9,
    "Enabled": true,
    "On": true,
    "King": true
  },
  {
    "Desc": "MemCardPro",
    "Address": "http://10.0.1.50/api/currentState",
    "DefaultProf": -5,
    "Enabled": true,
    "On": false,
    "King": false
  }
]
```

---

### `POST /api/v1/consoles`
Update the list of monitored consoles.

**Headers:** `Authorization: Bearer <token>`  
**Guardrails:**
- Maximum **16** console entries.
- URLs must be valid `http://` or `https://`.
- Loopback (`127.0.0.1`, `localhost`) and Cloud Metadata (`169.254.169.254`) addresses are blocked.
- Names are capped at 64 characters.

**Request Body:**
```json
[
  {
    "Desc": "PS1 Digital",
    "Address": "http://ps1digital.local/gameid",
    "DefaultProf": -9,
    "Enabled": true
  }
]
```

---

### `GET /api/v1/gamedb`
List gameID-to-profile mappings.

**Response (200 OK):**
```json
[
  ["N64 EverDrive", "00000000-00000000---00", "7"],
  ["Super Mario 64", "4e534d45", "1"]
]
```

---

### `POST /api/v1/gamedb`
Update gameID mappings table.

**Headers:** `Authorization: Bearer <token>`  
**Guardrails:** Maximum 2,000 entries.

**Request Body:**
```json
[
  ["N64 EverDrive", "00000000-00000000---00", "7"],
  ["Super Mario 64", "4e534d45", "1"]
]
```

---

### `POST /api/v1/command`
Send a command to the RetroTINK 4K, video switch matrix, or IR emitter.

**Headers:** `Authorization: Bearer <token>`  
**Guardrails:**
- Maximum 256 characters.
- Disallows null bytes or illegal control characters.

**Request Body:**
```json
{
  "command": "remote prof1"
}
```

---

### `GET /api/v1/rt4k/status`
Retrieve hardware connection, active profile, console detection, and performance telemetry optimized for lightweight microcontrollers (such as ESP32 Cheap Yellow Display / CYD).

**Response (200 OK):**
```json
{
  "connected": true,
  "active_profile": "SNES",
  "active_console": "Super Nintendo",
  "uptime": 1284,
  "free_heap": 159044,
  "rt4k_baud": 2000000
}
```

---

### `POST /api/v1/rt4k/remote`
Dispatch simulated remote control button actions directly to the RetroTINK 4K over USB CDC.

**Headers:** `Authorization: Bearer <token>` (or `X-Session-Token`)  
**Request Body:**
```json
{
  "button": "menu"
}
```
*Supported buttons:* `menu`, `ok`, `back`, `up`, `down`, `left`, `right`, `pwr`, `prof1` through `prof12`, `input`, `res`, `aspect`.

---

### `POST /api/v1/rt4k/profile`
Instantly switch the active RetroTINK 4K profile via index number, SVS input mapping, remote profile button, or SD card file path.

**Headers:** `Authorization: Bearer <token>`  
**Request Body Examples:**
```json
{
  "profile": 1
}
```
or by SD card profile path:
```json
{
  "name": "0:/profile/SNES/FirebrandX_1080p.cfg"
}
```
or by SVS input / remote button:
```json
{
  "svs": 3
}
```

---

### `POST /api/v1/rt4k/exec`
Synchronously execute a command on the RetroTINK 4K USB CDC bus (locked at 2,000,000 baud) and wait for the immediate serial response text.

**Headers:** `Authorization: Bearer <token>`  
**Request Body:**
```json
{
  "cmd": "prof"
}
```
**Response (200 OK):**
```json
{
  "ok": true,
  "cmd": "prof",
  "response": "Active profile: 0:/profile/SNES/FirebrandX_1080p.cfg"
}
```

---

### `GET /api/v1/system/backup`
Download a complete JSON export of all configurations, consoles, and GameDB entries.

**Headers:** `Authorization: Bearer <token>`  
**Response (200 OK):** Returns `donut_remix_backup.json` attachment.

---

### `POST /api/v1/system/update`
Securely flash new firmware onto the ESP32.

**Headers:**
- `Authorization: Bearer <token>`
- `Content-Type: multipart/form-data`

**Form field:** `update` (binary file).

---

## 3. Scalable Video Switch (SVS) Endpoints

When connected to an SVS matrix switcher via USB Host (CH340 CDC @ 9,600 baud), Donut Remix exposes dedicated real-time control, output module transcoding, and serial monitoring endpoints.

---

### `GET /api/v1/svs/status`
Retrieve real-time hardware status, active input, total inputs, power, attract/manual mode states, USB host peripheral telemetry, and recent serial log history.

**Response (200 OK):**
```json
{
  "connected": true,
  "firmware": "1.20",
  "current_input": 0,
  "total_inputs": 6,
  "power": true,
  "attract": false,
  "manual": false,
  "uptime": 240,
  "cdc_connected": true,
  "usb_host_ready": true,
  "svs_connected": true,
  "rt4k_baud": 9600,
  "usb_device_count": 1,
  "usb_devices": [
    {
      "vid": 6790,
      "pid": 29987,
      "addr": 1,
      "mfg": "",
      "prod": "USB Serial"
    }
  ],
  "last_cmd": "R590",
  "last_bytes_written": 8,
  "last_rx": "SVS CURRENT INPUT=0 SVS TOTAL INPUTS=6",
  "serial_log": [
    {
      "t": 102655,
      "d": "R",
      "m": "SVS TOTAL INPUTS=6"
    },
    {
      "t": 102666,
      "d": "R",
      "m": "SVS CURRENT INPUT=0"
    }
  ]
}
```

*Note:* `serial_log` returns up to 30 recent log entries from the thread-safe circular buffer (`t` = uptime timestamp in ms, `d` = direction `'T'` for Transmit or `'R'` for Receive, `m` = message text).

---

### `POST /api/v1/svs/input`
Switch the active SVS video input channel (0–32, where 0 indicates no active input / standby).

**Request Body (JSON):**
```json
{
  "input": 1
}
```
*Alternative form/plain encoding:* `input=1` or raw payload `1`.

**Response (200 OK):**
```json
{
  "ok": true,
  "input": 1
}
```

---

### `POST /api/v1/svs/seek`
Seek to the next or previous input channel with an active video signal.

**Request Body (JSON or URL parameters):**
```json
{
  "dir": "up"
}
```
*Supported values:* `"up"` (next active channel) or `"down"` (previous active channel).

**Response (200 OK):**
```json
{
  "ok": true,
  "direction": "up"
}
```

---

### `POST /api/v1/svs/power`
Turn SVS power on or enter standby mode.

**Request Body (JSON or URL parameters):**
```json
{
  "power": true
}
```
*Supported values:* `true` / `false`, `"on"` / `"off"`.

**Response (200 OK):**
```json
{
  "ok": true,
  "power": true
}
```

---

### `POST /api/v1/svs/command`
Send a guardrail-sanitized command directly to the SVS hardware over serial and synchronously receive the hardware response.

**Request Body (JSON):**
```json
{
  "cmd": "SVS_Attract_Mode"
}
```
*Useful commands:* `SVS_Attract_Mode`, `SVS_Manual_Mode`, `SVS_Change_Input_<N>`, `R590`, `R591`.

**Response (200 OK):**
```json
{
  "ok": true,
  "command": "SVS_Attract_Mode",
  "response": "SVS ATTRACT MODE ON",
  "bytesWritten": 20
}
```

---

### `GET /api/v1/svs/modules`
Query installed transcoder output module states across all SVS inputs (RGB-to-Component, Component-to-RGB, RGsB / Sync-on-Green, and Sync Bypass).

**Query Parameters:**
- `refresh=1` (optional): Query the SVS hardware immediately before returning.

**Response (200 OK):**
```json
{
  "ok": true,
  "total_inputs": 6,
  "current_input": 0,
  "rgb_to_comp": [false, false, false, false, false, false],
  "comp_to_rgb": [false, false, false, false, false, false],
  "rgsb": [false, false, false, false, false, false],
  "sync_bypass": [true, true, true, true, true, true]
}
```

---

### `POST /api/v1/svs/modules`
Configure or toggle transcoder output modules and sync options for a specific SVS input.

**Set explicit state:**
```json
{
  "input": 1,
  "type": "rgb_to_comp",
  "enable": true
}
```
*Supported types:* `rgb_to_comp`, `comp_to_rgb`, `rgsb`, `sync_bypass`.

**Toggle state:**
```json
{
  "input": 1,
  "action": "toggle_rgb_to_comp"
}
```
*Supported actions:* `toggle_rgb_to_comp`, `toggle_comp_to_rgb`, `toggle_rgsb`, `toggle_sync_bypass`.

**Response (200 OK):**
Returns the updated `modules` state payload matching `GET /api/v1/svs/modules`.

---

## 4. UI Customization & Theme Persistence

### `GET /webctl-theme`
Retrieve the active web interface theme.

**Response (200 OK):**
```json
{
  "ok": true,
  "theme": "dark"
}
```
*Values:* `"dark"` or `"light"` (Glazed mode).

---

### `POST /webctl-theme`
Set and persist the active web interface theme to Non-Volatile Storage (NVS).

**Request Body (JSON or Form URL-encoded):**
```
theme=light
```
*or JSON:*
```json
{
  "theme": "light"
}
```

**Response (200 OK):**
```json
{
  "ok": true,
  "theme": "light"
}
```

---

## 5. Backward Compatibility Endpoints

For compatibility with legacy scripts and home automation tools, the following endpoints remain available but are now routed through the security middleware:
- `POST /cmd`: Accepts plain text commands (authenticated).
- `GET /getConsoles`, `POST /updateConsoles`
- `GET /getGameDB`, `POST /updateGameDB`
- `GET /getVars`, `POST /updateVars`
- `GET /exportAll`, `POST /importAll`
