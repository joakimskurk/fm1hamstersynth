# HamsterSynth: the everyday commands. The firmware itself is built by ./build.sh (docs/DINGHY_README.md: the toolchain).
#   make test       the host tests: the tables and pictures, the sound against the page, the panel through scripts
#   make host       the app on the host with the default script: build/host/hamstersynth.wav and its screens
#   make tables     firmware/gen/ham_tables.h (tools/gen_ham_tables.py)
#   make art        firmware/gen/ham_art.h from tools/ham_sprites.py (tools/gen_ham_art.py)
#   make fw         the FM-1 package build/hamstersynth.fwsc (JieLi toolchain in ~/.jieli, SDK in ~/fw-AC79_AIoT_SDK)
#   make emu        the package in fm1-emu (tools/emu_ci.sh builds fm1-emu first, if it is not there)
#   make check      what CI runs after the package: the host tests, Dinghy's platform tests, fm1-emu
#   make demo       the sound demo: build/demo/hamstersynth-demo.wav, .mp3, .mp4 (ffmpeg), .json; and to docs/ for the site
#   make wasm       HamsterSynth for the browser: build/web/hamstersynth.wasm (clang with wasm-ld)
#   make site       the site in build/site (after make fw and make wasm)
#   make site-test  the site in a browser (node with Playwright): the app starts and plays, the demo's player plays and
#                   its hamsters sing, the installer installs
PY ?= python3
VER ?= dev

.PHONY: test host tables art fw emu check demo wasm site site-test clean
test:
	sh tests/run_hamster.sh
host:
	sh tests/run_host.sh
tables:
	$(PY) tools/gen_ham_tables.py
art:
	$(PY) tools/gen_ham_art.py
fw:
	PYTHON=$(PY) sh build.sh
	sha256sum build/hamstersynth.fwsc
emu:
	sh tools/emu_ci.sh
check: fw
	sh tests/run_hamster.sh
	sh tests/run_tests.sh
	sh tools/emu_ci.sh
demo:
	sh tools/ham_demo.sh --docs
wasm:
	APP_VER=$(VER) sh web/app/build.sh build/web/hamstersynth.wasm
site: wasm
	$(PY) web/make_og.py
	$(PY) web/make_site.py build/hamstersynth.fwsc $(VER) build/site build/web/hamstersynth.wasm
site-test: site
	node tests/web/app_page.js build/site
	node tests/web/player_page.js build/site
	node tests/web/install_sim.js build/site
clean:
	rm -rf build
