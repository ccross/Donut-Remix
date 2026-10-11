#pragma once

namespace DonutRemix {

const char SVS_UI_HTML[] = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Donut Remix | SVS Switch Controller</title>
  <script>
    try {
      const themeCacheKey = 'donutshop.webctl.theme.' + location.host;
      const cachedTheme = localStorage.getItem(themeCacheKey) || localStorage.getItem('donutshop.webctl.theme');
      if((cachedTheme && cachedTheme.startsWith('light')) || location.search.includes('theme=light') || location.search.includes('theme=glazed')){
        document.documentElement.dataset.theme = 'light';
      }
    } catch {}
  </script>
  <style>
  :root{
    --bg:#0b0e14; --bg2:#0f131c; --card:#141926; --card2:#1a2030;
    --line:#232b3d; --line2:#2d3750;
    --txt:#dfe5f1; --dim:#8b94ab; --faint:#5a6379;
    --pink:#ff3d81; --pink2:#ff6da1; --cyan:#3dd9ff; --grn:#3dffa4;
    --amber:#ffc53d; --red:#ff5d5d;
    --mono:ui-monospace,'Cascadia Code','JetBrains Mono',Consolas,monospace;
    --sans:'Segoe UI Variable','Segoe UI',system-ui,-apple-system,sans-serif;
    --rad:12px; --rad-s:8px;
  }
  *{box-sizing:border-box;margin:0;padding:0}
  [hidden]{display:none!important}
  html,body{height:100%}
  body{background:var(--bg);color:var(--txt);font:14px/1.5 var(--sans);overflow-x:hidden}
  ::selection{background:#ff3d8140}
  ::-webkit-scrollbar{width:10px;height:10px}
  ::-webkit-scrollbar-thumb{background:#2a3348;border-radius:6px;border:2px solid var(--bg)}
  ::-webkit-scrollbar-track{background:transparent}
  button{font:inherit;color:inherit;background:none;border:none;cursor:pointer}
  input,select,textarea{font:inherit;color:var(--txt);background:var(--bg2);border:1px solid var(--line);
    border-radius:var(--rad-s);padding:7px 10px;outline:none}
  input:focus,select:focus,textarea:focus{border-color:var(--pink)}
  input::placeholder{color:var(--faint)}
  input[type=checkbox]{accent-color:var(--pink);width:15px;height:15px}

  /* ---------- shell ---------- */
  #app{display:flex;flex-direction:column;min-height:100vh}
  #topbar{display:flex;align-items:center;gap:14px;padding:0 18px;background:var(--bg2);
    border-bottom:1px solid var(--line);height:56px;z-index:5;flex-shrink:0}
  #logo{display:flex;align-items:baseline;gap:8px;user-select:none}
  #logo b{font-size:17px;letter-spacing:.5px}
  #logo b i{color:var(--pink);font-style:normal}
  #logo span{color:var(--faint);font-size:11px;font-family:var(--mono)}
  #content{flex:1;overflow:auto;padding:20px 24px;max-width:1100px;margin:0 auto;width:100%}

  /* ---------- topbar widgets ---------- */
  .chip{display:inline-flex;align-items:center;gap:6px;background:var(--card);border:1px solid var(--line);
    border-radius:999px;padding:3px 11px;font-size:12px;color:var(--dim);white-space:nowrap}
  .chip b{color:var(--txt);font-weight:600;font-family:var(--mono)}
  #dot{width:9px;height:9px;border-radius:50%;background:var(--faint);flex:none;transition:background .2s}
  #dot.ok{background:var(--grn);box-shadow:0 0 8px #3dffa480}
  #dot.busy{background:#ffc53d;box-shadow:0 0 8px #ffc53d80}

  .fast-tip{position:relative}
  .fast-tip::after{
    content:attr(data-tip);
    position:absolute;
    top:calc(100% + 9px);
    left:50%;
    transform:translate(-50%,-2px);
    z-index:9999;
    min-width:max-content;
    max-width:260px;
    padding:7px 9px;
    border:1px solid var(--line2);
    border-radius:7px;
    background:var(--bg2);
    color:var(--txt);
    box-shadow:0 8px 20px #0008;
    font-size:11.5px;
    font-weight:500;
    line-height:1.45;
    white-space:pre-line;
    overflow-wrap:anywhere;
    opacity:0;
    visibility:hidden;
    pointer-events:none;
    transition:opacity .12s,visibility .12s,transform .12s;
  }
  .fast-tip:hover::after,.fast-tip:focus-visible::after{
    opacity:1;visibility:visible;transform:translate(-50%,0);transition-delay:250ms
  }

  /* ---------- buttons ---------- */
  .btn{display:inline-flex;align-items:center;justify-content:center;gap:7px;background:var(--card);
    border:1px solid var(--line2);border-radius:var(--rad-s);padding:7px 15px;font-weight:600;
    transition:background .12s,border-color .12s,transform .05s;white-space:nowrap;color:var(--txt)}
  .btn:hover{background:var(--card2);border-color:#3a4666}
  .btn:active{transform:translateY(1px)}
  .btn:disabled{opacity:.4;cursor:not-allowed;transform:none}
  .btn.pri{background:linear-gradient(135deg,#ff3d81,#d92e6b);border-color:transparent;color:#fff}
  .btn.pri:hover{background:linear-gradient(135deg,#ff5591,#e63a77)}
  .btn.danger{border-color:#ff5d5d55;color:var(--red)}
  .btn.danger:hover{background:#ff5d5d18;border-color:var(--red)}
  .btn.ghost{border-color:transparent;color:var(--dim)}
  .btn.ghost:hover{color:var(--txt);background:var(--card)}
  .btn.sm{padding:4px 10px;font-size:12.5px;font-weight:500}
  .btn.on{border-color:var(--cyan);background:rgba(61,217,255,0.12);color:var(--cyan);font-weight:650}

  /* ---------- theme control ---------- */
  #themecontrol{margin-left:auto;position:relative;display:flex;align-items:center;white-space:nowrap}
  #themebtn{padding:5px 9px;font-size:11.5px}
  #theme-panel{
    position:absolute;right:0;top:calc(100% + 8px);z-index:120;width:max-content;min-width:120px;padding:10px;
    border:1px solid var(--line2);border-radius:10px;background:var(--card2);
    box-shadow:0 16px 42px #0007;color:var(--txt);white-space:normal
  }
  #theme-panel[hidden]{display:none}
  .theme-choice-row{display:flex;flex-direction:column;gap:5px}
  .theme-choice{width:100%;padding:6px 10px;font-size:11.5px;text-align:left}
  .theme-choice.on{border-color:var(--cyan);box-shadow:inset 0 0 0 1px var(--cyan)}

  /* ---------- cards / layout ---------- */
  .grid{display:grid;gap:16px}
  .card{background:var(--card);border:1px solid var(--line);border-radius:var(--rad);padding:16px 18px;min-width:0}
  .card h3{font-size:11.5px;font-weight:700;letter-spacing:1.2px;text-transform:uppercase;
    color:var(--faint);margin-bottom:12px;display:flex;align-items:center;gap:8px}
  .card h3 .spacer{flex:1}
  .hint{color:var(--faint);font-size:11.5px;line-height:1.55}
  .hint b{color:var(--dim)}
  .sep{height:1px;background:var(--line);margin:12px 0}
  .row{display:flex;align-items:center;gap:10px;flex-wrap:wrap}
  .mono{font-family:var(--mono)}
  .pill{font-size:10.5px;font-family:var(--mono);border:1px solid var(--line2);border-radius:5px;
    padding:1px 6px;color:var(--dim)}
  .pill.live{color:var(--grn);border-color:#3dffa444}
  .pill.warn{color:var(--amber);border-color:#ffc53d44}
  .pill.cyan{color:var(--cyan);border-color:rgba(61,217,255,0.44)}

  /* ---------- hero & input cards ---------- */
  .hero-top{display:flex;align-items:center;justify-content:space-between;flex-wrap:wrap;gap:16px}
  .hero-val{font-size:24px;font-weight:750;letter-spacing:-.02em;display:flex;align-items:center;gap:10px;margin:4px 0}
  .hero-val #hero-input-num{color:var(--cyan)}
  .hero-val #hero-input-name{color:var(--txt);font-weight:600;font-size:19px}
  .hero-actions{display:flex;gap:8px;align-items:center}
  .mod-toggles{display:flex;flex-wrap:wrap;gap:8px}
  .pill-dot{width:6px;height:6px;border-radius:50%;background:var(--dim);display:inline-block;margin-right:4px}
  .btn.on .pill-dot{background:var(--cyan);box-shadow:0 0 6px var(--cyan)}

  .inputs-grid{grid-template-columns:repeat(auto-fit,minmax(220px,1fr));margin-top:10px}
  .input-card{
    background:var(--card);border:1.5px solid var(--line);border-radius:var(--rad);
    padding:14px 16px;cursor:pointer;display:flex;flex-direction:column;gap:8px;
    transition:all .15s ease;position:relative;overflow:hidden;user-select:none;
  }
  .input-card:hover{background:var(--card2);border-color:var(--line2);transform:translateY(-2px)}
  .input-card.active{
    border-color:var(--cyan);background:rgba(61,217,255,0.06);
    box-shadow:0 0 16px rgba(61,217,255,0.18)
  }
  .input-card.active::before{content:"";position:absolute;top:0;left:0;right:0;height:3px;background:var(--cyan)}
  .input-card-head{display:flex;align-items:center;justify-content:space-between}
  .input-card-num{font-family:var(--mono);font-size:15px;font-weight:750;color:var(--dim)}
  .input-card.active .input-card-num{color:var(--cyan)}
  .input-card-title{font-size:14px;font-weight:600;color:var(--txt);display:flex;align-items:center;justify-content:space-between}
  .input-card-badges{display:flex;flex-wrap:wrap;gap:4px;margin-top:2px}
  .mod-badge{font-size:9.5px;font-family:var(--mono);padding:1px 5px;border-radius:4px;border:1px solid var(--line2);color:var(--dim)}
  .mod-badge.y{color:var(--amber);border-color:rgba(255,197,61,0.4);background:rgba(255,197,61,0.08)}
  .mod-badge.g{color:var(--grn);border-color:rgba(61,255,164,0.4);background:rgba(61,255,164,0.08)}
  .mod-badge.r{color:var(--cyan);border-color:rgba(61,217,255,0.4);background:rgba(61,217,255,0.08)}
  .mod-badge.s{color:var(--pink);border-color:rgba(255,61,129,0.4);background:rgba(255,61,129,0.08)}

  /* ---------- console ---------- */
  #conlog{font-family:var(--mono);font-size:12.3px;line-height:1.62;background:var(--bg2);
    border:1px solid var(--line);border-radius:var(--rad);padding:12px 16px;height:240px;
    overflow:auto;white-space:pre-wrap;word-break:break-all}
  .cl-tx{color:var(--cyan)} .cl-rx{color:var(--txt)} .cl-mcu{color:var(--amber)}
  .cl-sys{color:var(--faint);font-style:italic} .cl-err{color:var(--red)}
  .cl-wire{color:#6a7590} .cl-t{color:#4a5468;user-select:none;margin-right:6px}
  #conform{display:flex;gap:10px;margin-top:12px}
  #coninput{flex:1;font-family:var(--mono)}
  .qchips{display:flex;gap:6px;flex-wrap:wrap;margin-top:10px}
  .qchip{font-family:var(--mono);font-size:11.5px;border:1px solid var(--line);border-radius:999px;
    padding:3px 11px;color:var(--dim)}
  .qchip:hover{color:var(--txt);border-color:var(--line2);background:var(--card2)}

  /* ---------- toasts / modal ---------- */
  #toasts{position:fixed;right:20px;bottom:20px;display:flex;flex-direction:column;gap:8px;z-index:60}
  .toast{background:var(--card2);border:1px solid var(--line2);border-left:3px solid var(--cyan);
    border-radius:var(--rad-s);padding:10px 15px;max-width:420px;font-size:13px;
    box-shadow:0 8px 30px #0009;animation:tin .18s ease-out}
  .toast.err{border-left-color:var(--red)} .toast.ok{border-left-color:var(--grn)}
  .toast.warn{border-left-color:var(--amber)}
  @keyframes tin{from{transform:translateY(8px);opacity:0}}
  #modalwrap{position:fixed;inset:0;background:#000a;display:none;align-items:center;
    justify-content:center;z-index:90;backdrop-filter:blur(2px)}
  #modalwrap.on{display:flex}
  #modal{background:var(--card);border:1px solid var(--line2);border-radius:var(--rad);
    padding:22px 24px;width:min(480px,90vw);box-shadow:0 20px 60px #000c}
  #modal h4{font-size:15px;margin-bottom:6px}
  #modal p{color:var(--dim);font-size:13px;margin-bottom:12px;line-height:1.55}
  #modalbtns{display:flex;justify-content:flex-end;gap:10px;margin-top:18px}

  /* ---------- GLAZED LIGHT THEME ---------- */
  html[data-theme="light"]{
    color-scheme:light;
    --bg:#e0e6ea;
    --bg2:rgba(233,237,239,.79);
    --card:rgba(255,255,255,.62);
    --card2:rgba(246,249,251,.78);
    --line:rgba(112,125,134,.22);
    --line2:rgba(100,114,124,.34);
    --txt:#111318;
    --dim:#555d63;
    --faint:#7e878d;
    --pink:#d93672;
    --pink2:#e45c8d;
    --cyan:#087f9d;
    --grn:#118b5b;
    --amber:#a56f00;
    --red:#d64545;

    --glz-body-a:#dde6e9;
    --glz-body-b:#eaecef;
    --glz-body-c:#dee5eb;
    --glz-topbar-bg:rgba(249,251,252,.70);
    --glz-topbar-border:rgba(255,255,255,.78);
    --glz-card-border:rgba(255,255,255,.78);
    --glz-card-inset:rgba(255,255,255,.58);
    --glz-chip-bg:rgba(255,255,255,.48);
    --glz-chip-border:rgba(255,255,255,.72);
    --glz-input-bg:rgba(255,255,255,.58);
    --glz-popup-bg:rgba(250,252,253,.82);
    --glz-popup-border:rgba(255,255,255,.82);
    --glz-modal-bg:rgba(255,255,255,.78);
    --glz-modal-border:rgba(255,255,255,.86);
  }
  html[data-theme="light"] body{
    background:
      radial-gradient(circle at 12% 10%,rgba(118,205,224,.38),transparent 31%),
      radial-gradient(circle at 86% 14%,rgba(222,176,224,.34),transparent 29%),
      radial-gradient(circle at 32% 92%,rgba(153,222,190,.28),transparent 30%),
      radial-gradient(circle at 88% 88%,rgba(255,175,203,.25),transparent 31%),
      linear-gradient(135deg,var(--glz-body-a) 0%,var(--glz-body-b) 48%,var(--glz-body-c) 100%);
    background-attachment:fixed;
  }
  html[data-theme="light"] #topbar,
  html[data-theme="light"] .card,
  html[data-theme="light"] .chip,
  html[data-theme="light"] .toast,
  html[data-theme="light"] #theme-panel,
  html[data-theme="light"] #modal{
    -webkit-backdrop-filter:blur(16px) saturate(135%);
    backdrop-filter:blur(16px) saturate(135%);
  }
  html[data-theme="light"] #topbar{
    background:var(--glz-topbar-bg);
    border-bottom-color:var(--glz-topbar-border);
    box-shadow:0 5px 20px rgba(56,72,82,.08)
  }
  html[data-theme="light"] #dot.busy{
    background:#d29a1f;
    box-shadow:0 0 8px #d29a1f80;
  }
  html[data-theme="light"] .card{
    border-color:var(--glz-card-border);
    box-shadow:0 8px 24px rgba(56,72,82,.085),inset 0 1px 0 var(--glz-card-inset)
  }
  html[data-theme="light"] .card h3::before{
    content:"";width:13px;height:4px;flex:none;border-radius:999px;
    background:linear-gradient(90deg,#ff6da1,#e45c8d);box-shadow:0 1px 3px rgba(17,19,24,.08)
  }
  html[data-theme="light"] .card:nth-of-type(2) h3::before{background:linear-gradient(90deg,#3dd9ff,#58b8e8)}
  html[data-theme="light"] .card:nth-of-type(3) h3::before{background:linear-gradient(90deg,#5ed6a5,#8ad08f)}
  html[data-theme="light"] .card:nth-of-type(4) h3::before{background:linear-gradient(90deg,#ffae57,#ff7f93)}
  html[data-theme="light"] .chip{background:var(--glz-chip-bg);border-color:var(--glz-chip-border)}
  html[data-theme="light"] input,
  html[data-theme="light"] select,
  html[data-theme="light"] textarea{
    background:var(--glz-input-bg);border-color:rgba(112,125,134,.26)
  }
  html[data-theme="light"] #theme-panel{
    background:var(--glz-popup-bg);border-color:var(--glz-popup-border);box-shadow:0 16px 42px rgba(56,72,82,.14)
  }
  html[data-theme="light"] #modalwrap{background:rgba(40,48,54,.28)}
  html[data-theme="light"] #modal{background:var(--glz-modal-bg);border-color:var(--glz-modal-border);box-shadow:0 20px 60px rgba(56,72,82,.22)}
  html[data-theme="light"] .cl-t{color:#8a949a}
  </style>
</head>
<body>
  <div id="app">
    <header id="topbar">
      <div id="logo"><b>Donut<i>·</i>Remix</b><span>SVS</span></div>
      <span id="dot" class="fast-tip" tabindex="0" data-tip="Gray = disconnected&#10;Green = connected / idle&#10;Orange = command busy" aria-label="Status indicator"></span>
      <span class="chip" id="chip-fw">fw <b>v1.20</b></span>
      <span class="chip" id="chip-inputs">inputs <b>6</b></span>
      <span class="chip fast-tip" id="chip-peer" data-tip="Open linked RT4K WebCtl" aria-label="Linked RT4K Node"><a href="http://192.168.1.80/" target="_blank" style="color:inherit;text-decoration:none;">peer <b>RT4K</b></a></span>
      <div id="themecontrol">
        <button class="btn sm" id="themebtn" type="button" title="Theme" aria-haspopup="true" aria-expanded="false">Theme</button>
        <div id="theme-panel" hidden>
          <div class="theme-choice-row">
            <button class="btn sm theme-choice" id="theme-dark" type="button">Dark</button>
            <button class="btn sm theme-choice" id="theme-glazed" type="button">Glazed</button>
          </div>
        </div>
      </div>
    </header>

    <div id="content">
      <!-- ===== HERO ACTIVE ROUTE ===== -->
      <div class="card hero-card">
        <div class="hero-top">
          <div>
            <h3>Active Video Routing <span class="spacer"></span> <span class="pill cyan" id="hero-pill">STANDBY</span></h3>
            <div class="hero-val">
              <span id="hero-input-num" class="mono">Input --</span>
              <span id="hero-input-name">(--)</span>
            </div>
            <div class="hint" id="hero-sub">Polling switch hardware over USB CDC...</div>
          </div>
          <div class="hero-actions">
            <button class="btn pri sm" onclick="seekInput('up')">Seek Next ▲</button>
            <button class="btn sm" onclick="seekInput('down')">Seek Prev ▼</button>
          </div>
        </div>
        <div class="sep"></div>
        <div class="mod-toggles">
          <button class="btn sm" id="tog-y" onclick="toggleActiveModule('rgb_to_comp')">
            <span class="pill-dot"></span> RGB &rarr; Comp: <b id="tog-y-val" class="mono">--</b>
          </button>
          <button class="btn sm" id="tog-g" onclick="toggleActiveModule('comp_to_rgb')">
            <span class="pill-dot"></span> Comp &rarr; RGB: <b id="tog-g-val" class="mono">--</b>
          </button>
          <button class="btn sm" id="tog-r" onclick="toggleActiveModule('rgsb')">
            <span class="pill-dot"></span> Sync: <b id="tog-r-val" class="mono">--</b>
          </button>
          <button class="btn sm" id="tog-s" onclick="toggleActiveModule('sync_bypass')">
            <span class="pill-dot"></span> Sync Bypass: <b id="tog-s-val" class="mono">--</b>
          </button>
        </div>
      </div>

      <!-- ===== INPUTS GRID ===== -->
      <div class="card" style="margin-top:16px;">
        <h3>Switch Channels (1 – 6) <span class="spacer"></span> <span class="hint">Click channel to route video &bull; ⚙ for output transcode &amp; sync settings</span></h3>
        <div class="grid inputs-grid" id="inputs-container">
          <!-- Rendered dynamically -->
        </div>
      </div>

      <!-- ===== CONTROLS PANEL ===== -->
      <div class="card" style="margin-top:16px;">
        <h3>Switch Controls &amp; Settings <span class="spacer"></span></h3>
        <div class="row">
          <button class="btn sm" onclick="cycleInput('up')">Input Up (+1)</button>
          <button class="btn sm" onclick="cycleInput('down')">Input Down (-1)</button>
          <button id="btn-power" class="btn sm" onclick="togglePower()">Power: Standby</button>
          <button id="btn-attract" class="btn sm" onclick="toggleAttract()">Attract Mode</button>
          <button id="btn-manual" class="btn sm" onclick="toggleManual()">Manual Lock</button>
          <button class="btn sm" onclick="fetchModules(true)">Refresh Modules</button>
          <button class="btn sm danger" onclick="switchInput(0)" style="margin-left:auto;">Input 0 (Standby)</button>
        </div>
      </div>

      <!-- ===== CONSOLE ===== -->
      <div class="card" style="margin-top:16px;">
        <h3>SVS Serial Console
          <span class="spacer"></span>
          <span class="pill" id="term-status">CH340 9600 8N1</span>
          <label for="term-autoscroll" class="hint" style="display:flex;align-items:center;gap:4px;cursor:pointer;margin:0 8px 0 0;">
            <input type="checkbox" id="term-autoscroll" checked> autoscroll
          </label>
          <button class="btn sm ghost" onclick="clearTerminal()">clear</button>
        </h3>
        <div id="conlog"></div>
        <form id="conform" onsubmit="sendTerminalCommand(event)">
          <input id="coninput" placeholder="command… (e.g. SVS_Change_Input_1, Seek, R550, Y1) (↑ history)" autocomplete="off" spellcheck="false" aria-label="Console command" />
          <button class="btn pri sm" type="submit">Send</button>
        </form>
        <div class="qchips">
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_1')">Input 1</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_2')">Input 2</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_3')">Input 3</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_4')">Input 4</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_5')">Input 5</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Change_Input_6')">Input 6</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Input_Seek_Up')">Seek ▲</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Input_Seek_Down')">Seek ▼</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Attract_ON')">Attract ON</button>
          <button type="button" class="qchip" onclick="quickCmd('SVS_Attract_OFF')">Attract OFF</button>
          <button type="button" class="qchip" onclick="quickCmd('R550')">Read 550 (RGsB)</button>
          <button type="button" class="qchip" onclick="quickCmd('R590')">Read 590 (Sync)</button>
          <button type="button" class="qchip" onclick="quickCmd('Y1')">Y1 (RGB-Comp 1)</button>
          <button type="button" class="qchip" onclick="quickCmd('G1')">G1 (Comp-RGB 1)</button>
        </div>
      </div>

      <div style="margin-top:24px;text-align:center;color:var(--faint);font-size:11.5px;padding-bottom:16px;">
        Donut Remix &bull; Scalable Video Switch Node &bull; <a href="http://192.168.1.80/" target="_blank" style="color:var(--dim);text-decoration:none;border-bottom:1px dotted var(--faint);">Open RT4K WebCtl</a>
      </div>
    </div>
  </div>

  <div id="toasts"></div>

  <div id="modalwrap">
    <div id="modal">
      <h4 id="modal-title">Channel Configuration</h4>
      <p id="modal-sub">Adjust transcode and sync routing for this video input.</p>
      <div style="margin:12px 0;">
        <label class="hint" style="display:block;margin-bottom:4px;">Channel Friendly Name:</label>
        <input type="text" id="modal-name-input" style="width:100%;" placeholder="e.g. SNES" />
      </div>
      <div class="sep"></div>
      <div style="display:flex;flex-direction:column;gap:12px;">
        <label style="display:flex;align-items:center;justify-content:space-between;cursor:pointer;">
          <span><b>RGB to YPbPr (Component)</b><div class="hint">Transcode RGB signal to Component output</div></span>
          <input type="checkbox" id="modal-tog-y" />
        </label>
        <label style="display:flex;align-items:center;justify-content:space-between;cursor:pointer;">
          <span><b>YPbPr to RGB</b><div class="hint">Transcode Component signal to RGB output</div></span>
          <input type="checkbox" id="modal-tog-g" />
        </label>
        <label style="display:flex;align-items:center;justify-content:space-between;cursor:pointer;">
          <span><b>RGBS to RGsB (Sync-on-Green)</b><div class="hint">Route sync onto green channel (for PVMs / SoG)</div></span>
          <input type="checkbox" id="modal-tog-r" />
        </label>
        <label style="display:flex;align-items:center;justify-content:space-between;cursor:pointer;">
          <span><b>Sync Bypass (Raw CSync)</b><div class="hint">Bypass sync cleaner for raw arcade / TTL sync</div></span>
          <input type="checkbox" id="modal-tog-s" />
        </label>
      </div>
      <div id="modalbtns">
        <button class="btn sm" onclick="closeModal()">Cancel</button>
        <button class="btn pri sm" onclick="saveModalConfig()">Save Settings</button>
      </div>
    </div>
  </div>

  <script>
    const $ = id => document.getElementById(id);

    /* ---- WebCtl theme ---- */
    const WEBCTL_THEME_LEGACY_KEY = 'donutshop.webctl.theme';
    const WEBCTL_THEME_CACHE_KEY = 'donutshop.webctl.theme.' + location.host;
    const readWebCtlThemeCache = () => {
      try {
        if(location.search.includes('theme=light') || location.search.includes('theme=glazed')) return 'light';
        if(location.search.includes('theme=dark')) return 'dark';
        const cached = localStorage.getItem(WEBCTL_THEME_CACHE_KEY) ||
                       localStorage.getItem(WEBCTL_THEME_LEGACY_KEY) || 'dark';
        return cached.startsWith('light') ? 'light' : 'dark';
      } catch { return 'dark'; }
    };
    const writeWebCtlThemeCache = theme => {
      try {
        localStorage.setItem(WEBCTL_THEME_CACHE_KEY, theme === 'light' ? 'light' : 'dark');
        localStorage.removeItem(WEBCTL_THEME_LEGACY_KEY);
      } catch {}
    };

    let webctlTheme = readWebCtlThemeCache();
    let webctlConfirmedTheme = webctlTheme;

    function updateThemeControls(){
      const dark = $('theme-dark');
      const glazed = $('theme-glazed');
      if(dark) dark.classList.toggle('on', webctlTheme === 'dark');
      if(glazed) glazed.classList.toggle('on', webctlTheme === 'light');
    }

    function applyWebCtlAppearance(theme){
      webctlTheme = theme === 'light' ? 'light' : 'dark';
      document.documentElement.dataset.theme = webctlTheme;
      updateThemeControls();
    }

    async function syncWebCtlThemeFromNano(){
      try {
        const response = await fetch('/webctl-theme', {cache:'no-store'});
        if(!response.ok) return;
        const state = await response.json();
        const theme = state.theme === 'light' ? 'light' : 'dark';
        applyWebCtlAppearance(theme);
        writeWebCtlThemeCache(theme);
      } catch (e) {}
    }

    async function saveWebCtlAppearance(theme, previous){
      try {
        const response = await fetch('/webctl-theme', {
          method: 'POST',
          headers: {'Content-Type': 'application/x-www-form-urlencoded'},
          body: 'theme=' + encodeURIComponent(theme)
        });
        if(response.ok){
          const state = await response.json();
          const confirmed = state.theme === 'light' ? 'light' : 'dark';
          applyWebCtlAppearance(confirmed);
          writeWebCtlThemeCache(confirmed);
        }
      } catch(e) {
        console.warn('Theme save failed', e);
      }
    }

    applyWebCtlAppearance(webctlTheme);
    syncWebCtlThemeFromNano();

    const themeButton = $('themebtn');
    const themePanel = $('theme-panel');

    themeButton?.addEventListener('click', event => {
      event.stopPropagation();
      if(!themePanel) return;
      themePanel.hidden = !themePanel.hidden;
      themeButton.setAttribute('aria-expanded', themePanel.hidden ? 'false' : 'true');
    });
    themePanel?.addEventListener('click', event => event.stopPropagation());
    document.addEventListener('click', () => {
      if(themePanel) themePanel.hidden = true;
      if(themeButton) themeButton.setAttribute('aria-expanded','false');
    });

    $('theme-dark')?.addEventListener('click', () => {
      if(webctlTheme === 'dark') return;
      const previous = webctlConfirmedTheme;
      applyWebCtlAppearance('dark');
      writeWebCtlThemeCache('dark');
      saveWebCtlAppearance('dark', previous);
      if(themePanel) themePanel.hidden = true;
    });
    $('theme-glazed')?.addEventListener('click', () => {
      if(webctlTheme === 'light') return;
      const previous = webctlConfirmedTheme;
      applyWebCtlAppearance('light');
      writeWebCtlThemeCache('light');
      saveWebCtlAppearance('light', previous);
      if(themePanel) themePanel.hidden = true;
    });

    /* ---- SVS State & Control ---- */
    let sessionToken = localStorage.getItem('donut_token') || '';
    let currentInput = 0;
    let totalInputs = 6;
    let isConnected = false;
    let isPower = true;
    let isAttract = false;
    let isManual = false;

    let serverInputNames = {};
    let pollCount = 0;

    async function loadServerInputNames() {
      try {
        const res = await fetch('/api/v1/svs/names', { cache: 'no-store' });
        if (res.ok) {
          const data = await res.json();
          if (data && data.names) {
            serverInputNames = data.names;
            renderInputs();
            updateHero();
          }
        }
      } catch (e) {}
    }

    function getInputName(idx) {
      if (idx === 0) return 'Standby / None';
      if (serverInputNames[idx] && String(serverInputNames[idx]).trim() !== '') {
        return serverInputNames[idx];
      }
      const saved = localStorage.getItem('svs_input_name_' + idx);
      if (saved) return saved;
      const defaults = { 1: 'SNES', 2: 'Genesis', 3: 'PS1', 4: 'N64', 5: 'Saturn', 6: 'Dreamcast' };
      return defaults[idx] || ('Input ' + idx);
    }

    async function setInputName(idx, name) {
      serverInputNames[idx] = name;
      localStorage.setItem('svs_input_name_' + idx, name);
      renderInputs();
      updateHero();
      try {
        await apiFetch('/api/v1/svs/names', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ input: idx, name: name })
        });
        toast('Saved name for Input ' + idx, 'ok');
      } catch (e) {}
    }

    function editName(e, idx) {
      e.stopPropagation();
      const curr = getInputName(idx);
      const val = prompt('Enter friendly name for Input ' + idx + ':', curr);
      if (val !== null && val.trim() !== '') {
        setInputName(idx, val.trim());
      }
    }

    function toast(msg, type = 'ok') {
      const c = $('toasts');
      if(!c) return;
      const t = document.createElement('div');
      t.className = 'toast ' + type;
      t.innerText = msg;
      c.appendChild(t);
      setTimeout(() => t.remove(), 2500);
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
      $('dot').className = 'busy';
      try {
        const res = await authedFetch('/api/v1/svs/input', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ input: num })
        });
        if (res.ok) {
          toast('Switched to ' + getInputName(num));
        } else {
          currentInput = prevInput;
          renderInputs();
          updateHero();
          toast('Error switching input (' + res.status + ')', 'err');
        }
      } catch (e) {
        currentInput = prevInput;
        renderInputs();
        updateHero();
        toast('Failed to switch input', 'err');
      }
    }

    async function seekInput(dir) {
      $('dot').className = 'busy';
      try {
        const res = await authedFetch('/api/v1/svs/seek', {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify({ dir: dir })
        });
        if (res.ok) {
          toast('Seeking active signal ' + dir + '…');
          pollStatus();
        } else {
          toast('Seek failed (' + res.status + ')', 'err');
        }
      } catch (e) {
        toast('Seek error', 'err');
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
          toast(next ? 'Power ON' : 'Standby Mode');
        } else {
          isPower = !next;
          updateButtons();
          toast('Power toggle failed', 'err');
        }
      } catch (e) {
        isPower = !next;
        updateButtons();
        toast('Power toggle error', 'err');
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
          toast('Attract mode ' + (next ? 'ON' : 'OFF'));
        } else {
          isAttract = !next;
          updateButtons();
          toast('Attract toggle failed', 'err');
        }
      } catch (e) {
        isAttract = !next;
        updateButtons();
        toast('Attract error', 'err');
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
          toast('Manual mode ' + (next ? 'ON' : 'OFF'));
        } else {
          isManual = !next;
          updateButtons();
          toast('Manual toggle failed', 'err');
        }
      } catch (e) {
        isManual = !next;
        updateButtons();
        toast('Manual error', 'err');
      }
    }

    let moduleData = { rgbToComp: [], compToRgb: [], rgsb: [], syncBypass: [] };
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
          if (force) toast('Refreshed module settings from hardware');
        }
      } catch (e) {}
    }

    async function toggleActiveModule(type) {
      if (currentInput <= 0) {
        toast('Select an active input first', 'warn');
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
          toast('Updated ' + type + ' on Input #' + currentInput);
        } else {
          toast('Failed to toggle ' + type, 'err');
        }
      } catch (e) {
        toast('Error toggling module', 'err');
      }
    }

    function openConfigModal(e, idx) {
      if (e) e.stopPropagation();
      modalInputIdx = idx;
      $('modal-title').innerText = 'Input #' + idx + ' Settings (' + getInputName(idx) + ')';
      $('modal-name-input').value = getInputName(idx);
      $('modal-tog-y').checked = !!moduleData.rgbToComp[idx - 1];
      $('modal-tog-g').checked = !!moduleData.compToRgb[idx - 1];
      $('modal-tog-r').checked = !!moduleData.rgsb[idx - 1];
      $('modal-tog-s').checked = !!moduleData.syncBypass[idx - 1];
      $('modalwrap').className = 'on';
    }

    function closeModal(e) {
      if (e && e.target !== $('modalwrap') && e.type === 'click' && !e.target.classList.contains('btn')) return;
      $('modalwrap').className = '';
    }

    async function saveModalConfig() {
      const idx = modalInputIdx;
      const newName = $('modal-name-input').value.trim();
      if (newName) setInputName(idx, newName);

      const updates = [
        { type: 'rgb_to_comp', enable: $('modal-tog-y').checked },
        { type: 'comp_to_rgb', enable: $('modal-tog-g').checked },
        { type: 'rgsb', enable: $('modal-tog-r').checked },
        { type: 'sync_bypass', enable: $('modal-tog-s').checked }
      ];

      closeModal();
      toast('Saving channel settings…');

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
      toast('Settings saved for Input #' + idx);
    }

    function renderInputs() {
      const c = $('inputs-container');
      if (!c) return;
      c.innerHTML = '';
      for (let i = 1; i <= totalInputs; i++) {
        const card = document.createElement('div');
        card.className = 'input-card' + (currentInput === i ? ' active' : '');
        card.onclick = () => switchInput(i);

        const head = document.createElement('div');
        head.className = 'input-card-head';

        const num = document.createElement('div');
        num.className = 'input-card-num';
        num.innerText = '#' + i;

        const pill = document.createElement('span');
        pill.className = 'pill' + (currentInput === i ? ' live' : '');
        pill.innerText = (currentInput === i ? 'ACTIVE' : 'READY');

        head.appendChild(num);
        head.appendChild(pill);

        const titleRow = document.createElement('div');
        titleRow.className = 'input-card-title';

        const nameSpan = document.createElement('span');
        nameSpan.innerText = getInputName(i);

        const actionGroup = document.createElement('div');
        actionGroup.style.display = 'flex';
        actionGroup.style.gap = '4px';

        const cfgBtn = document.createElement('button');
        cfgBtn.className = 'btn sm ghost';
        cfgBtn.style.padding = '2px 6px';
        cfgBtn.title = 'Configure Output & Sync Settings';
        cfgBtn.innerText = '⚙';
        cfgBtn.onclick = (e) => openConfigModal(e, i);

        const editBtn = document.createElement('button');
        editBtn.className = 'btn sm ghost';
        editBtn.style.padding = '2px 6px';
        editBtn.title = 'Edit Name';
        editBtn.innerText = '✎';
        editBtn.onclick = (e) => editName(e, i);

        actionGroup.appendChild(cfgBtn);
        actionGroup.appendChild(editBtn);

        titleRow.appendChild(nameSpan);
        titleRow.appendChild(actionGroup);

        const badges = document.createElement('div');
        badges.className = 'input-card-badges';
        if (moduleData.rgbToComp && moduleData.rgbToComp[i - 1]) {
          badges.innerHTML += '<span class="mod-badge y">RGB→Comp</span>';
        }
        if (moduleData.compToRgb && moduleData.compToRgb[i - 1]) {
          badges.innerHTML += '<span class="mod-badge g">Comp→RGB</span>';
        }
        if (moduleData.rgsb && moduleData.rgsb[i - 1]) {
          badges.innerHTML += '<span class="mod-badge r">RGsB</span>';
        }
        if (moduleData.syncBypass && moduleData.syncBypass[i - 1]) {
          badges.innerHTML += '<span class="mod-badge s">Raw Sync</span>';
        }

        card.appendChild(head);
        card.appendChild(titleRow);
        if (badges.innerHTML !== '') card.appendChild(badges);
        c.appendChild(card);
      }
    }

    function updateHero() {
      const numElem = $('hero-input-num');
      const nameElem = $('hero-input-name');
      const subElem = $('hero-sub');
      const pillElem = $('hero-pill');

      if (currentInput === 0) {
        numElem.innerText = 'Standby';
        nameElem.innerText = '(No Input)';
        subElem.innerText = 'Switch in standby mode. Click a channel card to route video.';
        if(pillElem){ pillElem.className = 'pill warn'; pillElem.innerText = 'STANDBY'; }

        $('tog-y-val').innerText = '--';
        $('tog-g-val').innerText = '--';
        $('tog-r-val').innerText = '--';
        $('tog-s-val').innerText = '--';
        $('tog-y').className = 'btn sm';
        $('tog-g').className = 'btn sm';
        $('tog-r').className = 'btn sm';
        $('tog-s').className = 'btn sm';
      } else {
        numElem.innerText = 'Input #' + currentInput;
        nameElem.innerText = getInputName(currentInput);
        subElem.innerText = 'Active routing to SVS Output &bull; CH340 USB CDC 9600 baud';
        if(pillElem){ pillElem.className = 'pill live'; pillElem.innerText = 'LIVE ROUTE'; }

        const yOn = moduleData.rgbToComp && moduleData.rgbToComp[currentInput - 1];
        const gOn = moduleData.compToRgb && moduleData.compToRgb[currentInput - 1];
        const rOn = moduleData.rgsb && moduleData.rgsb[currentInput - 1];
        const sOn = moduleData.syncBypass && moduleData.syncBypass[currentInput - 1];

        $('tog-y').className = 'btn sm' + (yOn ? ' on' : '');
        $('tog-y-val').innerText = yOn ? 'ON' : 'OFF';

        $('tog-g').className = 'btn sm' + (gOn ? ' on' : '');
        $('tog-g-val').innerText = gOn ? 'ON' : 'OFF';

        $('tog-r').className = 'btn sm' + (rOn ? ' on' : '');
        $('tog-r-val').innerText = rOn ? 'RGsB (SoG)' : 'CSync (Pass)';

        $('tog-s').className = 'btn sm' + (sOn ? ' on' : '');
        $('tog-s-val').innerText = sOn ? 'Raw Bypass' : 'Cleaned';
      }
    }

    function updateButtons() {
      const pBtn = $('btn-power');
      pBtn.innerText = isPower ? 'Power: ON' : 'Power: Standby';
      pBtn.className = 'btn sm' + (isPower ? ' on' : '');

      const aBtn = $('btn-attract');
      aBtn.className = 'btn sm' + (isAttract ? ' on' : '');
      aBtn.innerText = isAttract ? 'Attract: ON' : 'Attract: OFF';

      const mBtn = $('btn-manual');
      mBtn.className = 'btn sm' + (isManual ? ' on' : '');
      mBtn.innerText = isManual ? 'Manual: Locked' : 'Manual: Normal';
    }

    /* ---- Serial Console ---- */
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
      const log = $('conlog');
      if (!log) return;
      const line = document.createElement('div');
      
      let cls = 'cl-sys';
      let prefix = '* [SYS] ';
      if (dir === 'T') { cls = 'cl-tx'; prefix = '> [TX] '; }
      else if (dir === 'R') { cls = 'cl-rx'; prefix = '< [RX] '; }
      else if (dir === 'E') { cls = 'cl-err'; prefix = '! [ERR] '; }

      const ts = timeStr || formatTime();
      line.innerHTML = '<span class="cl-t">' + ts + '</span><span class="' + cls + '"><strong>' + prefix + '</strong>' + escapeHtml(text) + '</span>';
      log.appendChild(line);

      if ($('term-autoscroll')?.checked ?? true) {
        log.scrollTop = log.scrollHeight;
      }
    }

    function clearTerminal() {
      const log = $('conlog');
      if (log) log.innerHTML = '';
      appendTerminalLine('S', 'Console buffer cleared.');
    }

    async function sendTerminalCommand(e) {
      if (e) e.preventDefault();
      const input = $('coninput');
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
      const input = $('coninput');
      if (input) {
        input.value = cmd;
        sendTerminalCommand();
      }
    }

    $('coninput')?.addEventListener('keydown', (e) => {
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

    /* ---- Polling & Hardware Telemetry ---- */
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

          // Topbar Badges
          const dot = $('dot');
          if (dot) dot.className = data.connected ? 'ok' : '';
          $('chip-fw').innerHTML = 'fw <b>v' + (data.firmware || '1.20') + '</b>';
          $('chip-inputs').innerHTML = 'inputs <b>' + totalInputs + '</b>';

          // Terminal Status & Feed
          const tStatus = $('term-status');
          if (tStatus) {
            tStatus.innerText = (data.cdc_connected ? 'CDC LIVE' : 'READY') + ' 9600 8N1';
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
          pollCount++;
          if (pollCount % 10 === 0) {
            loadServerInputNames();
          }
        }
      } catch (e) {
        const dot = $('dot');
        if (dot) dot.className = '';
      }
    }

    renderInputs();
    updateHero();
    loadServerInputNames();
    pollStatus();
    fetchModules(false);
    setInterval(pollStatus, 1500);
    appendTerminalLine('S', 'Donut Remix Serial Console active. Attached to SVS bridge.');
  </script>
</body>
</html>)rawliteral";

} // namespace DonutRemix
