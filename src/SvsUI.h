#pragma once

namespace DonutRemix {

const char SVS_UI_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Donut Remix | SVS Switch Controller</title>
  <style>
    :root {
      --bg: #101216;
      --card-bg: #181b22;
      --card-hover: #222631;
      --border: #282d3b;
      --text: #e6edf3;
      --text-muted: #8b949e;
      --accent: #ff6e20;
      --accent-glow: rgba(255, 110, 32, 0.25);
      --success: #238636;
      --active-border: #38bdf8;
      --active-glow: rgba(56, 189, 248, 0.35);
      --font: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
    }
    body.theme-light {
      --bg: #f6f8fa;
      --card-bg: #ffffff;
      --card-hover: #f3f4f6;
      --border: #d0d7de;
      --text: #1f2328;
      --text-muted: #656d76;
      --accent: #d97706;
      --accent-glow: rgba(217, 119, 6, 0.2);
      --active-border: #0284c7;
      --active-glow: rgba(2, 132, 199, 0.25);
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: var(--font); }
    body { background: var(--bg); color: var(--text); min-height: 100vh; display: flex; flex-direction: column; }
    header {
      background: var(--card-bg);
      border-bottom: 1px solid var(--border);
      padding: 14px 24px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 12px;
    }
    .logo-group { display: flex; align-items: center; gap: 12px; }
    .logo-icon { font-size: 1.5rem; }
    .logo-text { font-size: 1.2rem; font-weight: 700; letter-spacing: -0.02em; }
    .logo-text span { color: var(--accent); }
    .header-badges { display: flex; align-items: center; gap: 8px; flex-wrap: wrap; }
    .badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.75rem;
      font-weight: 600;
      padding: 4px 10px;
      border-radius: 999px;
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--border);
      color: var(--text-muted);
    }
    .badge.connected { border-color: rgba(35, 134, 54, 0.5); color: #3fb950; background: rgba(35, 134, 54, 0.1); }
    .badge.connected .dot { background: #3fb950; box-shadow: 0 0 8px #3fb950; }
    .dot { width: 8px; height: 8px; border-radius: 50%; background: var(--text-muted); }
    
    main { flex: 1; padding: 24px; max-width: 1000px; margin: 0 auto; width: 100%; }
    
    .hero-card {
      background: linear-gradient(135deg, rgba(255, 110, 32, 0.08) 0%, rgba(56, 189, 248, 0.04) 100%), var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 24px;
      margin-bottom: 24px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 20px;
    }
    .hero-info { display: flex; flex-direction: column; gap: 6px; }
    .hero-label { font-size: 0.85rem; font-weight: 600; text-transform: uppercase; letter-spacing: 0.05em; color: var(--text-muted); }
    .hero-value { font-size: 2.2rem; font-weight: 800; letter-spacing: -0.02em; display: flex; align-items: center; gap: 12px; }
    .hero-sub { font-size: 0.9rem; color: var(--text-muted); }
    
    .section-title { font-size: 1.1rem; font-weight: 700; margin-bottom: 16px; display: flex; align-items: center; justify-content: space-between; }
    .inputs-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(240px, 1fr));
      gap: 16px;
      margin-bottom: 28px;
    }
    .input-card {
      background: var(--card-bg);
      border: 2px solid var(--border);
      border-radius: 12px;
      padding: 18px 20px;
      cursor: pointer;
      display: flex;
      flex-direction: column;
      gap: 10px;
      transition: all 0.2s cubic-bezier(0.16, 1, 0.3, 1);
      position: relative;
      overflow: hidden;
    }
    .input-card:hover {
      background: var(--card-hover);
      border-color: rgba(255, 110, 32, 0.4);
      transform: translateY(-2px);
    }
    .input-card.active {
      border-color: var(--active-border);
      background: rgba(56, 189, 248, 0.08);
      box-shadow: 0 0 20px var(--active-glow);
    }
    .input-card.active::before {
      content: '';
      position: absolute;
      top: 0; left: 0; right: 0;
      height: 4px;
      background: var(--active-border);
    }
    .card-top { display: flex; align-items: center; justify-content: space-between; }
    .input-num {
      font-size: 1.4rem;
      font-weight: 800;
      letter-spacing: -0.02em;
    }
    .input-status-pill {
      font-size: 0.7rem;
      font-weight: 700;
      padding: 3px 8px;
      border-radius: 999px;
      background: var(--border);
      color: var(--text-muted);
      text-transform: uppercase;
    }
    .input-card.active .input-status-pill {
      background: var(--active-border);
      color: #0b1120;
    }
    .input-name {
      font-size: 1.05rem;
      font-weight: 600;
      color: var(--text);
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .edit-label-btn {
      background: none;
      border: none;
      color: var(--text-muted);
      cursor: pointer;
      font-size: 0.85rem;
      padding: 4px;
      border-radius: 4px;
    }
    .edit-label-btn:hover { color: var(--text); background: var(--border); }
    
    .controls-panel {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 20px;
      margin-bottom: 24px;
    }
    .btn-row { display: flex; flex-wrap: wrap; gap: 10px; margin-top: 12px; }
    .btn {
      background: var(--border);
      color: var(--text);
      border: 1px solid transparent;
      padding: 10px 16px;
      border-radius: 8px;
      font-size: 0.9rem;
      font-weight: 600;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 8px;
      transition: all 0.15s ease;
    }
    .btn:hover { background: var(--card-hover); border-color: var(--text-muted); }
    .btn:active { transform: scale(0.98); }
    .btn-primary { background: var(--accent); color: #fff; }
    .btn-primary:hover { background: #e05d15; }
    .btn-active { background: rgba(56, 189, 248, 0.2); border-color: var(--active-border); color: var(--active-border); }

    .toast {
      position: fixed;
      bottom: 20px;
      right: 20px;
      background: var(--card-bg);
      border: 1px solid var(--border);
      padding: 12px 20px;
      border-radius: 8px;
      font-size: 0.9rem;
      box-shadow: 0 8px 24px rgba(0, 0, 0, 0.5);
      display: none;
      z-index: 1000;
    }
    footer {
      border-top: 1px solid var(--border);
      padding: 16px 24px;
      font-size: 0.8rem;
      color: var(--text-muted);
      text-align: center;
    }
    footer a { color: var(--accent); text-decoration: none; font-weight: 600; }
  </style>
</head>
<body>
  <header>
    <div class="logo-group">
      <div class="logo-icon">&#128256;</div>
      <div class="logo-text">Donut <span>Remix</span> | SVS</div>
    </div>
    <div class="header-badges">
      <div id="badge-conn" class="badge"><span class="dot"></span> Connecting...</div>
      <div id="badge-fw" class="badge">Firmware: --</div>
      <div id="badge-inputs" class="badge">Inputs: --</div>
      <button id="theme-btn" class="btn" style="padding: 4px 10px; font-size: 0.8rem;" onclick="toggleTheme()">Theme</button>
    </div>
  </header>

  <main>
    <div class="hero-card">
      <div class="hero-info">
        <div class="hero-label">Active Video Input</div>
        <div class="hero-value">
          <span id="hero-input-num">Input --</span>
          <span id="hero-input-name" style="font-size: 1.2rem; font-weight: 500; color: var(--text-muted);">(--)</span>
        </div>
        <div class="hero-sub" id="hero-sub">Polling switch hardware over USB Host...</div>
      </div>
      <div style="display: flex; gap: 8px;">
        <button class="btn btn-primary" onclick="seekInput('up')">&#9650; Seek Next Active</button>
        <button class="btn" onclick="seekInput('down')">&#9660; Seek Prev</button>
      </div>
    </div>

    <div class="section-title">
      <span>Switch Inputs (1 &ndash; 6)</span>
      <span style="font-size: 0.8rem; font-weight: 400; color: var(--text-muted);">Click to switch directly</span>
    </div>

    <div class="inputs-grid" id="inputs-container">
      <!-- Generated dynamically -->
    </div>

    <div class="controls-panel">
      <div class="section-title" style="margin-bottom: 0;">Switch Controls &amp; Settings</div>
      <div class="btn-row">
        <button class="btn" onclick="cycleInput('up')">Input Up (+1)</button>
        <button class="btn" onclick="cycleInput('down')">Input Down (-1)</button>
        <button id="btn-power" class="btn" onclick="togglePower()">Power: Standby</button>
        <button id="btn-attract" class="btn" onclick="toggleAttract()">Attract Mode</button>
        <button id="btn-manual" class="btn" onclick="toggleManual()">Manual Lock</button>
        <button class="btn" onclick="switchInput(0)" style="margin-left: auto;">Input 0 (Standby)</button>
      </div>
    </div>
  </main>

  <div id="toast" class="toast"></div>

  <footer>
    Donut Remix &bull; Scalable Video Switch Node &bull; <a href="/" id="rt4k-link">Open RT4K WebCtl</a>
  </footer>

  <script>
    let sessionToken = localStorage.getItem('donut_token') || '';
    let currentInput = 0;
    let totalInputs = 6;
    let isConnected = false;
    let isPower = true;
    let isAttract = false;
    let isManual = false;

    // Load custom names from localStorage
    function getInputName(idx) {
      if (idx === 0) return 'Standby / None';
      const saved = localStorage.getItem('svs_input_name_' + idx);
      if (saved) return saved;
      const defaults = { 1: 'SNES', 2: 'Genesis', 3: 'PS1', 4: 'N64', 5: 'Saturn', 6: 'Dreamcast' };
      return defaults[idx] || ('Input ' + idx);
    }

    function setInputName(idx, name) {
      localStorage.setItem('svs_input_name_' + idx, name);
      renderInputs();
      updateHero();
    }

    function editName(e, idx) {
      e.stopPropagation();
      const curr = getInputName(idx);
      const val = prompt('Enter friendly name for Input ' + idx + ':', curr);
      if (val !== null && val.trim() !== '') {
        setInputName(idx, val.trim());
      }
    }

    function showToast(msg) {
      const t = document.getElementById('toast');
      t.innerText = msg;
      t.style.display = 'block';
      setTimeout(() => { t.style.display = 'none'; }, 2500);
    }

    async function ensureAuth() {
      if (sessionToken) return sessionToken;
      try {
        const res = await fetch('/api/v1/auth/login', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ username: 'admin', password: 'donutremix' })
        });
        if (res.ok) {
          const data = await res.json();
          sessionToken = data.token;
          localStorage.setItem('donut_token', sessionToken);
          return sessionToken;
        }
      } catch (e) {}
      return '';
    }

    async function authedFetch(url, options = {}) {
      options.headers = options.headers || {};
      if (sessionToken) options.headers['Authorization'] = 'Bearer ' + sessionToken;
      let res = await fetch(url, options);
      if (res.status === 401) {
        sessionToken = '';
        localStorage.removeItem('donut_token');
        const newToken = await ensureAuth();
        if (newToken) {
          options.headers['Authorization'] = 'Bearer ' + newToken;
          res = await fetch(url, options);
        }
      }
      return res;
    }

    async function switchInput(num) {
      const prevInput = currentInput;
      currentInput = num;
      renderInputs();
      updateHero();
      try {
        const res = await authedFetch('/api/v1/svs/input', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ input: num })
        });
        if (res.ok) {
          showToast('Switched to ' + getInputName(num));
        } else {
          currentInput = prevInput;
          renderInputs();
          updateHero();
          showToast('Error switching input (' + res.status + ')');
        }
      } catch (e) {
        currentInput = prevInput;
        renderInputs();
        updateHero();
        showToast('Failed to switch input: ' + e);
      }
    }

    async function seekInput(dir) {
      try {
        const res = await authedFetch('/api/v1/svs/seek', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ dir: dir })
        });
        if (res.ok) {
          showToast('Seeking active signal ' + dir + '...');
          pollStatus();
        } else {
          showToast('Seek failed (' + res.status + ')');
        }
      } catch (e) {
        showToast('Seek error: ' + e);
      }
    }

    async function cycleInput(dir) {
      const next = dir === 'up' ? (currentInput >= totalInputs ? 1 : currentInput + 1) : (currentInput <= 1 ? totalInputs : currentInput - 1);
      switchInput(next);
    }

    async function togglePower() {
      const next = !isPower;
      isPower = next;
      updateButtons();
      try {
        const res = await authedFetch('/api/v1/svs/power', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ power: next })
        });
        if (res.ok) {
          showToast(next ? 'Power ON' : 'Standby Mode');
        } else {
          isPower = !next;
          updateButtons();
          showToast('Power toggle failed (' + res.status + ')');
        }
      } catch (e) {
        isPower = !next;
        updateButtons();
        showToast('Power toggle error: ' + e);
      }
    }

    async function toggleAttract() {
      const next = !isAttract;
      isAttract = next;
      updateButtons();
      try {
        const res = await authedFetch('/api/v1/svs/command', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ cmd: next ? 'SVS_Attract_ON' : 'SVS_Attract_OFF' })
        });
        if (res.ok) {
          showToast('Attract mode ' + (next ? 'ON' : 'OFF'));
        } else {
          isAttract = !next;
          updateButtons();
          showToast('Attract toggle failed (' + res.status + ')');
        }
      } catch (e) {
        isAttract = !next;
        updateButtons();
        showToast('Attract error: ' + e);
      }
    }

    async function toggleManual() {
      const next = !isManual;
      isManual = next;
      updateButtons();
      try {
        const res = await authedFetch('/api/v1/svs/command', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ cmd: next ? 'SVS_Manual_ON' : 'SVS_Manual_OFF' })
        });
        if (res.ok) {
          showToast('Manual mode ' + (next ? 'ON' : 'OFF'));
        } else {
          isManual = !next;
          updateButtons();
          showToast('Manual toggle failed (' + res.status + ')');
        }
      } catch (e) {
        isManual = !next;
        updateButtons();
        showToast('Manual error: ' + e);
      }
    }

    function renderInputs() {
      const c = document.getElementById('inputs-container');
      c.innerHTML = '';
      for (let i = 1; i <= totalInputs; i++) {
        const card = document.createElement('div');
        card.className = 'input-card' + (currentInput === i ? ' active' : '');
        card.onclick = () => switchInput(i);

        const top = document.createElement('div');
        top.className = 'card-top';

        const num = document.createElement('div');
        num.className = 'input-num';
        num.innerText = '#' + i;

        const pill = document.createElement('div');
        pill.className = 'input-status-pill';
        pill.innerText = (currentInput === i ? 'Active' : 'Input');

        top.appendChild(num);
        top.appendChild(pill);

        const nameRow = document.createElement('div');
        nameRow.className = 'input-name';
        
        const labelText = document.createElement('span');
        labelText.innerText = getInputName(i);

        const editBtn = document.createElement('button');
        editBtn.className = 'edit-label-btn';
        editBtn.title = 'Edit Name';
        editBtn.innerHTML = '&#9998;';
        editBtn.onclick = (e) => editName(e, i);

        nameRow.appendChild(labelText);
        nameRow.appendChild(editBtn);

        card.appendChild(top);
        card.appendChild(nameRow);
        c.appendChild(card);
      }
    }

    function updateHero() {
      const numElem = document.getElementById('hero-input-num');
      const nameElem = document.getElementById('hero-input-name');
      const subElem = document.getElementById('hero-sub');

      if (currentInput === 0) {
        numElem.innerText = 'Standby';
        nameElem.innerText = '(No Input)';
        subElem.innerText = 'Switch in standby mode. Click an input card below to route video.';
      } else {
        numElem.innerText = 'Input #' + currentInput;
        nameElem.innerText = getInputName(currentInput);
        subElem.innerText = 'Active routing to SVS Output &bull; CH340 USB CDC 9600 baud';
      }
    }

    function updateButtons() {
      const pBtn = document.getElementById('btn-power');
      pBtn.innerText = isPower ? 'Power: ON' : 'Power: Standby';
      pBtn.className = 'btn' + (isPower ? ' btn-active' : '');

      const aBtn = document.getElementById('btn-attract');
      aBtn.className = 'btn' + (isAttract ? ' btn-active' : '');

      const mBtn = document.getElementById('btn-manual');
      mBtn.className = 'btn' + (isManual ? ' btn-active' : '');
    }

    async function pollStatus() {
      try {
        const res = await fetch('/api/v1/svs/status', { cache: 'no-store' });
        if (res.ok) {
          const data = await res.json();
          isConnected = data.connected;
          if (data.total_inputs && data.total_inputs !== totalInputs) {
            totalInputs = data.total_inputs;
            renderInputs();
          }
          if (data.current_input !== currentInput) {
            currentInput = data.current_input;
            renderInputs();
            updateHero();
          }
          isPower = data.power;
          isAttract = data.attract;
          isManual = data.manual;
          updateButtons();

          // Badges
          const bConn = document.getElementById('badge-conn');
          bConn.className = 'badge connected';
          bConn.innerHTML = '<span class="dot"></span> SVS Connected';

          document.getElementById('badge-fw').innerText = 'Firmware: v' + (data.firmware || '1.20');
          document.getElementById('badge-inputs').innerText = 'Total: ' + totalInputs + ' Inputs';
        }
      } catch (e) {
        const bConn = document.getElementById('badge-conn');
        bConn.className = 'badge';
        bConn.innerHTML = '<span class="dot"></span> Offline';
      }
    }

    function toggleTheme() {
      document.body.classList.toggle('theme-light');
      localStorage.setItem('svs_theme', document.body.classList.contains('theme-light') ? 'light' : 'dark');
    }

    if (localStorage.getItem('svs_theme') === 'light') {
      document.body.classList.add('theme-light');
    }

    renderInputs();
    updateHero();
    pollStatus();
    setInterval(pollStatus, 1500);
  </script>
</body>
</html>)rawliteral";

} // namespace DonutRemix
