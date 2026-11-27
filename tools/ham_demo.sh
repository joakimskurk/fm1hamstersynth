#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
# HamsterSynth's sound demo: the firmware's own code (firmware/src/hamster.c, its engine and its screen) on this
# computer (tests/host.c) plays tests/scripts/ham/demo.txt with a song of eight patterns (tests/ham/demo_song.h; the
# firmware itself starts as the page does). Out come, in build/demo/:
#   hamstersynth-demo.wav   the sound, as the FM-1 sends it over USB (44.1 kHz, 16 bit), brought to -1 dBFS
#   hamstersynth-demo.mp3   the same, 192 kbit/s                       (ffmpeg)
#   hamstersynth-demo.mp4   with the screen, 3 x, 30 frames a second   (ffmpeg)
#   hamstersynth-demo-side.mp3   the same demo in the page's voice, to hold against it
#   hamstersynth-demo.json  who sings when, the drums, THALL, the parts ("#@" in the script): the page's player
#   tools/ham_demo.sh [--no-video] [--docs]      --docs: the MP3 and the JSON to docs/ too (the site takes them there)
set -e
cd "$(dirname "$0")/.."
OUT=build/demo
rm -rf "$OUT"
mkdir -p "$OUT/frames"
cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST -DAPP_VER='"demo"' -DHAM_DEMO='"../../tests/ham/demo_song.h"' \
    -Ifirmware/hal -Ifirmware/src -Ifirmware/gen -o "$OUT/hamstersynth" firmware/src/hamster.c
VIDEO=1; DOCS=0
for a in "$@"; do case "$a" in --no-video) VIDEO=0 ;; --docs) DOCS=1 ;; esac; done
if [ $VIDEO = 0 ]; then
    "$OUT/hamstersynth" "$OUT/frames/f" tests/scripts/ham/demo.txt >"$OUT/run.log"
else
    "$OUT/hamstersynth" "$OUT/frames/f" tests/scripts/ham/demo.txt 30 >"$OUT/run.log"
fi
python3 - "$OUT/frames/f.wav" "$OUT/hamstersynth-demo.wav" <<'PY'
import struct, sys
import numpy as np
d = open(sys.argv[1], "rb").read()
x = np.frombuffer(d[44:], dtype="<i2").astype(float)
g = 32767 * 10 ** (-1 / 20) / max(np.abs(x).max(), 1)
y = np.clip(np.round(x * g), -32767, 32767).astype("<i2")
open(sys.argv[2], "wb").write(d[:44] + y.tobytes())
print(f"demo: {len(x) / 2 / 44100:.1f} s, peak brought to -1 dBFS (x {g:.2f})")
PY
if command -v ffmpeg >/dev/null 2>&1; then
    ffmpeg -loglevel error -y -i "$OUT/hamstersynth-demo.wav" -codec:a libmp3lame -b:a 192k \
        -metadata title="HamsterSynth (FM-1)" "$OUT/hamstersynth-demo.mp3"
    if [ $VIDEO = 1 ]; then
        ffmpeg -loglevel error -y -framerate 30 -i "$OUT/frames/f-%05d.ppm" -i "$OUT/hamstersynth-demo.wav" \
            -vf scale=720:720:flags=neighbor -c:v libx264 -preset slow -crf 16 -pix_fmt yuv420p \
            -c:a aac -b:a 192k -shortest -movflags +faststart "$OUT/hamstersynth-demo.mp4"
    fi
else
    echo "demo: no ffmpeg, the WAV only"
fi
if command -v ffmpeg >/dev/null 2>&1; then           # the page's voice, the sound only
    mkdir -p "$OUT/side"
    cc -O2 -Wall -Wno-unused-function -DDINGHY_HOST -DAPP_VER='"demo"' -DHAM_DEMO='"../../tests/ham/demo_song.h"' \
        -DHAM_DEMO_PAGE_VOICE -Ifirmware/hal -Ifirmware/src -Ifirmware/gen -o "$OUT/side/hamstersynth" firmware/src/hamster.c
    "$OUT/side/hamstersynth" "$OUT/side/f" tests/scripts/ham/demo.txt >/dev/null
    ffmpeg -loglevel error -y -i "$OUT/side/f.wav" -af "volume=$(python3 -c "
import numpy as np; d = open('$OUT/side/f.wav', 'rb').read(); x = np.frombuffer(d[44:], dtype='<i2')
print(f'{32767 * 10 ** (-1 / 20) / max(int(np.abs(x.astype(int)).max()), 1):.3f}')")" \
        -codec:a libmp3lame -b:a 192k -metadata title="HamsterSynth (FM-1), the page's voice" "$OUT/hamstersynth-demo-side.mp3"
    rm -rf "$OUT/side"
fi
python3 - "$OUT/run.log" tests/scripts/ham/demo.txt "$OUT/hamstersynth-demo.json" <<'PY'
import json, re, sys
log, script = open(sys.argv[1]).read(), open(sys.argv[2]).read()
ev = lambda what: [[int(t), int(v)] for t, v in re.findall(rf"^\s*(\d+) ms  {what} (\d+)$", log, re.M)]
parts = [[int(t), n.strip()] for t, n in re.findall(r"^#@ (\d+) (.+)$", script, re.M)]
end = int(re.findall(r"^(\d+) end$", script, re.M)[-1])
d = {"len": end, "parts": parts, "ham": ev("hamster"), "drum": ev("drums"), "thall": ev("thall")}
open(sys.argv[3], "w").write(json.dumps(d, separators=(",", ":")))
print(f"demo: {len(d['ham'])} hamster notes, {len(d['drum'])} drum hits, {len(parts)} parts -> {sys.argv[3]}")
PY
if [ $DOCS = 1 ] && [ -f "$OUT/hamstersynth-demo.mp3" ]; then
    cp "$OUT/hamstersynth-demo.mp3" docs/hamster-demo.mp3
    cp "$OUT/hamstersynth-demo.json" docs/hamster-demo.json
    echo "demo: docs/hamster-demo.mp3, docs/hamster-demo.json"
fi
rm -rf "$OUT/frames"
ls -l "$OUT"/hamstersynth-demo.*
