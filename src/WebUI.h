#pragma once

namespace DonutRemix {

// Embedded modern single page application for Donut Remix Web UI
const char WEB_UI_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Donut Remix | RT4K Automation</title>
  <style>
    :root {
      --bg: #121316;
      --card-bg: #1a1c23;
      --border: #2e323f;
      --text: #e1e4ea;
      --text-muted: #8b92a5;
      --accent: #e56717;
      --accent-hover: #ff7a26;
      --success: #22c55e;
      --danger: #ef4444;
      --info: #3b82f6;
    }
    body.theme-light {
      --bg: #f8fafc;
      --card-bg: #ffffff;
      --border: #e2e8f0;
      --text: #0f172a;
      --text-muted: #64748b;
      --accent: #d97706;
      --accent-hover: #b45309;
    }
    body.theme-glazed {
      --bg: #2b1f1d;
      --card-bg: #3a2b27;
      --border: #523f3b;
      --text: #fce7db;
      --text-muted: #c9ada7;
      --accent: #f472b6;
      --accent-hover: #ec4899;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif; }
    body { background: var(--bg); color: var(--text); min-height: 100vh; display: flex; flex-direction: column; }
    header { background: var(--card-bg); border-bottom: 1px solid var(--border); padding: 12px 20px; display: flex; align-items: center; justify-content: space-between; }
    .logo { font-size: 1.25rem; font-weight: 700; color: var(--accent); display: flex; align-items: center; gap: 8px; }
    .badge { background: var(--border); font-size: 0.75rem; padding: 2px 8px; border-radius: 9999px; color: var(--text-muted); }
    nav { display: flex; gap: 8px; background: var(--card-bg); padding: 8px 20px; border-bottom: 1px solid var(--border); overflow-x: auto; }
    .nav-btn { background: none; border: none; color: var(--text-muted); padding: 8px 16px; border-radius: 6px; cursor: pointer; font-weight: 500; font-size: 0.9rem; transition: all 0.2s; white-space: nowrap; }
    .nav-btn.active, .nav-btn:hover { color: var(--text); background: var(--border); }
    .nav-btn.active { color: var(--accent); font-weight: 600; }
    main { flex: 1; padding: 20px; max-width: 1100px; margin: 0 auto; width: 100%; }
    .tab-content { display: none; }
    .tab-content.active { display: block; }
    .card { background: var(--card-bg); border: 1px solid var(--border); border-radius: 8px; padding: 20px; margin-bottom: 20px; }
    .card-title { font-size: 1.1rem; font-weight: 600; margin-bottom: 12px; display: flex; justify-content: space-between; align-items: center; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 16px; }
    .stat-box { background: var(--bg); border: 1px solid var(--border); border-radius: 6px; padding: 14px; }
    .stat-label { font-size: 0.8rem; color: var(--text-muted); text-transform: uppercase; margin-bottom: 4px; }
    .stat-value { font-size: 1.2rem; font-weight: 700; }
    table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 0.9rem; }
    th, td { border: 1px solid var(--border); padding: 10px 12px; text-align: left; }
    th { background: var(--bg); color: var(--text-muted); font-weight: 600; }
    input, select, button { font-size: 0.9rem; border-radius: 6px; }
    input, select { background: var(--bg); border: 1px solid var(--border); color: var(--text); padding: 8px 12px; width: 100%; }
    .btn { background: var(--accent); color: white; border: none; padding: 8px 16px; cursor: pointer; font-weight: 600; transition: background 0.2s; display: inline-flex; align-items: center; gap: 6px; }
    .btn:hover { background: var(--accent-hover); }
    .btn-secondary { background: var(--border); color: var(--text); }
    .btn-secondary:hover { background: #3b4052; }
    .btn-danger { background: var(--danger); }
    .btn-sm { padding: 4px 8px; font-size: 0.8rem; }
    .remote-grid { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; max-width: 320px; margin: 0 auto; }
    .remote-btn { padding: 16px; font-weight: bold; font-size: 1.1rem; }
    #modalBackdrop { position: fixed; inset: 0; background: rgba(0,0,0,0.7); display: none; justify-content: center; align-items: center; z-index: 100; }
    .modal { background: var(--card-bg); border: 1px solid var(--border); border-radius: 8px; max-width: 400px; width: 90%; padding: 24px; }
    .status-pill { display: inline-block; width: 10px; height: 10px; border-radius: 50%; margin-right: 6px; }
    .pill-green { background: var(--success); }
    .pill-blue { background: var(--info); }
    .pill-orange { background: var(--accent); }
    .pill-red { background: var(--danger); }
  </style>
</head>
<body>

  <header>
    <div class="logo">
      <span>🍩 Donut Remix</span>
      <span class="badge" id="fwBadge">v1.0.0</span>
    </div>
    <div style="display:flex; align-items:center; gap:12px;">
      <select id="themeSelect" style="width:auto; padding:4px 8px;" onchange="changeTheme(this.value)">
        <option value="dark">Dark</option>
        <option value="light">Light</option>
        <option value="glazed">Glazed</option>
      </select>
      <button class="btn btn-secondary btn-sm" id="authBtn" onclick="toggleAuthModal()">Login</button>
    </div>
  </header>

  <nav>
    <button class="nav-btn active" onclick="switchTab('dashboard')">Dashboard</button>
    <button class="nav-btn" onclick="switchTab('consoles')">Consoles</button>
    <button class="nav-btn" onclick="switchTab('gamedb')">Game Database</button>
    <button class="nav-btn" onclick="switchTab('remote')">Remote & Terminal</button>
    <button class="nav-btn" onclick="switchTab('files')">Files (WebCtl)</button>
    <button class="nav-btn" onclick="switchTab('security')">Security & System</button>
  </nav>

  <main>
    <!-- TAB: Dashboard -->
    <section id="tab-dashboard" class="tab-content active">
      <div class="grid">
        <div class="stat-box">
          <div class="stat-label">Active Console</div>
          <div class="stat-value" id="statKingConsole"><span class="status-pill pill-blue"></span>None</div>
        </div>
        <div class="stat-box">
          <div class="stat-label">Current Profile</div>
          <div class="stat-value" id="statProfile">--</div>
        </div>
        <div class="stat-box">
          <div class="stat-label">RT4K Connection</div>
          <div class="stat-value" id="statRt4k">USB CDC Ready</div>
        </div>
        <div class="stat-box">
          <div class="stat-label">Uptime</div>
          <div class="stat-value" id="statUptime">--</div>
        </div>
      </div>

      <div class="card" style="margin-top:20px;">
        <div class="card-title">Active Polling Status</div>
        <div id="dashboardConsolesList">Loading consoles...</div>
      </div>
    </section>

    <!-- TAB: Consoles -->
    <section id="tab-consoles" class="tab-content">
      <div class="card">
        <div class="card-title">
          <span>Configured Consoles (Max 16)</span>
          <button class="btn btn-sm" onclick="addConsoleRow()">+ Add Console</button>
        </div>
        <table id="consolesTable">
          <thead>
            <tr>
              <th style="width:40px;">On</th>
              <th>Description</th>
              <th>Endpoint URL (http/https)</th>
              <th style="width:110px;">Default Prof</th>
              <th style="width:80px;">Action</th>
            </tr>
          </thead>
          <tbody id="consolesBody"></tbody>
        </table>
        <div style="margin-top:16px; display:flex; justify-content:flex-end;">
          <button class="btn" onclick="saveConsoles()">Save Consoles</button>
        </div>
      </div>
    </section>

    <!-- TAB: Game Database -->
    <section id="tab-gamedb" class="tab-content">
      <div class="card">
        <div class="card-title">
          <span>GameID Database</span>
          <div style="display:flex; gap:8px;">
            <input type="text" id="gamedbSearch" placeholder="Search title or gameID..." style="width:200px;" oninput="filterGameDB()">
            <button class="btn btn-sm" onclick="addGameDBRow()">+ Add Game</button>
          </div>
        </div>
        <table id="gamedbTable">
          <thead>
            <tr>
              <th>Game Title</th>
              <th>gameID</th>
              <th style="width:120px;">SVS Profile</th>
              <th style="width:80px;">Action</th>
            </tr>
          </thead>
          <tbody id="gamedbBody"></tbody>
        </table>
        <div style="margin-top:16px; display:flex; justify-content:flex-end;">
          <button class="btn" onclick="saveGameDB()">Save Game Database</button>
        </div>
      </div>
    </section>

    <!-- TAB: Remote & Terminal -->
    <section id="tab-remote" class="tab-content">
      <div class="grid" style="grid-template-columns: 1fr 1fr;">
        <div class="card">
          <div class="card-title">RetroTINK 4K Remote</div>
          <div class="remote-grid">
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof1')">1</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof2')">2</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof3')">3</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof4')">4</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof5')">5</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof6')">6</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof7')">7</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof8')">8</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof9')">9</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof10')">10</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof11')">11</button>
            <button class="btn btn-secondary remote-btn" onclick="sendCmd('prof12')">12</button>
          </div>
          <div style="display:flex; justify-content:center; gap:10px; margin-top:16px;">
            <button class="btn btn-danger" onclick="sendCmd('remote pwr')">Power</button>
            <button class="btn btn-secondary" onclick="sendCmd('prevprof')">Prev Prof</button>
            <button class="btn btn-secondary" onclick="sendCmd('tv pwr')">TV Power (IR)</button>
          </div>
        </div>

        <div class="card">
          <div class="card-title">Serial Terminal</div>
          <div id="termLog" style="background:#000; color:#22c55e; font-family:monospace; padding:12px; height:260px; overflow-y:auto; border-radius:6px; font-size:0.85rem; margin-bottom:12px;">Donut Remix Terminal Ready...</div>
          <form onsubmit="event.preventDefault(); sendCustomCmd();" style="display:flex; gap:8px;">
            <input type="text" id="termInput" placeholder="Enter command (e.g. status, ver, SVS NEW INPUT=1)">
            <button class="btn" type="submit">Send</button>
          </form>
        </div>
      </div>
    </section>

    <!-- TAB: Files (WebCtl) -->
    <section id="tab-files" class="tab-content">
      <div class="card">
        <div class="card-title">
          <span>SD Card File Manager (WebCtl Bridge)</span>
          <button class="btn btn-secondary btn-sm" onclick="loadFiles()">Refresh Files</button>
        </div>
        <div id="fileList">Click 'Refresh Files' to query RT4K SD card over serial.</div>
      </div>
    </section>

    <!-- TAB: Security & System -->
    <section id="tab-security" class="tab-content">
      <div class="grid">
        <div class="card">
          <div class="card-title">Security Settings</div>
          <div style="display:flex; flex-direction:column; gap:12px;">
            <label>Admin Password</label>
            <input type="password" id="newPass" placeholder="Enter new admin password">
            <button class="btn" onclick="changePassword()">Update Password</button>
            <hr style="border-color:var(--border); margin:8px 0;">
            <label style="display:flex; align-items:center; gap:8px; cursor:pointer;">
              <input type="checkbox" id="authRequiredCheck" style="width:auto;"> Require Login for API & Web
            </label>
            <button class="btn btn-secondary btn-sm" onclick="saveAuthReq()">Save Policy</button>
          </div>
        </div>

        <div class="card">
          <div class="card-title">Firmware & Backup</div>
          <div style="display:flex; flex-direction:column; gap:12px;">
            <button class="btn btn-secondary" onclick="downloadBackup()">Export Config JSON</button>
            <label>Import Config</label>
            <input type="file" id="importFileInput" accept=".json" onchange="uploadBackup(event)">
            <hr style="border-color:var(--border); margin:8px 0;">
            <label>Authenticated Firmware Flash (.bin)</label>
            <input type="file" id="fwFileInput" accept=".bin">
            <button class="btn btn-danger" onclick="flashFirmware()">Upload & Flash</button>
            <div id="flashStatus" style="font-size:0.85rem; color:var(--text-muted);"></div>
          </div>
        </div>
      </div>
    </section>
  </main>

  <!-- Login Modal -->
  <div id="modalBackdrop">
    <div class="modal">
      <h3 style="margin-bottom:16px;">Admin Authentication</h3>
      <input type="password" id="loginPass" placeholder="Password" style="margin-bottom:16px;" onkeydown="if(event.key==='Enter') doLogin()">
      <div style="display:flex; justify-content:flex-end; gap:8px;">
        <button class="btn btn-secondary" onclick="toggleAuthModal()">Cancel</button>
        <button class="btn" onclick="doLogin()">Login</button>
      </div>
    </div>
  </div>

  <script>
    let sessionToken = localStorage.getItem('donut_token') || '';
    let currentTheme = localStorage.getItem('donut_theme') || 'dark';

    function init() {
      applyTheme(currentTheme);
      document.getElementById('themeSelect').value = currentTheme;
      updateAuthBtn();
      fetchStatus();
      setInterval(fetchStatus, 3000);
      loadConsoles();
      loadGameDB();
    }

    function switchTab(tabId) {
      document.querySelectorAll('.tab-content').forEach(el => el.classList.remove('active'));
      document.querySelectorAll('.nav-btn').forEach(el => el.classList.remove('active'));
      const target = document.getElementById('tab-' + tabId);
      if (target) target.classList.add('active');
      event.target.classList.add('active');
    }

    function applyTheme(theme) {
      document.body.className = '';
      if (theme !== 'dark') document.body.classList.add('theme-' + theme);
      localStorage.setItem('donut_theme', theme);
    }

    function changeTheme(theme) {
      currentTheme = theme;
      applyTheme(theme);
    }

    function updateAuthBtn() {
      const btn = document.getElementById('authBtn');
      if (sessionToken) {
        btn.innerText = 'Logout';
        btn.onclick = doLogout;
      } else {
        btn.innerText = 'Login';
        btn.onclick = toggleAuthModal;
      }
    }

    function toggleAuthModal() {
      const modal = document.getElementById('modalBackdrop');
      modal.style.display = (modal.style.display === 'flex') ? 'none' : 'flex';
      if (modal.style.display === 'flex') document.getElementById('loginPass').focus();
    }

    function getHeaders() {
      const h = { 'Content-Type': 'application/json' };
      if (sessionToken) h['Authorization'] = 'Bearer ' + sessionToken;
      return h;
    }

    async function doLogin() {
      const pwd = document.getElementById('loginPass').value;
      try {
        const res = await fetch('/api/v1/auth/login', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ password: pwd })
        });
        const data = await res.json();
        if (data.token) {
          sessionToken = data.token;
          localStorage.setItem('donut_token', sessionToken);
          updateAuthBtn();
          toggleAuthModal();
          alert('Logged in successfully');
        } else {
          alert(data.error || 'Login failed');
        }
      } catch (err) {
        alert('Login error: ' + err.message);
      }
    }

    async function doLogout() {
      if (sessionToken) {
        await fetch('/api/v1/auth/logout', { method: 'POST', headers: getHeaders() });
      }
      sessionToken = '';
      localStorage.removeItem('donut_token');
      updateAuthBtn();
      alert('Logged out');
    }

    async function fetchStatus() {
      try {
        const res = await fetch('/api/v1/status', { headers: getHeaders() });
        if (!res.ok) return;
        const d = await res.json();
        document.getElementById('statUptime').innerText = d.uptime || '--';
        document.getElementById('statProfile').innerText = d.active_profile || '--';
        document.getElementById('statKingConsole').innerText = d.active_console || 'None';
      } catch (e) {}
    }

    async function loadConsoles() {
      try {
        const res = await fetch('/api/v1/consoles', { headers: getHeaders() });
        if (!res.ok) return;
        const list = await res.json();
        const tbody = document.getElementById('consolesBody');
        tbody.innerHTML = '';
        list.forEach((c, idx) => {
          tbody.innerHTML += `<tr>
            <td style="text-align:center;"><input type="checkbox" ${c.enabled ? 'checked' : ''} style="width:auto;" id="c_en_${idx}"></td>
            <td><input type="text" value="${c.name || ''}" id="c_name_${idx}"></td>
            <td><input type="text" value="${c.url || ''}" id="c_url_${idx}"></td>
            <td><input type="number" value="${c.defaultProf}" id="c_prof_${idx}"></td>
            <td><button class="btn btn-danger btn-sm" onclick="this.closest('tr').remove()">Delete</button></td>
          </tr>`;
        });
      } catch (e) {}
    }

    function addConsoleRow() {
      const tbody = document.getElementById('consolesBody');
      const idx = tbody.children.length;
      tbody.innerHTML += `<tr>
        <td style="text-align:center;"><input type="checkbox" checked style="width:auto;" id="c_en_${idx}"></td>
        <td><input type="text" placeholder="Console Name" id="c_name_${idx}"></td>
        <td><input type="text" placeholder="http://ip/gameid" id="c_url_${idx}"></td>
        <td><input type="number" value="1" id="c_prof_${idx}"></td>
        <td><button class="btn btn-danger btn-sm" onclick="this.closest('tr').remove()">Delete</button></td>
      </tr>`;
    }

    async function saveConsoles() {
      const tbody = document.getElementById('consolesBody');
      const payload = [];
      for (let i = 0; i < tbody.children.length; i++) {
        const row = tbody.children[i];
        const inputs = row.querySelectorAll('input');
        payload.push({
          enabled: inputs[0].checked,
          name: inputs[1].value,
          url: inputs[2].value,
          defaultProf: parseInt(inputs[3].value, 10)
        });
      }
      const res = await fetch('/api/v1/consoles', {
        method: 'POST',
        headers: getHeaders(),
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (res.ok) alert('Consoles saved successfully');
      else alert(data.error || 'Failed to save consoles');
    }

    async function loadGameDB() {
      try {
        const res = await fetch('/api/v1/gamedb', { headers: getHeaders() });
        if (!res.ok) return;
        const list = await res.json();
        const tbody = document.getElementById('gamedbBody');
        tbody.innerHTML = '';
        list.forEach((g, idx) => {
          tbody.innerHTML += `<tr>
            <td><input type="text" value="${g.name || ''}" id="g_name_${idx}"></td>
            <td><input type="text" value="${g.gameID || ''}" id="g_id_${idx}"></td>
            <td><input type="text" value="${g.profile || ''}" id="g_prof_${idx}"></td>
            <td><button class="btn btn-danger btn-sm" onclick="this.closest('tr').remove()">Delete</button></td>
          </tr>`;
        });
      } catch (e) {}
    }

    function addGameDBRow() {
      const tbody = document.getElementById('gamedbBody');
      const idx = tbody.children.length;
      tbody.innerHTML += `<tr>
        <td><input type="text" placeholder="Game Title" id="g_name_${idx}"></td>
        <td><input type="text" placeholder="Game ID" id="g_id_${idx}"></td>
        <td><input type="text" placeholder="Profile Num" id="g_prof_${idx}"></td>
        <td><button class="btn btn-danger btn-sm" onclick="this.closest('tr').remove()">Delete</button></td>
      </tr>`;
    }

    async function saveGameDB() {
      const tbody = document.getElementById('gamedbBody');
      const payload = [];
      for (let i = 0; i < tbody.children.length; i++) {
        const row = tbody.children[i];
        const inputs = row.querySelectorAll('input');
        payload.push({
          name: inputs[0].value,
          gameID: inputs[1].value,
          profile: inputs[2].value
        });
      }
      const res = await fetch('/api/v1/gamedb', {
        method: 'POST',
        headers: getHeaders(),
        body: JSON.stringify(payload)
      });
      const data = await res.json();
      if (res.ok) alert('Game Database saved');
      else alert(data.error || 'Failed to save database');
    }

    async function sendCmd(cmd) {
      const log = document.getElementById('termLog');
      log.innerHTML += `<div>> ${cmd}</div>`;
      log.scrollTop = log.scrollHeight;
      await fetch('/api/v1/command', {
        method: 'POST',
        headers: getHeaders(),
        body: JSON.stringify({ command: cmd })
      });
    }

    function sendCustomCmd() {
      const inp = document.getElementById('termInput');
      if (!inp.value) return;
      sendCmd(inp.value);
      inp.value = '';
    }

    async function flashFirmware() {
      const fileInput = document.getElementById('fwFileInput');
      const status = document.getElementById('flashStatus');
      if (!fileInput.files.length) return alert('Select a .bin file');

      const formData = new FormData();
      formData.append('update', fileInput.files[0]);

      status.innerText = 'Uploading firmware...';
      const res = await fetch('/api/v1/system/update', {
        method: 'POST',
        headers: { 'Authorization': 'Bearer ' + sessionToken },
        body: formData
      });
      if (res.ok) {
        status.innerText = 'Flash successful! Rebooting...';
      } else {
        status.innerText = 'Flash failed!';
      }
    }

    window.addEventListener('DOMContentLoaded', init);
  </script>
</body>
</html>)rawliteral";

} // namespace DonutRemix
