#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""The site's pictures, from the firmware's own screens (docs/screens/ham-*.png, 480 x 480: tests/run_hamster.sh's
tour) and its own hamsters (tools/ham_sprites.py):

  docs/screens/ham-logo.png   the logo bar off the screen, 4 x (960 x 96)
  docs/screens/ham-icon.png   the orange hamster, its mouth open (64 x 64): the page's icon
  web/og.png                  the link preview (1200 x 630): the logo and three screens on the page's paper
  docs/screens/ham-strip.png  the eight hamsters in their four poses side by side, their own pixels (32 x 26 x 28; rest,
                              sing, sharp, flat; the sharp a row higher): the demo's player
  docs/screens/ham-drummer.png  the drummer's four frames side by side (4 x 56 x 45): the demo's player

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
    blank = [None] * ham_sprites.W
    cells = []
    for ps in ham_sprites.hamster_poses():      # (a row more: the sharp lifted into it, the others under it)
        for j, g in enumerate(ps):
            cells.append(g + [blank] if ham_sprites.POSES[j] == "sharp" else [blank] + g)
    strip(cells, S / "ham-strip.png")
    strip(ham_sprites.drummer(), S / "ham-drummer.png")
    print("ham-logo.png, ham-icon.png, web/og.png, ham-strip.png, ham-drummer.png")


def strip(grids, path):
    """grids (rows of colours, None clear), side by side, one pixel each, on nothing"""
    h, w = len(grids[0]), len(grids[0][0])
    im = Image.new("RGBA", (w * len(grids), h), (0, 0, 0, 0))
    for i, g in enumerate(grids):
        for y, row in enumerate(g):
            for x, c in enumerate(row):
                if c:
                    im.putpixel((i * w + x, y), c + (255,))
    im.save(path)


if __name__ == "__main__":
    main()
