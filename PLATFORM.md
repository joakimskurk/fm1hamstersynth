# PLATFORM

HamsterSynth runs on **Dinghy 0.5**, imported as source (as VaveHero and FM-1 Clock do).

- upstream: https://github.com/zvenson/dinghy
- commit: `ea1b6bf` (2026-11, after "Dinghy 0.5" `1675934`; the release workflow's file pattern only)
- Dinghy's docs stay in `docs/` (`API.md`, `RULES.md`, `MAKE-IT-YOURS.md`, `SMK37.md`). Its README is
  `docs/DINGHY_README.md` (the toolchain, the SDK, the host mode).
- HamsterSynth began in [joakimskurk/fm1clock](https://github.com/joakimskurk/fm1clock) as a second app beside FM-1
  Clock, and lives here on its own.

## What differs from upstream

Kept small, so a later Dinghy can be merged by diffing these files.

| file | change |
| --- | --- |
| `firmware/src/dinghy.c` | renamed `hamster.c`: the app; `APP_NAME "HAMSTERSYNTH"`, `STORE_MAGIC` "HAMR"; with it `ham_engine.h`, `ham_draw.h` and `firmware/gen/ham_*.h` |
| `tools/build.py` | compiles `hamster.c`; outputs `hamstersynth.*` and `hamstersynth.syms` (the symbol table: `tools/emu_hamster.sh` reads the engine's state by its address); identity `FM-1_902`, one for every build (`SMK-37 Pro_902` on the SMK-37); loader screen "HAMSTERSYNTH" in `0xF54A`; data family `HAMSTER` |
| `build.sh` | a comment |
| `tests/run_host.sh` | builds `hamster.c` as `build/host/hamstersynth` |
| `tests/run_tests.sh` | `hamstersynth.*` names; the SMK-37 packages' identities `_902` |
| `tests/install_test.py` | `hamstersynth.*` names |
| `README.md` | moved to `docs/DINGHY_README.md`; HamsterSynth's README in its place |
| `.github/workflows/release.yml` | replaced by `build.yml` (zvenson/fm1-build, as VaveHero, RoboSeq and SkurkyMello), with a job for the site (GitHub Pages) |
| `web/` (Dinghy has none) | the site: `fm1pkg.js` and `fm1ota.js` from SLOOP (`bac9c05`, unchanged: the update protocol over Web MIDI; Dinghy's `tests/install_test.py` checks `fm1pkg.js` against `tools/fm1pkg_make.py`), HamsterSynth's own page and `make_site.py` |

Nothing in `firmware/hal/`, `firmware/loader/`, `firmware/app.ld` or `tests/host.c` is changed. `platform.c` has one
change: with `DINGHY_WEB` (and `DINGHY_HOST`) it includes `web/app/web_host.c`, the page's FM-1, instead of
`tests/host.c` (HamsterSynth in the browser: `web/app/`).

## Updating Dinghy

1. Diff the new upstream against the commit above for the imported paths.
2. Apply upstream changes; re-apply the table above.
3. `make test`, CI's build, a boot in fm1-emu.
4. Update the commit here.
