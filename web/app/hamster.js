// SPDX-License-Identifier: GPL-3.0-only
// HamsterSynth på siden: firmwaren kjører i lydtråden (worklet.js, hamstersynth.wasm); her er skjermen dens, lysene
// og FM-1-panelet: klaviaturet, knappene (trykk, eller hold), dreieknappene. Datamaskinens tastatur spiller også.
// Ingenting lastes før Start (en nettleser spiller lyd først etter et klikk).
const $ = (id) => document.getElementById(id);
const app = $('app');
if (app) {
  const lcd = $('lcd'), g = lcd.getContext('2d');
  const small = document.createElement('canvas');
  small.width = small.height = 240;
  const gs = small.getContext('2d');
  const img = gs.createImageData(240, 240);
  const px32 = new Uint32Array(img.data.buffer);
  const status = $('appstatus');
  let ac = null, node = null, gain = null, booting = false, keys = 0;

  const send = (m) => node && node.port.postMessage(m);
  const say = (t) => { status.textContent = t; };

  // the 27 keys, F3..G5 (LED id 14 + key); the black ones where platform.c's pattern has them
  const isBlack = (k) => (0x54A >> ((53 + k) % 12)) & 1;
  const keysEl = $('keys');
  const keyEls = [];
  const nWhite = [...Array(27).keys()].filter((k) => !isBlack(k)).length;
  let wi = 0;
  for (let k = 0; k < 27; k++) {
    const el = document.createElement('button');
    el.type = 'button';
    el.dataset.k = k;
    el.setAttribute('aria-label', `Tangent ${k + 1}`);
    if (isBlack(k)) {
      el.className = 'kb';
      el.style.left = `calc(${(100 * wi) / nWhite}% - ${(100 * 0.32) / nWhite}%)`;
      el.style.width = `${(100 * 0.64) / nWhite}%`;
    } else {
      el.className = 'kw';
      el.style.left = `${(100 * wi) / nWhite}%`;
      el.style.width = `${100 / nWhite}%`;
      wi++;
    }
    keysEl.appendChild(el);
    keyEls.push(el);
  }
  const keyDown = (k) => { keys |= 1 << k; send({ type: 'keys', mask: keys }); keyEls[k].classList.add('held'); };
  const keyUp = (k) => { keys &= ~(1 << k); send({ type: 'keys', mask: keys }); keyEls[k].classList.remove('held'); };
  const ptr = new Map();                         // pointer -> the key it holds (several fingers at once)
  keysEl.addEventListener('pointerdown', (e) => {
    const k = e.target.dataset.k;
    if (k === undefined) return;
    e.preventDefault();
    keysEl.setPointerCapture?.(e.pointerId);
    ptr.set(e.pointerId, +k);
    keyDown(+k);
  });
  const release = (e) => { if (ptr.has(e.pointerId)) { keyUp(ptr.get(e.pointerId)); ptr.delete(e.pointerId); } };
  for (const t of ['pointerup', 'pointercancel', 'lostpointercapture']) keysEl.addEventListener(t, release);

  // the buttons: down and up (the firmware tells a press from a hold: FX, GLO, SEQ, SEL and HOME held do more)
  const btnEls = new Map();
  for (const b of app.querySelectorAll('[data-btn]')) {
    const id = +b.dataset.btn;
    btnEls.set(id, b);
    let held = false;
    const up = () => { if (held) { held = false; send({ type: 'up', id }); b.classList.remove('held'); } };
    b.addEventListener('pointerdown', (e) => { e.preventDefault(); held = true; send({ type: 'down', id }); b.classList.add('held'); });
    for (const t of ['pointerup', 'pointerleave', 'pointercancel']) b.addEventListener(t, up);
    b.addEventListener('keydown', (e) => { if ((e.key === 'Enter' || e.key === ' ') && !e.repeat) { e.preventDefault(); held = true; send({ type: 'down', id }); } });
    b.addEventListener('keyup', (e) => { if (e.key === 'Enter' || e.key === ' ') up(); });
  }
  for (const b of app.querySelectorAll('[data-enc]'))
    b.addEventListener('click', () => send({ type: 'enc', id: +b.dataset.enc, n: +b.dataset.n }));

  // the computer's keyboard: a piano's row (A W S E D F T G Y H U J K), space PLAY, R REC, the arrows SELECT and PRESETS
  const KEYMAP = { a: 7, w: 8, s: 9, e: 10, d: 11, f: 12, t: 13, g: 14, y: 15, h: 16, u: 17, j: 18, k: 19, o: 20, l: 21, p: 22 };
  const BTNMAP = { ' ': 12, r: 13, z: 0, x: 1 };
  const ENCMAP = { ArrowLeft: [0, -1], ArrowRight: [0, 1], ArrowUp: [6, 1], ArrowDown: [6, -1] };
  const typing = () => /^(INPUT|TEXTAREA|SELECT)$/.test(document.activeElement?.tagName || '');
  window.addEventListener('keydown', (e) => {
    if (!node || typing() || e.metaKey || e.ctrlKey || e.altKey) return;
    const c = e.key.length === 1 ? e.key.toLowerCase() : e.key;
    if (c in KEYMAP || c in BTNMAP || c in ENCMAP) e.preventDefault();
    if (e.repeat) return;
    if (c in KEYMAP) keyDown(KEYMAP[c]);
    else if (c in BTNMAP) send({ type: 'down', id: BTNMAP[c] });
    else if (c in ENCMAP) send({ type: 'enc', id: ENCMAP[c][0], n: ENCMAP[c][1] });
  });
  window.addEventListener('keyup', (e) => {
    if (!node) return;
    const c = e.key.length === 1 ? e.key.toLowerCase() : e.key;
    if (c in KEYMAP) keyUp(KEYMAP[c]);
    else if (c in BTNMAP) send({ type: 'up', id: BTNMAP[c] });
  });

  function draw(p) {                             // RGB565 -> the canvas, twice its size, sharp
    for (let i = 0; i < 57600; i++) {
      const c = p[i];
      const r = ((c >> 11) * 527 + 23) >> 6, gg = (((c >> 5) & 63) * 259 + 33) >> 6, b = ((c & 31) * 527 + 23) >> 6;
      px32[i] = 0xFF000000 | (b << 16) | (gg << 8) | r;
    }
    gs.putImageData(img, 0, 0);
    g.imageSmoothingEnabled = false;
    g.drawImage(small, 0, 0, lcd.width, lcd.height);
  }
  const bit = (v, id) => (id < 32 ? v[0] & (1 << id) : v[1] & (1 << (id - 32))) !== 0;
  function lights(leds, glow) {                  // the buttons' and the keys' lights, as the firmware has them
    for (const [id, b] of btnEls) b.classList.toggle('lit', bit(leds, id));
    for (let k = 0; k < 27; k++) {
      keyEls[k].classList.toggle('on', bit(leds, 14 + k));
      keyEls[k].classList.toggle('glow', !bit(leds, 14 + k) && bit(glow, 14 + k));
    }
  }

  async function start() {
    if (booting) return;
    if (ac) {
      if (ac.state === 'running') { await ac.suspend(); $('start').hidden = false; $('start').textContent = '▶ Fortsett'; say('På pause.'); }
      else { await ac.resume(); $('start').hidden = true; say('Hamsterne er klare.'); }
      return;
    }
    booting = true;
    say('Henter hamsterne…');
    try {
      ac = new AudioContext({ sampleRate: 44100, latencyHint: 'interactive' });
      const [wasm] = await Promise.all([
        fetch('app/hamstersynth.wasm').then((r) => { if (!r.ok) throw new Error(`hamstersynth.wasm: ${r.status}`); return r.arrayBuffer(); }),
        ac.audioWorklet.addModule('app/worklet.js'),
      ]);
      node = new AudioWorkletNode(ac, 'hamstersynth', { numberOfInputs: 0, outputChannelCount: [2] });
      gain = ac.createGain();
      gain.gain.value = vol();
      node.connect(gain).connect(ac.destination);
      node.port.onmessage = (e) => {
        const m = e.data;
        if (m.type === 'frame') { if (m.lcd) draw(m.lcd); lights(m.leds, m.glow); window.__hamster = m.state; }
        else if (m.type === 'ready') { $('start').hidden = true; app.classList.add('on'); say('Hamsterne er klare.'); }
        else if (m.type === 'error') say(`Den startet ikke: ${m.message}`);
      };
      node.port.postMessage({ type: 'boot', wasm }, [wasm]);
      await ac.resume();
    } catch (err) {
      say(`Den startet ikke her: ${err.message || err}. Den trenger en nettleser med Web Audio (alle nyere).`);
      ac = null;
      node = null;
    }
    booting = false;
  }

  const vol = () => { const v = $('vol').value / 100; return v * v; };
  $('vol').addEventListener('input', () => { if (gain) gain.gain.value = vol(); });
  $('start').addEventListener('click', start);
  $('b-pause').addEventListener('click', () => { if (ac) start(); });
}
