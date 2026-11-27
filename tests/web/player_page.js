// SPDX-License-Identifier: GPL-3.0-only
// The site's demo player in Chromium (Playwright): its hamster plays the demo, the hamsters sing as it plays (the
// timeline, demo/hamstersynth-demo.json), a part's button takes it there (the hook: Refreng), HAMSTERCORE darkens the
// stage, the seeds' track and the time follow, the hamster pauses it. No page errors, no sideways scroll on a phone.
//
//   node tests/web/player_page.js SITE_DIR
const { chromium } = require('playwright');
const http = require('http'), fs = require('fs'), path = require('path');
const dir = process.argv[2];
if (!dir) { console.error('usage: node tests/web/player_page.js SITE_DIR'); process.exit(2); }
const types = { '.html': 'text/html', '.png': 'image/png', '.js': 'text/javascript', '.wasm': 'application/wasm',
                '.mp3': 'audio/mpeg', '.json': 'application/json', '.fwsc': 'application/octet-stream' };
const server = http.createServer((q, r) => {         // (with ranges: the audio element asks for them to seek)
  const f = path.join(dir, decodeURIComponent(q.url.split('?')[0]).replace(/\/$/, '/index.html'));
  fs.stat(f, (e, st) => {
    if (e) { r.writeHead(404); r.end(); return; }
    const ty = types[path.extname(f)] || 'application/octet-stream', m = /bytes=(\d*)-(\d*)/.exec(q.headers.range || '');
    if (m) {
      const a = +m[1] || 0, b = m[2] ? +m[2] : st.size - 1;
      r.writeHead(206, { 'Content-Type': ty, 'Content-Range': `bytes ${a}-${b}/${st.size}`, 'Accept-Ranges': 'bytes', 'Content-Length': b - a + 1 });
      fs.createReadStream(f, { start: a, end: b }).pipe(r);
    } else {
      r.writeHead(200, { 'Content-Type': ty, 'Accept-Ranges': 'bytes', 'Content-Length': st.size });
      fs.createReadStream(f).pipe(r);
    }
  });
});
let fails = 0;
const check = (what, ok, got = '') => { console.log(`${what}: ${ok ? 'ok' : 'FAIL'} ${got}`); if (!ok) fails++; };
server.listen(0, async () => {
  const url = `http://127.0.0.1:${server.address().port}/`;
  const b = await chromium.launch({ args: ['--autoplay-policy=no-user-gesture-required'] });
  const p = await b.newPage();
  const errs = [];
  p.on('pageerror', (e) => errs.push(String(e)));
  await p.goto(url, { waitUntil: 'networkidle' });
  const state = () => p.evaluate(() => ({
    t: document.getElementById('d-audio').currentTime, paused: document.getElementById('d-audio').paused,
    part: document.getElementById('d-part').textContent, core: document.getElementById('d-stage').classList.contains('core'),
    fill: parseFloat(document.getElementById('d-fill').style.width) || 0, time: document.getElementById('d-time').textContent }));
  check('the parts as buttons, from the timeline', (await p.locator('#d-parts button').count()) === 7);
  await p.click('#d-pp');
  await p.waitForTimeout(1500);
  let s = await state();
  check('the hamster plays it', !s.paused && s.t > 0.5, JSON.stringify(s));
  await p.locator('#d-parts button', { hasText: 'Refreng' }).click();
  let sang = 0;
  for (let i = 0; i < 20; i++) { await p.waitForTimeout(100); sang = Math.max(sang, await p.locator('.hm.sing').count()); }
  s = await state();
  check('Refreng: there (27.85 s on), and the hamsters sing as it plays', s.part === 'Refreng' && s.t > 27.8 && s.t < 31 && sang >= 1, `${JSON.stringify(s)} sang ${sang}`);
  check('the seeds fill as far as it is, the time says it', s.fill > 45 && s.fill < 56 && /^0:(2[89]|3[01]) \/ 0:57$/.test(s.time), `${s.fill} ${s.time}`);
  await p.locator('#d-parts button', { hasText: 'HAMSTERCORE' }).click();
  await p.waitForTimeout(800);
  s = await state();
  check('HAMSTERCORE: the stage goes dark', s.core && s.part === 'HAMSTERCORE', JSON.stringify(s));
  await p.click('#d-pp');
  await p.waitForTimeout(300);
  s = await state();
  check('the hamster pauses it', s.paused, JSON.stringify(s));
  await p.setViewportSize({ width: 390, height: 800 });
  await p.waitForTimeout(200);
  check('a phone: no sideways scroll', (await p.evaluate(() => document.documentElement.scrollWidth)) <= 390);
  check('no page errors', !errs.length, errs.join(' '));
  await b.close();
  server.close();
  process.exit(fails ? 1 : 0);
});
