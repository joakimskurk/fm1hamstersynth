#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""The site's pictures, from the firmware's own screens (docs/screens/ham-*.png, 480 x 480: tests/run_hamster.sh's
tour) and its own hamsters (tools/ham_sprites.py):

  docs/screens/ham-logo.png   the logo bar off the screen, 4 x (960 x 96)
  docs/screens/ham-icon.png   the orange hamster, its mouth open, its seed (64 x 64): the page's icon
  web/og.png                  the link preview (1200 x 630): the logo and three screens on the page's paper

  python3 web/make_og.py
"""
import sys
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import ham_sprites  # noqa: E402

S = ROOT / "docs" / "screens"
PAPER, INK = (0xE8, 0xDF, 0xCB), (0x09, 0x1A, 0x25)


def main():
    home = Image.open(S / "ham-home.png").convert("RGB")
    logo = home.crop((0, 0, 480, 48)).resize((960, 96), Image.NEAREST)
    logo.save(S / "ham-logo.png")
    g = ham_sprites.hamsters()[5]
    icon = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    for y, row in enumerate(g):
        for x, c in enumerate(row):
            if c:
                icon.paste(c + (255,), (6 + 2 * x, 5 + 2 * y, 8 + 2 * x, 7 + 2 * y))
    icon.save(S / "ham-icon.png")
    og = Image.new("RGB", (1200, 630), PAPER)
    d = ImageDraw.Draw(og)
    for x in range(0, 1200, 8):
        d.line([(x, 0), (x, 630)], fill=(0xE2, 0xD9, 0xC5))
    for y in range(0, 630, 8):
        d.line([(0, y), (1200, y)], fill=(0xE2, 0xD9, 0xC5))
    d.rectangle([114, 38, 1085, 140], fill=INK)
    og.paste(logo, (120, 44))
    for i, n in enumerate(("ham-sign.png", "ham-tromster.png", "ham-edit.png")):
        x = 30 + i * 390
        d.rectangle([x - 4, 176, x + 360 + 3, 176 + 360 + 7], fill=INK)
        og.paste(Image.open(S / n).convert("RGB").resize((360, 360), Image.NEAREST), (x, 180))
    og.save(ROOT / "web" / "og.png")
    print("ham-logo.png, ham-icon.png, web/og.png")


if __name__ == "__main__":
    main()
