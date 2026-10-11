#pragma once

namespace DonutRemix {

const char SVS_UI_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Donut Remix | SVS Switch Controller</title>
  <style>
    :root, [data-theme="dark"] {
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
      --mono: ui-monospace, 'Cascadia Code', 'JetBrains Mono', Consolas, monospace;
      --terminal-bg: #0b0e14;
      --terminal-border: #242c3d;
    }
    [data-theme="glazed"] {
      --bg: #0b0e14;
      --bg-gradient: radial-gradient(circle at 12% 10%, rgba(118, 205, 224, 0.22), transparent 35%), radial-gradient(circle at 86% 14%, rgba(222, 176, 224, 0.20), transparent 35%), linear-gradient(135deg, #0d1117 0%, #151b23 100%);
      --card-bg: rgba(20, 25, 38, 0.85);
      --card-hover: rgba(28, 35, 52, 0.95);
      --border: rgba(255, 61, 129, 0.3);
      --text: #dfe5f1;
      --text-muted: #8b94ab;
      --accent: #ff3d81;
      --accent-glow: rgba(255, 61, 129, 0.35);
      --success: #3dffa4;
      --active-border: #3dd9ff;
      --active-glow: rgba(61, 217, 255, 0.45);
      --terminal-bg: rgba(11, 14, 20, 0.95);
      --terminal-border: rgba(61, 217, 255, 0.35);
    }
    [data-theme="light"] {
      --bg: #f6f8fa;
      --card-bg: #ffffff;
      --card-hover: #f3f4f6;
      --border: #d0d7de;
      --text: #1f2328;
      --text-muted: #656d76;
      --accent: #d97706;
      --accent-glow: rgba(217, 119, 6, 0.2);
      --success: #1a7f37;
      --active-border: #0284c7;
      --active-glow: rgba(2, 132, 199, 0.25);
      --terminal-bg: #f6f8fa;
      --terminal-border: #d0d7de;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: var(--font); }
    body { background: var(--bg); background-image: var(--bg-gradient, none); color: var(--text); min-height: 100vh; display: flex; flex-direction: column; }
    
    .theme-menu-container { position: relative; }
    .theme-dropdown {
      position: absolute;
      top: calc(100% + 6px);
      right: 0;
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 8px;
      padding: 6px;
      min-width: 140px;
      box-shadow: 0 8px 24px rgba(0,0,0,0.4);
      z-index: 1000;
      backdrop-filter: blur(12px);
    }
    .theme-item {
      display: flex;
      align-items: center;
      gap: 8px;
      padding: 7px 10px;
      border-radius: 6px;
      font-size: 0.85rem;
      font-weight: 500;
      cursor: pointer;
      color: var(--text);
      transition: background 0.12s;
    }
    .theme-item:hover { background: var(--card-hover); }
    .theme-item.selected { font-weight: 700; color: var(--accent); }
    .theme-check { margin-left: auto; font-size: 0.85rem; visibility: hidden; }
    .theme-item.selected .theme-check { visibility: visible; }
    .swatch { width: 10px; height: 10px; border-radius: 50%; border: 1px solid var(--border); }
    .swatch-dark { background: #181b22; }
    .swatch-glazed { background: linear-gradient(135deg, #ff3d81, #3dd9ff); }
    .swatch-light { background: #ffffff; }

    .terminal-card {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 12px;
      padding: 20px;
      margin-bottom: 24px;
    }
    .terminal-header {
      display: flex;
      align-items: center;
      justify-content: space-between;
      flex-wrap: wrap;
      gap: 10px;
      margin-bottom: 12px;
    }
    .terminal-title {
      font-size: 1.05rem;
      font-weight: 700;
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .terminal-controls {
      display: flex;
      align-items: center;
      gap: 12px;
      font-size: 0.8rem;
      color: var(--text-muted);
    }
    .terminal-controls label {
      display: flex;
      align-items: center;
      gap: 5px;
      cursor: pointer;
      user-select: none;
    }
    .terminal-log {
      font-family: var(--mono);
      font-size: 0.82rem;
      line-height: 1.6;
      background: var(--terminal-bg);
      border: 1px solid var(--terminal-border);
      border-radius: 8px;
      padding: 12px 16px;
      height: 220px;
      overflow-y: auto;
      white-space: pre-wrap;
      word-break: break-all;
    }
    .cl-tx { color: #38bdf8; }
    .cl-rx { color: #4ade80; }
    .cl-sys { color: #94a3b8; font-style: italic; }
    .cl-err { color: #f87171; }
    .cl-time { color: var(--text-muted); opacity: 0.6; user-select: none; margin-right: 6px; font-size: 0.75rem; }
    .terminal-form {
      display: flex;
      gap: 10px;
      margin-top: 12px;
    }
    .terminal-input {
      flex: 1;
      font-family: var(--mono);
      font-size: 0.85rem;
      background: var(--bg);
      border: 1px solid var(--border);
      color: var(--text);
      padding: 8px 12px;
      border-radius: 6px;
      outline: none;
    }
    .terminal-input:focus {
      border-color: var(--accent);
    }
    .terminal-chips {
      display: flex;
      gap: 6px;
      flex-wrap: wrap;
      margin-top: 10px;
    }
    .tchip {
      font-family: var(--mono);
      font-size: 0.75rem;
      border: 1px solid var(--border);
      background: rgba(255, 255, 255, 0.04);
      border-radius: 999px;
      padding: 3px 10px;
      color: var(--text-muted);
      cursor: pointer;
      transition: all 0.15s ease;
    }
    .tchip:hover {
      color: var(--text);
      border-color: var(--text-muted);
      background: var(--card-hover);
    }
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
    .active-toggles { display: flex; flex-wrap: wrap; gap: 8px; margin-top: 14px; }
    .pill-btn {
      background: rgba(255, 255, 255, 0.05);
      border: 1px solid var(--border);
      border-radius: 999px;
      padding: 6px 14px;
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--text-muted);
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      gap: 6px;
      transition: all 0.15s ease;
    }
    .pill-btn:hover { background: var(--card-hover); color: var(--text); border-color: var(--text-muted); }
    .pill-btn.active {
      border-color: #38bdf8;
      background: rgba(56, 189, 248, 0.15);
      color: #38bdf8;
    }
    .pill-btn.active .pill-dot { background: #38bdf8; box-shadow: 0 0 6px #38bdf8; }
    .pill-dot { width: 6px; height: 6px; border-radius: 50%; background: var(--text-muted); }

    .card-badges { display: flex; flex-wrap: wrap; gap: 4px; margin-top: 6px; }
    .mod-pill {
      font-size: 0.65rem;
      font-weight: 700;
      padding: 2px 6px;
      border-radius: 4px;
      background: rgba(255, 255, 255, 0.08);
      border: 1px solid var(--border);
      color: var(--text-muted);
    }
    .mod-pill.active-y { background: rgba(255, 110, 32, 0.15); border-color: rgba(255, 110, 32, 0.5); color: #ff6e20; }
    .mod-pill.active-g { background: rgba(35, 134, 54, 0.15); border-color: rgba(35, 134, 54, 0.5); color: #3fb950; }
    .mod-pill.active-r { background: rgba(56, 189, 248, 0.15); border-color: rgba(56, 189, 248, 0.5); color: #38bdf8; }
    .mod-pill.active-s { background: rgba(168, 85, 247, 0.15); border-color: rgba(168, 85, 247, 0.5); color: #c084fc; }

    .card-actions { display: flex; align-items: center; gap: 4px; }
    .card-btn {
      background: none;
      border: none;
      color: var(--text-muted);
      cursor: pointer;
      font-size: 0.85rem;
      padding: 4px 6px;
      border-radius: 4px;
      transition: all 0.15s ease;
    }
    .card-btn:hover { color: var(--text); background: var(--border); }

    .modal-overlay {
      position: fixed;
      inset: 0;
      background: rgba(0, 0, 0, 0.7);
      backdrop-filter: blur(4px);
      display: flex;
      align-items: center;
      justify-content: center;
      z-index: 2000;
      padding: 16px;
    }
    .modal-card {
      background: var(--card-bg);
      border: 1px solid var(--border);
      border-radius: 14px;
      width: 100%;
      max-width: 480px;
      box-shadow: 0 16px 40px rgba(0, 0, 0, 0.6);
      overflow: hidden;
      display: flex;
      flex-direction: column;
    }
    .modal-header {
      padding: 16px 20px;
      border-bottom: 1px solid var(--border);
      display: flex;
      align-items: center;
      justify-content: space-between;
    }
    .modal-header h3 { font-size: 1.1rem; font-weight: 700; margin: 0; }
    .modal-close {
      background: none;
      border: none;
      font-size: 1.4rem;
      color: var(--text-muted);
      cursor: pointer;
      line-height: 1;
    }
    .modal-close:hover { color: var(--text); }
    .modal-body {
      padding: 20px;
      display: flex;
      flex-direction: column;
      gap: 14px;
      max-height: 75vh;
      overflow-y: auto;
    }
    .modal-footer {
      padding: 14px 20px;
      border-top: 1px solid var(--border);
      display: flex;
      justify-content: flex-end;
      gap: 10px;
      background: rgba(0, 0, 0, 0.1);
    }
    .toggle-row {
      display: flex;
      align-items: center;
      justify-content: space-between;
      padding: 12px 14px;
      background: rgba(255, 255, 255, 0.03);
      border: 1px solid var(--border);
      border-radius: 8px;
    }
    .toggle-desc { font-size: 0.75rem; color: var(--text-muted); margin-top: 2px; }
    .switch {
      width: 40px;
      height: 22px;
      appearance: none;
      background: var(--border);
      border-radius: 999px;
      position: relative;
      cursor: pointer;
      outline: none;
      transition: background 0.2s;
      flex-shrink: 0;
    }
    .switch:checked { background: var(--accent); }
    .switch::after {
      content: '';
      position: absolute;
      top: 3px;
      left: 3px;
      width: 16px;
      height: 16px;
      border-radius: 50%;
      background: #fff;
      transition: transform 0.2s;
    }
    .switch:checked::after { transform: translateX(18px); }
    .modal-input {
      width: 100%;
      background: var(--bg);
      border: 1px solid var(--border);
      color: var(--text);
      padding: 8px 12px;
      border-radius: 6px;
      font-size: 0.9rem;
      margin-top: 4px;
      box-sizing: border-box;
    }
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
      <div class="theme-menu-container">
        <button id="theme-btn" class="btn" style="padding: 4px 12px; font-size: 0.8rem; display: flex; align-items: center; gap: 6px;" onclick="toggleThemeMenu(event)">
          <span>Theme</span> <span style="font-size: 0.65rem;">▼</span>
        </button>
        <div id="theme-dropdown" class="theme-dropdown" style="display: none;">
          <div class="theme-item" id="choice-dark" onclick="selectTheme('dark')">
            <span class="swatch swatch-dark"></span>
            <span>Dark</span>
            <span class="theme-check" id="check-dark">&#10003;</span>
          </div>
          <div class="theme-item" id="choice-glazed" onclick="selectTheme('glazed')">
            <span class="swatch swatch-glazed"></span>
            <span>Glazed</span>
            <span class="theme-check" id="check-glazed">&#10003;</span>
          </div>
          <div class="theme-item" id="choice-light" onclick="selectTheme('light')">
            <span class="swatch swatch-light"></span>
            <span>Light</span>
            <span class="theme-check" id="check-light">&#10003;</span>
          </div>
        </div>
      </div>
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
        <div class="active-toggles" id="active-toggles">
          <button class="pill-btn" id="tog-y" onclick="toggleActiveModule('rgb_to_comp')">
            <span class="pill-dot"></span> RGB &rarr; Comp: <strong id="tog-y-val">--</strong>
          </button>
          <button class="pill-btn" id="tog-g" onclick="toggleActiveModule('comp_to_rgb')">
            <span class="pill-dot"></span> Comp &rarr; RGB: <strong id="tog-g-val">--</strong>
          </button>
          <button class="pill-btn" id="tog-r" onclick="toggleActiveModule('rgsb')">
            <span class="pill-dot"></span> Sync: <strong id="tog-r-val">--</strong>
          </button>
          <button class="pill-btn" id="tog-s" onclick="toggleActiveModule('sync_bypass')">
            <span class="pill-dot"></span> Sync Bypass: <strong id="tog-s-val">--</strong>
          </button>
        </div>
      </div>
      <div style="display: flex; gap: 8px;">
        <button class="btn btn-primary" onclick="seekInput('up')">&#9650; Seek Next Active</button>
        <button class="btn" onclick="seekInput('down')">&#9660; Seek Prev</button>
      </div>
    </div>

    <div class="section-title">
      <span>Switch Inputs (1 &ndash; 6)</span>
      <span style="font-size: 0.8rem; font-weight: 400; color: var(--text-muted);">Click to switch &bull; &#9881; for output &amp; sync settings</span>
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
        <button class="btn" onclick="fetchModules(true)">&#8635; Refresh Modules</button>
        <button class="btn" onclick="switchInput(0)" style="margin-left: auto;">Input 0 (Standby)</button>
      </div>
    </div>

    <div class="terminal-card">
      <div class="terminal-header">
        <div class="terminal-title">
          <span>Serial Terminal</span>
          <span style="font-size: 0.75rem; font-weight: 500; color: var(--text-muted);" id="term-status">(CH340 &bull; 9600 baud)</span>
        </div>
        <div class="terminal-controls">
          <label><input type="checkbox" id="term-autoscroll" checked> Auto-scroll</label>
          <label><input type="checkbox" id="term-timestamps" checked> Timestamps</label>
          <button class="btn" style="padding: 3px 8px; font-size: 0.75rem;" onclick="clearTerminal()">Clear</button>
        </div>
      </div>
      <div class="terminal-log" id="terminal-log"></div>
      <form class="terminal-form" onsubmit="sendTerminalCommand(event)">
        <input type="text" id="terminal-input" class="terminal-input" placeholder="Enter command... (e.g. SVS_Change_Input_1, Seek, R550) (&uarr;&darr; history)" autocomplete="off" spellcheck="false" />
        <button type="submit" class="btn btn-primary" style="padding: 8px 16px;">Send</button>
      </form>
      <div class="terminal-chips">
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_1')">Input 1</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_2')">Input 2</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_3')">Input 3</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_4')">Input 4</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_5')">Input 5</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Change_Input_6')">Input 6</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Input_Seek_Up')">Seek &blacktriangle;</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Input_Seek_Down')">Seek &blacktriangledown;</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Attract_ON')">Attract ON</button>
        <button type="button" class="tchip" onclick="quickCmd('SVS_Attract_OFF')">Attract OFF</button>
        <button type="button" class="tchip" onclick="quickCmd('R550')">Read 550 (RGsB)</button>
        <button type="button" class="tchip" onclick="quickCmd('R590')">Read 590 (Sync)</button>
        <button type="button" class="tchip" onclick="quickCmd('Y1')">Y1 (RGB-Comp 1)</button>
        <button type="button" class="tchip" onclick="quickCmd('G1')">G1 (Comp-RGB 1)</button>
      </div>
    </div>
  </main>

  <div id="cfg-modal" class="modal-overlay" style="display:none;" onclick="closeModal(event)">
    <div class="modal-card" onclick="event.stopPropagation()">
      <div class="modal-header">
        <h3 id="modal-title">Input Configuration</h3>
        <button class="modal-close" onclick="closeModal()">&times;</button>
      </div>
      <div class="modal-body">
        <div class="form-group">
          <label style="font-size:0.85rem; font-weight:600; color:var(--text-muted);">Channel Friendly Name:</label>
          <input type="text" id="modal-name-input" class="modal-input" placeholder="e.g. SNES" />
        </div>
        <div class="toggle-row">
          <div>
            <strong>RGB to YPbPr (Component)</strong>
            <div class="toggle-desc">Transcode RGB signal to Component output</div>
          </div>
          <input type="checkbox" id="modal-tog-y" class="switch" />
        </div>
        <div class="toggle-row">
          <div>
            <strong>YPbPr (Component) to RGB</strong>
            <div class="toggle-desc">Transcode Component signal to RGB output</div>
          </div>
          <input type="checkbox" id="modal-tog-g" class="switch" />
        </div>
        <div class="toggle-row">
          <div>
            <strong>RGBS to RGsB (Sync-on-Green)</strong>
            <div class="toggle-desc">Route sync onto green channel (for PVMs / SoG)</div>
          </div>
          <input type="checkbox" id="modal-tog-r" class="switch" />
        </div>
        <div class="toggle-row">
          <div>
            <strong>Sync Bypass (Raw CSync)</strong>
            <div class="toggle-desc">Bypass sync cleaner for raw arcade / TTL sync</div>
          </div>
          <input type="checkbox" id="modal-tog-s" class="switch" />
        </div>
      </div>
      <div class="modal-footer">
        <button class="btn" onclick="closeModal()">Cancel</button>
        <button class="btn btn-primary" onclick="saveModalConfig()">Save Settings</button>
      </div>
    </div>
  </div>

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

    let moduleData = {
      rgbToComp: [],
      compToRgb: [],
      rgsb: [],
      syncBypass: []
    };
    let modalInputIdx = 1;

    async function fetchModules(force = false) {
      try {
        const url = '/api/v1/svs/modules' + (force ? '?refresh=1' : '');
        const res = await authedFetch(url);
        if (res.ok) {
          const d = await res.json();
          moduleData.rgbToComp = d.rgb_to_comp || [];
          moduleData.compToRgb = d.comp_to_rgb || [];
          moduleData.rgsb = d.rgsb || [];
          moduleData.syncBypass = d.sync_bypass || [];
          renderInputs();
          updateHero();
          if (force) showToast('Refreshed module settings from hardware');
        }
      } catch (e) {
        console.error('Error fetching SVS modules:', e);
      }
    }

    async function toggleActiveModule(type) {
      if (currentInput <= 0) {
        showToast('Select an active input first');
        return;
      }
      try {
        const res = await authedFetch('/api/v1/svs/modules', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ action: 'toggle_' + type, input: currentInput })
        });
        if (res.ok) {
          const d = await res.json();
          moduleData.rgbToComp = d.rgb_to_comp || [];
          moduleData.compToRgb = d.comp_to_rgb || [];
          moduleData.rgsb = d.rgsb || [];
          moduleData.syncBypass = d.sync_bypass || [];
          renderInputs();
          updateHero();
          showToast('Updated ' + type + ' on Input #' + currentInput);
        } else {
          showToast('Failed to toggle ' + type);
        }
      } catch (e) {
        showToast('Error toggling module: ' + e);
      }
    }

    function openConfigModal(e, idx) {
      if (e) e.stopPropagation();
      modalInputIdx = idx;
      const title = document.getElementById('modal-title');
      title.innerText = 'Input #' + idx + ' Settings (' + getInputName(idx) + ')';
      
      document.getElementById('modal-name-input').value = getInputName(idx);
      document.getElementById('modal-tog-y').checked = !!moduleData.rgbToComp[idx - 1];
      document.getElementById('modal-tog-g').checked = !!moduleData.compToRgb[idx - 1];
      document.getElementById('modal-tog-r').checked = !!moduleData.rgsb[idx - 1];
      document.getElementById('modal-tog-s').checked = !!moduleData.syncBypass[idx - 1];

      document.getElementById('cfg-modal').style.display = 'flex';
    }

    function closeModal(e) {
      if (e && e.target !== document.getElementById('cfg-modal') && e.type === 'click' && !e.target.classList.contains('modal-close') && e.target.tagName !== 'BUTTON') return;
      document.getElementById('cfg-modal').style.display = 'none';
    }

    async function saveModalConfig() {
      const idx = modalInputIdx;
      const newName = document.getElementById('modal-name-input').value.trim();
      if (newName) {
        localStorage.setItem('svs_input_name_' + idx, newName);
      }

      const newY = document.getElementById('modal-tog-y').checked;
      const newG = document.getElementById('modal-tog-g').checked;
      const newR = document.getElementById('modal-tog-r').checked;
      const newS = document.getElementById('modal-tog-s').checked;

      const updates = [];
      if (newY !== !!moduleData.rgbToComp[idx - 1]) updates.push({ type: 'rgb_to_comp', enable: newY });
      if (newG !== !!moduleData.compToRgb[idx - 1]) updates.push({ type: 'comp_to_rgb', enable: newG });
      if (newR !== !!moduleData.rgsb[idx - 1]) updates.push({ type: 'rgsb', enable: newR });
      if (newS !== !!moduleData.syncBypass[idx - 1]) updates.push({ type: 'sync_bypass', enable: newS });

      document.getElementById('cfg-modal').style.display = 'none';

      for (const u of updates) {
        try {
          await authedFetch('/api/v1/svs/modules', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ type: u.type, input: idx, enable: u.enable })
          });
        } catch (e) {}
      }

      await fetchModules(false);
      renderInputs();
      updateHero();
      showToast('Settings saved for Input #' + idx);
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

        const actions = document.createElement('div');
        actions.className = 'card-actions';

        const cfgBtn = document.createElement('button');
        cfgBtn.className = 'card-btn';
        cfgBtn.title = 'Configure Output & Sync Settings';
        cfgBtn.innerHTML = '&#9881;';
        cfgBtn.onclick = (e) => openConfigModal(e, i);

        const editBtn = document.createElement('button');
        editBtn.className = 'card-btn';
        editBtn.title = 'Edit Name';
        editBtn.innerHTML = '&#9998;';
        editBtn.onclick = (e) => editName(e, i);

        actions.appendChild(cfgBtn);
        actions.appendChild(editBtn);

        nameRow.appendChild(labelText);
        nameRow.appendChild(actions);

        // Feature badges for this channel
        const badges = document.createElement('div');
        badges.className = 'card-badges';
        if (moduleData.rgbToComp && moduleData.rgbToComp[i - 1]) {
          badges.innerHTML += '<span class="mod-pill active-y">RGB&rarr;Comp</span>';
        }
        if (moduleData.compToRgb && moduleData.compToRgb[i - 1]) {
          badges.innerHTML += '<span class="mod-pill active-g">Comp&rarr;RGB</span>';
        }
        if (moduleData.rgsb && moduleData.rgsb[i - 1]) {
          badges.innerHTML += '<span class="mod-pill active-r">RGsB</span>';
        }
        if (moduleData.syncBypass && moduleData.syncBypass[i - 1]) {
          badges.innerHTML += '<span class="mod-pill active-s">Raw Sync</span>';
        }

        card.appendChild(top);
        card.appendChild(nameRow);
        if (badges.innerHTML !== '') {
          card.appendChild(badges);
        }
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
        document.getElementById('tog-y-val').innerText = '--';
        document.getElementById('tog-g-val').innerText = '--';
        document.getElementById('tog-r-val').innerText = '--';
        document.getElementById('tog-s-val').innerText = '--';
        document.getElementById('tog-y').className = 'pill-btn';
        document.getElementById('tog-g').className = 'pill-btn';
        document.getElementById('tog-r').className = 'pill-btn';
        document.getElementById('tog-s').className = 'pill-btn';
      } else {
        numElem.innerText = 'Input #' + currentInput;
        nameElem.innerText = getInputName(currentInput);
        subElem.innerText = 'Active routing to SVS Output &bull; CH340 USB CDC 9600 baud';

        const yOn = moduleData.rgbToComp && moduleData.rgbToComp[currentInput - 1];
        const gOn = moduleData.compToRgb && moduleData.compToRgb[currentInput - 1];
        const rOn = moduleData.rgsb && moduleData.rgsb[currentInput - 1];
        const sOn = moduleData.syncBypass && moduleData.syncBypass[currentInput - 1];

        const togY = document.getElementById('tog-y');
        togY.className = 'pill-btn' + (yOn ? ' active' : '');
        document.getElementById('tog-y-val').innerText = yOn ? 'ON' : 'OFF';

        const togG = document.getElementById('tog-g');
        togG.className = 'pill-btn' + (gOn ? ' active' : '');
        document.getElementById('tog-g-val').innerText = gOn ? 'ON' : 'OFF';

        const togR = document.getElementById('tog-r');
        togR.className = 'pill-btn' + (rOn ? ' active' : '');
        document.getElementById('tog-r-val').innerText = rOn ? 'RGsB (SoG)' : 'CSync (Pass)';

        const togS = document.getElementById('tog-s');
        togS.className = 'pill-btn' + (sOn ? ' active' : '');
        document.getElementById('tog-s-val').innerText = sOn ? 'Raw Bypass' : 'Cleaned';
      }
    }

    function updateButtons() {
      const pBtn = document.getElementById('btn-power');
      pBtn.innerText = isPower ? 'Power: ON' : 'Power: Standby';
      pBtn.className = 'btn' + (isPower ? ' btn-active' : '');

      const aBtn = document.getElementById('btn-attract');
      aBtn.className = 'btn' + (isAttract ? ' btn-active' : '');
      aBtn.innerText = isAttract ? 'Attract: ON' : 'Attract: OFF';

      const mBtn = document.getElementById('btn-manual');
      mBtn.className = 'btn' + (isManual ? ' btn-active' : '');
      mBtn.innerText = isManual ? 'Manual: Locked' : 'Manual: Normal';
    }

    // Terminal State & History
    let cmdHistory = [];
    let historyIdx = -1;
    let seenLogKeys = new Set();

    function formatTime() {
      const d = new Date();
      return d.toTimeString().split(' ')[0] + '.' + String(d.getMilliseconds()).padStart(3, '0');
    }

    function escapeHtml(str) {
      return String(str).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
    }

    function appendTerminalLine(dir, text, timeStr) {
      const log = document.getElementById('terminal-log');
      if (!log) return;
      const showTime = document.getElementById('term-timestamps')?.checked ?? true;
      const line = document.createElement('div');
      
      let cls = 'cl-sys';
      let prefix = '* [SYS] ';
      if (dir === 'T') { cls = 'cl-tx'; prefix = '> [TX] '; }
      else if (dir === 'R') { cls = 'cl-rx'; prefix = '< [RX] '; }
      else if (dir === 'E') { cls = 'cl-err'; prefix = '! [ERR] '; }

      const ts = timeStr || formatTime();
      line.innerHTML = (showTime ? '<span class="cl-time">' + ts + '</span>' : '') +
                       '<span class="' + cls + '"><strong>' + prefix + '</strong>' + escapeHtml(text) + '</span>';
      log.appendChild(line);

      if (document.getElementById('term-autoscroll')?.checked ?? true) {
        log.scrollTop = log.scrollHeight;
      }
    }

    function clearTerminal() {
      const log = document.getElementById('terminal-log');
      if (log) log.innerHTML = '';
      appendTerminalLine('S', 'Terminal buffer cleared.');
    }

    async function sendTerminalCommand(e) {
      if (e) e.preventDefault();
      const input = document.getElementById('terminal-input');
      const cmd = input.value.trim();
      if (!cmd) return;
      
      cmdHistory.unshift(cmd);
      if (cmdHistory.length > 50) cmdHistory.pop();
      historyIdx = -1;
      input.value = '';

      appendTerminalLine('T', cmd);

      try {
        const res = await authedFetch('/api/v1/svs/command', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ cmd: cmd })
        });
        if (res.ok) {
          const d = await res.json();
          if (d.response && d.response.trim().length > 0) {
            appendTerminalLine('R', d.response.trim());
          }
          pollStatus();
        } else {
          appendTerminalLine('E', 'Command error: HTTP ' + res.status);
        }
      } catch (err) {
        appendTerminalLine('E', 'Network error: ' + err);
      }
    }

    function quickCmd(cmd) {
      const input = document.getElementById('terminal-input');
      if (input) {
        input.value = cmd;
        sendTerminalCommand();
      }
    }

    document.getElementById('terminal-input')?.addEventListener('keydown', (e) => {
      if (e.key === 'ArrowUp') {
        e.preventDefault();
        if (historyIdx + 1 < cmdHistory.length) {
          historyIdx++;
          e.target.value = cmdHistory[historyIdx];
        }
      } else if (e.key === 'ArrowDown') {
        e.preventDefault();
        if (historyIdx > 0) {
          historyIdx--;
          e.target.value = cmdHistory[historyIdx];
        } else if (historyIdx === 0) {
          historyIdx = -1;
          e.target.value = '';
        }
      }
    });

    // Theme Picker Functions
    function toggleThemeMenu(e) {
      if (e) e.stopPropagation();
      const d = document.getElementById('theme-dropdown');
      if (d) {
        d.style.display = (d.style.display === 'none' || !d.style.display) ? 'block' : 'none';
      }
    }

    function selectTheme(theme) {
      document.documentElement.setAttribute('data-theme', theme);
      localStorage.setItem('svs_theme', theme);
      updateThemeChecks(theme);
      const d = document.getElementById('theme-dropdown');
      if (d) d.style.display = 'none';
      showToast('Theme set to ' + theme.charAt(0).toUpperCase() + theme.slice(1));
    }

    function updateThemeChecks(activeTheme) {
      ['dark', 'glazed', 'light'].forEach(t => {
        const chk = document.getElementById('check-' + t);
        const item = document.getElementById('choice-' + t);
        if (chk) chk.style.visibility = (t === activeTheme) ? 'visible' : 'hidden';
        if (item) item.classList.toggle('selected', t === activeTheme);
      });
    }

    document.addEventListener('click', (e) => {
      const d = document.getElementById('theme-dropdown');
      if (d && !e.target.closest('.theme-menu-container')) {
        d.style.display = 'none';
      }
    });

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

          // Terminal Status & Feed
          const tStatus = document.getElementById('term-status');
          if (tStatus) {
            tStatus.innerText = (data.cdc_connected ? 'CDC Connected' : 'Ready') + ' • 9600 baud 8N1';
          }
          if (data.serial_log && Array.isArray(data.serial_log)) {
            data.serial_log.forEach(entry => {
              const key = entry.t + '_' + entry.d + '_' + entry.m;
              if (!seenLogKeys.has(key)) {
                seenLogKeys.add(key);
                if (seenLogKeys.size > 200) {
                  const it = seenLogKeys.values();
                  seenLogKeys.delete(it.next().value);
                }
                appendTerminalLine(entry.d, entry.m);
              }
            });
          }
        }
      } catch (e) {
        const bConn = document.getElementById('badge-conn');
        bConn.className = 'badge';
        bConn.innerHTML = '<span class="dot"></span> Offline';
      }
    }

    const savedTheme = localStorage.getItem('svs_theme') || 'dark';
    document.documentElement.setAttribute('data-theme', savedTheme);
    updateThemeChecks(savedTheme);
    appendTerminalLine('S', 'Donut Remix Serial Terminal active. Connected to SVS bridge.');

    renderInputs();
    updateHero();
    pollStatus();
    fetchModules(false);
    setInterval(pollStatus, 1500);
  </script>
</body>
</html>)rawliteral";

} // namespace DonutRemix
