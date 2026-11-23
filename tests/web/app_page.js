// SPDX-License-Identifier: GPL-3.0-only
// The site's «Prøv i nettleseren» in Chromium (Playwright): Start loads the firmware into the audio thread, its screen
// comes up (the logo's blue), a hamster pressed on the computer's keyboard (A: C4) lights its note on the screen, space
// (PLAY) starts the pattern (the state's bit 0), PLAY again stops it. No page errors.
//
//   node tests/web/app_page.js SITE_DIR
const { chromium } = require('playwright');
const http = require('http'), fs = require('fs'), path = require('path');
const dir = process.argv[2];
if (!dir) { console.error('usage: node tests/web/app_page.js SITE_DIR'); process.exit(2); }
const types = { '.html': 'text/html', '.png': 'image/png', '.js': 'text/javascript', '.wasm': 'application/wasm',
                '.mp3': 'audio/mpeg', '.fwsc': 'application/octet-stream' };
const server = http.createServer((q, r) => {
  const f = path.join(dir, decodeURIComponent(q.url.split('?')[0]).replace(/\/$/, '/index.html'));
  fs.readFile(f, (e, d) => { if (e) { r.writeHead(404); r.end(); } else { r.writeHead(200, { 'Content-Type': types[path.extname(f)] || 'application/octet-stream' }); r.end(d); } });
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
  await p.click('#start');
  await p.waitForFunction(() => document.getElementById('app').classList.contains('on'), null, { timeout: 20000 });
  await p.waitForTimeout(800);
  const px = (x, y) => p.evaluate(([x, y]) => [...document.getElementById('lcd').getContext('2d').getImageData(x, y, 1, 1).data].slice(0, 3), [x, y]);
  const near = (a, b) => a.every((v, i) => Math.abs(v - b[i]) <= 16);
  check('the firmware runs: its logo', near(await px(6, 6), [0x42, 0x6B, 0x7C]), JSON.stringify(await px(6, 6)));
  await p.keyboard.down('a');
  await p.waitForTimeout(60);
  const label = await px(60, 2 * 92);
  await p.keyboard.up('a');
  check('A on the keyboard: the C hamster sings (its note lit)', near(label, [0xD9, 0x5F, 0x67]), JSON.stringify(label));
  await p.keyboard.press(' ');
  await p.waitForTimeout(500);
  check('space: PLAY, it plays', ((await p.evaluate(() => window.__hamster)) & 1) === 1);
  await p.keyboard.press(' ');
  await p.waitForTimeout(400);
  check('space again: it stops', ((await p.evaluate(() => window.__hamster)) & 1) === 0);
  check('no page errors', !errs.length, errs.join(' '));
  await b.close();
  server.close();
  process.exit(fails ? 1 : 0);
});
