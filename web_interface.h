// web_interface.h - the controller page served from flash (PROGMEM).
// Include this file from the .ino only.
#pragma once

#include <Arduino.h>

static const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no,viewport-fit=cover">
<meta name="theme-color" content="#0f1115">
<title>Robot Car</title>
<style>
:root{--bg:#0f1115;--panel:#171a21;--line:#2a2f3a;--text:#e6e8ec;--dim:#8b93a1;--accent:#3ea6ff;--ok:#3ecf8e;--bad:#ff5c5c;--js:min(70vw,40vh,280px)}
*{box-sizing:border-box;-webkit-tap-highlight-color:transparent}
[hidden]{display:none!important}
html,body{margin:0;height:100%;background:var(--bg);color:var(--text);font:16px/1.4 system-ui,-apple-system,"Segoe UI",Roboto,sans-serif;overscroll-behavior:none}
body{display:flex;flex-direction:column;padding:env(safe-area-inset-top) env(safe-area-inset-right) env(safe-area-inset-bottom) env(safe-area-inset-left);-webkit-user-select:none;user-select:none;-webkit-touch-callout:none}
header{display:flex;justify-content:space-between;align-items:center;padding:10px 14px;border-bottom:1px solid var(--line)}
h1{margin:0;font-size:14px;font-weight:600;letter-spacing:.08em;text-transform:uppercase}
.status{display:flex;align-items:center;gap:8px;font-size:13px;color:var(--dim)}
.dot{width:10px;height:10px;border-radius:50%;background:var(--bad)}
.dot.ok{background:var(--ok)}
main{flex:1;min-height:0;overflow:auto;display:grid;gap:12px;padding:12px;grid-template-columns:1fr;grid-template-areas:"tele" "pad" "speed" "stop" "diag";grid-template-rows:auto 1fr auto auto auto}
.panel{background:var(--panel);border:1px solid var(--line);border-radius:6px;padding:10px 12px}
#tele{grid-area:tele;display:flex;justify-content:space-between;font-size:13px;color:var(--dim);font-variant-numeric:tabular-nums}
#tele b{color:var(--text);font-weight:600}
#pad{grid-area:pad;display:flex;flex-direction:column;align-items:center;justify-content:center;gap:14px;min-height:0}
.seg{display:flex;border:1px solid var(--line);border-radius:6px;overflow:hidden}
.seg button{background:transparent;color:var(--dim);border:0;padding:8px 18px;font:inherit;font-size:14px}
.seg button.on{background:var(--accent);color:#06121c;font-weight:600}
#joy{position:relative;width:var(--js);height:var(--js);border:2px solid var(--line);border-radius:50%;background:var(--panel);touch-action:none}
#joy::before,#joy::after{content:"";position:absolute;background:var(--line)}
#joy::before{left:50%;top:8%;bottom:8%;width:1px}
#joy::after{top:50%;left:8%;right:8%;height:1px}
#knob{position:absolute;left:30%;top:30%;width:40%;height:40%;border-radius:50%;background:#20364a;border:2px solid var(--accent);pointer-events:none}
#dpad {
  display: grid;
  width: var(--js);
  height: var(--js);
  grid-template-columns: repeat(3, 1fr);
  grid-template-rows: repeat(3, 1fr);
  gap: 6px;
  grid-template-areas:
    ". u ."
    "l . r"
    ". d .";
}
#dpad button[data-d=f]{grid-area:u}#dpad button[data-d=b]{grid-area:d}#dpad button[data-d=l]{grid-area:l}#dpad button[data-d=r]{grid-area:r}
.hold{background:var(--panel);color:var(--text);border:1px solid var(--line);border-radius:6px;font-size:28px;touch-action:none}
.hold.act{background:#1d3a55;border-color:var(--accent)}
#speed{grid-area:speed}
#speed .row{display:flex;justify-content:space-between;align-items:baseline;font-size:13px;color:var(--dim);text-transform:uppercase;letter-spacing:.06em}
#speed output{font-size:22px;color:var(--text);font-variant-numeric:tabular-nums}
input[type=range]{width:100%;height:36px;accent-color:var(--accent);touch-action:pan-y}
#stop{grid-area:stop;height:64px;border:0;border-radius:6px;background:#c62828;color:#fff;font:700 20px system-ui,sans-serif;letter-spacing:.1em;touch-action:none}
#stop:active{background:#8e1b1b}
#diag{grid-area:diag;font-size:13px;color:var(--dim)}
#diag summary{padding:6px 0}
.grid4{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.grid4 .hold{height:48px;font-size:14px}
@media (orientation:landscape){
 :root{--js:min(46vw,68vh,280px)}
 main{grid-template-columns:1fr 300px;grid-template-areas:"pad tele" "pad speed" "pad stop" "pad diag";grid-template-rows:auto auto auto 1fr;align-content:start}
}
</style>
</head>
<body>
<header>
  <h1>Robot Car</h1>
  <div class="status"><span id="dot" class="dot"></span><span id="link">No link</span></div>
</header>
<main>
  <section id="tele" class="panel"><span>L <b id="tl">0%</b></span><span>R <b id="tr">0%</b></span></section>

  <section id="pad">
    <div class="seg"><button data-mode="joy" class="on">Joystick</button><button data-mode="btn">Buttons</button></div>
    <div id="joy" aria-label="Joystick"><div id="knob"></div></div>
    <div id="dpad" hidden>
      <button class="hold" data-d="f" aria-label="Forward">&#9650;</button>
      <button class="hold" data-d="l" aria-label="Left">&#9664;</button>
      <button class="hold" data-d="r" aria-label="Right">&#9654;</button>
      <button class="hold" data-d="b" aria-label="Backward">&#9660;</button>
    </div>
  </section>

  <section id="speed" class="panel">
    <div class="row"><label for="spd">Speed</label><output id="spdOut">50%</output></div>
    <input id="spd" type="range" min="10" max="100" step="5" value="50">
  </section>

  <button id="stop">STOP</button>

  <details id="diag" class="panel">
    <summary>Motor test (hold to run, wheels off the ground)</summary>
    <div class="grid4">
      <button class="hold" data-t="l+">Left fwd</button>
      <button class="hold" data-t="r+">Right fwd</button>
      <button class="hold" data-t="l-">Left rev</button>
      <button class="hold" data-t="r-">Right rev</button>
    </div>
  </details>
</main>

<script>
(() => {
  const $ = id => document.getElementById(id);
  const HEARTBEAT_MS = 100, DRIVE_TIMEOUT_MS = 800, STATUS_MS = 1000;

  let speed = 50;                       // slider value, 10..100 (%)
  let target = {l: 0, r: 0};            // wheel commands, -100..100
  let dirty = false, inFlight = false, lastOk = true, ctrl = null;
  let owner = null, srcFn = null;       // active control and its [-1..1] output

  /* ---------- network ---------- */
  function req(url, ms) {
    const c = new AbortController();
    const t = setTimeout(() => c.abort(), ms);
    const p = fetch(url, {cache: 'no-store', signal: c.signal})
      .then(r => { if (!r.ok) throw new Error(r.status); return r.json(); })
      .finally(() => clearTimeout(t));
    return {c, p};
  }
  function setLink(ok) {
    $('dot').classList.toggle('ok', ok);
    $('link').textContent = ok ? 'Connected' : 'No link';
  }
  const fmt = v => (v > 0 ? '+' : '') + v + '%';
  function show(j) {
    $('tl').textContent = fmt(j.l) + ' \u00b7 duty ' + j.ld;
    $('tr').textContent = fmt(j.r) + ' \u00b7 duty ' + j.rd;
  }

  // One request at a time keeps commands in order. STOP aborts a drive in flight.
  function pump() {
    if (inFlight) return;
    const moving = target.l !== 0 || target.r !== 0;
    if (!dirty && !moving) return;
    dirty = false; inFlight = true; lastOk = true;
    const q = req(moving ? `/drive?l=${target.l}&r=${target.r}` : '/stop', DRIVE_TIMEOUT_MS);
    q.c.drive = moving; ctrl = q.c;
    q.p.then(j => { setLink(true); show(j); })
      .catch(() => { if (q.c.byUs) return; lastOk = false; setLink(false); if (!moving) dirty = true; })
      .finally(() => { inFlight = false; ctrl = null; if (dirty && lastOk) pump(); });
  }
  function setTarget(l, r, force) {
    l = Math.round(l); r = Math.round(r);
    const same = l === target.l && r === target.r;
    target = {l, r};
    if (same && !force) return;
    dirty = true;
    if (l === 0 && r === 0 && ctrl && ctrl.drive) { ctrl.byUs = true; ctrl.abort(); }
    pump();
  }
  setInterval(pump, HEARTBEAT_MS);      // keep-alive while moving / retry a failed stop
  setInterval(() => {                   // idle status poll
    if (inFlight || dirty || target.l || target.r) return;
    req('/status', 1500).p.then(j => { setLink(true); show(j); }).catch(() => setLink(false));
  }, STATUS_MS);

  /* ---------- control ownership ---------- */
  function update() {
    let l = 0, r = 0;
    if (srcFn) [l, r] = srcFn();
    setTarget(l * speed, r * speed, false);
  }
  function press(o, fn) { owner = o; srcFn = fn; update(); }
  function release(o) { if (owner === o) { owner = null; srcFn = null; update(); } }
  function stopAll() {
    owner = null; srcFn = null;
    resetJoy();
    document.querySelectorAll('.hold.act').forEach(b => b.classList.remove('act'));
    keys.clear();
    setTarget(0, 0, true);
  }
  const mix = (throttle, turn) => {
    const l = throttle + turn, r = throttle - turn;
    const m = Math.max(1, Math.abs(l), Math.abs(r));
    return [l / m, r / m];
  };

  /* ---------- joystick ---------- */
  const joy = $('joy'), knob = $('knob');
  let joyId = null, jx = 0, jy = 0;
  function resetJoy() { joyId = null; jx = jy = 0; knob.style.transform = ''; }
  function joyMove(e) {
    const b = joy.getBoundingClientRect(), R = b.width / 2, travel = R * 0.6;
    let dx = e.clientX - (b.left + R), dy = e.clientY - (b.top + R);
    const d = Math.hypot(dx, dy);
    if (d > travel) { dx *= travel / d; dy *= travel / d; }
    knob.style.transform = `translate(${dx}px,${dy}px)`;
    jx = dx / travel; jy = dy / travel;
    if (Math.hypot(jx, jy) < 0.12) { jx = 0; jy = 0; }   // dead zone
    update();
  }
  joy.addEventListener('pointerdown', e => {
    e.preventDefault(); joyId = e.pointerId; joy.setPointerCapture(e.pointerId);
    press(joy, () => mix(-jy, jx)); joyMove(e);
  });
  joy.addEventListener('pointermove', e => { if (e.pointerId === joyId) joyMove(e); });
  ['pointerup', 'pointercancel', 'lostpointercapture'].forEach(t =>
    joy.addEventListener(t, e => { if (e.pointerId === joyId) { resetJoy(); release(joy); } }));

  /* ---------- hold buttons (D-pad + motor test) ---------- */
  function bindHold(el, fn) {
    el.addEventListener('pointerdown', e => {
      e.preventDefault(); el.setPointerCapture(e.pointerId); el.classList.add('act'); press(el, fn);
    });
    ['pointerup', 'pointercancel', 'lostpointercapture'].forEach(t =>
      el.addEventListener(t, () => { el.classList.remove('act'); release(el); }));
  }
  const DIR = {f: [1, 1], b: [-1, -1], l: [-1, 1], r: [1, -1]};
  document.querySelectorAll('#dpad [data-d]').forEach(b => bindHold(b, () => DIR[b.dataset.d]));
  const TEST = {'l+': [1, 0], 'l-': [-1, 0], 'r+': [0, 1], 'r-': [0, -1]};
  document.querySelectorAll('[data-t]').forEach(b => bindHold(b, () => TEST[b.dataset.t]));

  /* ---------- speed, mode, stop ---------- */
  $('spd').addEventListener('input', e => { speed = +e.target.value; $('spdOut').textContent = speed + '%'; update(); });
  document.querySelectorAll('.seg button').forEach(b => b.addEventListener('click', () => {
    stopAll();
    document.querySelectorAll('.seg button').forEach(x => x.classList.toggle('on', x === b));
    const joyMode = b.dataset.mode === 'joy';
    joy.hidden = !joyMode; $('dpad').hidden = joyMode;
  }));
  $('stop').addEventListener('pointerdown', e => { e.preventDefault(); stopAll(); });

  /* ---------- keyboard (desktop) ---------- */
  const keys = new Set();
  const KEYMAP = {ArrowUp: 'f', w: 'f', ArrowDown: 'b', s: 'b', ArrowLeft: 'l', a: 'l', ArrowRight: 'r', d: 'r'};
  const kbdSrc = () => mix((keys.has('f') ? 1 : 0) - (keys.has('b') ? 1 : 0),
                           (keys.has('r') ? 1 : 0) - (keys.has('l') ? 1 : 0));
  document.addEventListener('keydown', e => {
    if (e.key === ' ') { e.preventDefault(); stopAll(); return; }
    const k = KEYMAP[e.key]; if (!k || e.repeat) return;
    e.preventDefault(); keys.add(k); press('kbd', kbdSrc);
  });
  document.addEventListener('keyup', e => {
    const k = KEYMAP[e.key]; if (!k) return;
    keys.delete(k); keys.size ? press('kbd', kbdSrc) : release('kbd');
  });

  /* ---------- safety hooks ---------- */
  window.addEventListener('blur', stopAll);
  window.addEventListener('pagehide', stopAll);
  document.addEventListener('visibilitychange', () => { if (document.hidden) stopAll(); });
  document.addEventListener('contextmenu', e => e.preventDefault());

  stopAll();   // on load: make sure the robot is told to stand still
})();
</script>
</body>
</html>)rawliteral";
