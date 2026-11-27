// SPDX-License-Identifier: GPL-3.0-only
// HamsterSynth's demo on the page: the sound, and the hamsters singing it, each when it sings in it. Who sings when,
// the drums and THALL come from the firmware's own run of the demo (demo/hamstersynth-demo.json, tools/ham_demo.sh),
// so they are where the sound is. The play button is a hamster; another runs along the seeds as it plays.
const P = document.getElementById('player');
if (P) {
  const $ = (s) => P.querySelector(s);
  const audio = $('#d-audio'), stage = $('#d-stage'), pp = $('#d-pp'), track = $('#d-track'), fill = $('#d-fill');
  const runner = $('#d-runner'), time = $('#d-time'), partEl = $('#d-part'), partsEl = $('#d-parts'), drummer = $('#d-drummer');
  const hams = [...P.querySelectorAll('.hm')];
  const band = P.querySelector('.band');
  const fit = () => {                             // the hamsters a whole number of times their pixels: 1, 2 or 3 x
    const w = hams[0] ? hams[0].getBoundingClientRect().width : 78;
    band.style.setProperty('--n', String(Math.max(1, Math.min(3, Math.floor(w / 30)))));
  };
  fit();
  window.addEventListener('resize', fit);
  let tl = null, last = -1, ih = 0, id = 0, playing = false, beat = 0;

  const fmt = (ms) => { const s = Math.max(0, Math.round(ms / 1000)); return `${Math.floor(s / 60)}:${String(s % 60).padStart(2, '0')}`; };
  const total = () => (audio.duration && isFinite(audio.duration) ? audio.duration * 1000 : tl ? tl.len : 0);
  const after = (a, t) => { let lo = 0, hi = a.length; while (lo < hi) { const m = (lo + hi) >> 1; if (a[m][0] <= t) lo = m + 1; else hi = m; } return lo; };
  const at = (a, t) => { const i = after(a, t) - 1; return i >= 0 ? a[i] : null; };

  fetch('demo/hamstersynth-demo.json').then((r) => (r.ok ? r.json() : null)).then((j) => {
    if (!j) return;
    tl = j;
    for (const [t, name] of tl.parts) {
      const b = document.createElement('button');
      b.type = 'button';
      b.textContent = name;
      b.dataset.t = t;
      b.addEventListener('click', () => seek(t, true));
      partsEl.appendChild(b);
    }
    draw();
  }).catch(() => {});

  function sing(v) {                              // the hamster hops (a sharp: up on its toes, sparkling), a note flies
    const h = hams[v & 7], cls = v & 8 ? 'sharp' : 'sing', k = v & 7;
    if (!h) return;
    h.classList.remove('sing', 'sharp');
    void h.offsetWidth;
    h.classList.add(cls);
    clearTimeout(h._t);
    h._t = setTimeout(() => h.classList.remove('sing', 'sharp'), cls === 'sharp' ? 240 : 170);
    if (h.querySelectorAll('.fly').length < 2) {
      const n = document.createElement('span');
      n.className = 'fly';
      n.textContent = v & 8 ? '♯' : '♪♫♪♬'[(k + beat++) & 3];
      n.style.setProperty('--dx', `${((k * 37 + beat * 13) % 30) - 15}px`);
      n.addEventListener('animationend', () => n.remove());
      h.appendChild(n);
    }
  }
  function drum(mask) {                           // the drummer, as on the FM-1: each drum its own move
    const f = mask & 8 ? 3 : (mask & 6) === 6 ? 5 : mask & 2 ? 1 : mask & 4 ? 2 : mask & 1 ? 4 : 0;
    drummer.dataset.f = f;
    clearTimeout(drummer._t);
    drummer._t = setTimeout(() => { drummer.dataset.f = 0; }, 120);
    if (mask & 1) {                               // a kick: everyone feels it
      for (const el of [pp, runner, stage]) { el.classList.remove('bump'); void el.offsetWidth; el.classList.add('bump'); }
    }
  }
  function draw() {
    const t = audio.currentTime * 1000, len = total();
    if (tl) {
      if (!playing || t < last || t - last > 400) {   // (a jump: from there, nothing sung on the way)
        ih = after(tl.ham, t);
        id = after(tl.drum, t);
      } else {
        while (ih < tl.ham.length && tl.ham[ih][0] <= t) sing(tl.ham[ih++][1]);
        while (id < tl.drum.length && tl.drum[id][0] <= t) drum(tl.drum[id++][1]);
      }
      const part = at(tl.parts, t), core = at(tl.thall, t);
      partEl.textContent = playing || t > 0 ? (part ? part[1] : '') : '';
      for (const b of partsEl.children) b.classList.toggle('on', part !== null && +b.dataset.t === part[0]);
      stage.classList.toggle('core', !!core && core[1] === 1);
    }
    last = t;
    const p = len ? Math.min(1, t / len) : 0;
    fill.style.width = `${p * 100}%`;
    runner.style.left = `${p * 100}%`;
    time.textContent = `${fmt(t)} / ${fmt(len)}`;
    track.setAttribute('aria-valuenow', String(Math.round(t / 1000)));
    track.setAttribute('aria-valuetext', fmt(t));
    if (playing) requestAnimationFrame(draw);
  }
  function seek(ms, play) {
    const go = () => { audio.currentTime = Math.max(0, ms / 1000); last = -1; draw(); if (play && audio.paused) audio.play(); };
    if (audio.readyState === 0) {                 // (nothing loaded yet: load, then go)
      audio.preload = 'auto';
      audio.addEventListener('loadedmetadata', go, { once: true });
      audio.load();
    } else go();
  }

  pp.addEventListener('click', () => { if (audio.paused) audio.play(); else audio.pause(); });
  audio.addEventListener('play', () => {
    playing = true;
    P.classList.add('on');
    pp.setAttribute('aria-label', 'Pause');
    last = -1;
    requestAnimationFrame(draw);
  });
  audio.addEventListener('pause', () => { playing = false; P.classList.remove('on'); pp.setAttribute('aria-label', 'Spill av'); draw(); });
  audio.addEventListener('ended', () => { playing = false; P.classList.remove('on'); audio.currentTime = 0; draw(); });
  audio.addEventListener('loadedmetadata', draw);

  const frac = (e) => { const r = track.getBoundingClientRect(); return Math.min(1, Math.max(0, (e.clientX - r.left) / r.width)); };
  track.addEventListener('pointerdown', (e) => {
    e.preventDefault();
    track.setPointerCapture?.(e.pointerId);
    seek(frac(e) * (total() || 57000), false);
    const move = (ev) => seek(frac(ev) * (total() || 57000), false);
    const up = () => { track.removeEventListener('pointermove', move); track.removeEventListener('pointerup', up); };
    track.addEventListener('pointermove', move);
    track.addEventListener('pointerup', up);
  });
  track.addEventListener('keydown', (e) => {
    const d = { ArrowLeft: -5000, ArrowRight: 5000, Home: -1e9, End: 1e9 }[e.key];
    if (d === undefined) return;
    e.preventDefault();
    seek(Math.min(total(), Math.max(0, audio.currentTime * 1000 + d)), false);
  });
  draw();
}
