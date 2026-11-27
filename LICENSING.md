# Licensing

HamsterSynth, like Dinghy under it, is free software under the GNU General Public License, version 3 only
(`GPL-3.0-only`, full text in `LICENSE`). You may use, study, change and share it. If you give someone a firmware built
from it, give them its complete source under the same licence too.

## Who made what

| | |
| --- | --- |
| The platform: `firmware/hal/`, `firmware/loader/`, `crt0.S`, `app.ld`, `firmware/src/` (`usb.c`, `ota.c`, `recovery.c`, `lcd.c`, `gfx.c`, `libc.c`, `bootguard.h`, `uac_fir.h`), `tools/` (but for HamsterSynth's, below) | Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments ([Felucca](https://github.com/hugelton/Felucca)), with the changes of [SLOOP](https://github.com/isod89/sloop-fm1) (isod89) |
| The SMK-37 board layer: `firmware/hal/smk_input.h`, `fm1_flash_map.h`, the SMK-37 parts of `fm1_cc.h`, `fm1_flash.h`, `fm1_lcd_hw.h`, `lcd.c`, `ota.c`, `bootguard.h`, `firmware/loader/`, `tools/fm1pkg_make.py`, `tools/fm1_install.py`, `tools/build.py`; `firmware/src/smk_wheel.h`; `tests/ota_test.c`, `ldr_test.c`, `install_test.py`, `smk_wheel_test.c`, `smk_pkg_test.py` | Copyright (C) 2026 M. Geisler (genkigenki/makkuro), from [genkigenki/sloop-fm1](https://github.com/genkigenki/sloop-fm1) branch `smk37` (the reference port of the [fm1-to-smk37](https://github.com/genkigenki/fm1-to-smk37) kit) |
| The newer FM-1s' screen: the ST7789V register values and their order in `firmware/src/lcd.c`'s `LCD_SEQ` (Dinghy 0.6.1) | Found by Jake Handy for [h1](https://github.com/jakehandy/h1) (0.3.1), M-VAVE's own values; in Dinghy by zvenson |
| The TRS MIDI IN `firmware/src/midi_uart.c` | Felucca's, as sloopDX carries it, with the ring read by content from Felucca [Salt] 0.9-salt14 by ChanceTheMaker (found by keremimo) |
| The update loader's screen: `firmware/loader/ldr_screen.c`, `tests/ldr_screen_test.c` | Copyright (C) 2026 zvenson (from OberVave, the Firmware Hub's look); the idea, a loader that draws, its 5 x 7 letters and the order of things from [Skiff](https://github.com/illuminus5/skiff) by illuminus5, GPL-3.0 |
| From OberVave: the storage (blobs, slots), `LCD_IDLE`, the SMK-37 wiring of `platform.c`, `store.h`, the pad event queue in `smk_input.h`, `tests/smk_flash_test.c`, `tests/run_tests.sh`, the CI workflow | Copyright (C) 2026 zvenson |
| The wiring `platform.c`, the host mode `tests/host.c`, Dinghy's docs (`docs/API.md`, `RULES.md`, `MAKE-IT-YOURS.md`, `SMK37.md`, `DINGHY_README.md`) | zvenson |
| The browser installer's protocol: `web/fm1pkg.js`, `web/fm1ota.js` (unchanged, from [SLOOP](https://github.com/isod89/sloop-fm1) `bac9c05`); the install flow of `web/index.html` and the shape of `web/make_site.py`, after SLOOP's `web/index_pkg.html` and `web/make_site.py` | Copyright (C) 2026 Leo Kuroshita (@kurogedelic), Hügelton Instruments (Felucca), with the changes of SLOOP (isod89) |
| HamsterSynth: `firmware/src/hamster.c` (from Dinghy's `dinghy.c`), `ham_engine.h`, `ham_draw.h`, `firmware/gen/ham_*.h`, `tools/ham_sprites.py`, `gen_ham_tables.py`, `gen_ham_art.py`, `ham_ref.js`, `ham_demo.sh`, `emu_hamster.sh`, `emu_ci.sh`, `features.py`, `tests/run_hamster.sh`, `tests/ham/`, `tests/scripts/`, `tests/ref/`, `tests/web/`, the page `web/index.html`, `web/make_site.py`, `web/make_og.py` (but for what is SLOOP's in them, above), HamsterSynth in the browser `web/app/` (its `web_host.c` after Dinghy's `tests/host.c`), `README.md`, `docs/screens/`, `docs/hamster-demo.mp3` and `.json`, `PLATFORM.md` | Copyright (C) 2026 Joakim (joakimskurk) |

## Third-party material

| What | Licence | Where |
| --- | --- | --- |
| Terminus font 8x16 (ter-u16n), rendered into `firmware/gen/felucca_font.h` | SIL OFL 1.1 | `assets/fonts/Terminus-LICENSE.txt` |
| JieLi AC79 SDK: `uboot.boot`, `cfg_tool.bin`, `eq_cfg_hw.bin` are read from your SDK checkout at build time and placed in the package; no SDK files are in this tree | Apache-2.0 | <https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK> |

## Hamster Piano, the hamsters and the lore

HamsterSynth is a port of the web page *Hamster Piano* (v4.17) by this repository's author. The hamsters and the
drummer are drawn anew here, pixel by pixel, after the page's (`tools/ham_sprites.py`). The page itself is not in this
repository (only measurements of its sound: `tests/ref/ham_features.json`). Every sound is synthesised; no sample is
in the firmware.

What the hamsters say on the sign are short sayings after [Propanpedia](https://jhf.no/musikklubben/propanpedia/),
Musikklubben's own, retold in a few words. No text is copied from it at length.

## Trademarks

"Felucca" and "Hügelton Instruments" are names of Hügelton Instruments; "M-VAVE", "FM-1", "SMK-37" and "MKE-P37"
belong to their owners. Dinghy and HamsterSynth are not made or endorsed by them.
