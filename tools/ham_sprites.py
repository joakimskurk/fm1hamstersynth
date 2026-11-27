# SPDX-License-Identifier: GPL-3.0-only
"""HamsterSynth's pictures, drawn here pixel by pixel: the eight hamsters and the Tromster's drummer.

Each is drawn on a grid of its own pixels (a hamster 26 x 27, the chubby one 30 x 27, each in a place 30 wide; the drummer 56 x 45); the screen shows each of them as
2 x 2 of its own, as the logo's letters and the text. One ink for every outline, the light from the upper left: a
pixel of fur against the outline at its right or below is in shade, at its left or above (on the upper left) in light.

  hamsters()          -> [grid] * 8, a grid: rows of (r, g, b) or None (clear)
  drummer()           -> [grid] * 4, its four frames
tools/gen_ham_art.py makes firmware/gen/ham_art.h of them, and a sheet to look at (--sheet).
"""

INK = (0x2A, 0x1A, 0x14)
EYE = (0x1C, 0x10, 0x0E)
GLINT = (0xFF, 0xFF, 0xFF)
EAR = (0xF2, 0x9A, 0xA0)
NOSE = (0xE0, 0x6A, 0x78)
BLUSH = (0xF6, 0xB0, 0xB0)
PAW = (0xF7, 0xD2, 0xC2)
PAW_S = (0xE6, 0xAE, 0x9C)
MOUTH = (0x6A, 0x24, 0x2A)
TONGUE = (0xF0, 0x7E, 0x8A)

# fur: light, mid, shade; belly: light, shade
FUR = {
    "golden": ((0xF8, 0xCE, 0x8A), (0xE9, 0xAB, 0x62), (0xC2, 0x80, 0x44)),
    "grey": ((0xCF, 0xD3, 0xD9), (0xA3, 0xA9, 0xB2), (0x76, 0x7C, 0x88)),
    "white": ((0xFF, 0xFF, 0xFF), (0xF6, 0xF0, 0xE8), (0xD8, 0xCE, 0xC2)),
    "orange": ((0xFF, 0xC0, 0x62), (0xF2, 0x96, 0x36), (0xC6, 0x6A, 0x26)),
    "brown": ((0xC4, 0x8E, 0x60), (0x9C, 0x66, 0x3C), (0x70, 0x44, 0x26)),
}
BELLY = ((0xFF, 0xF4, 0xE2), (0xEB, 0xD2, 0xB4))
BELLY_W = ((0xFF, 0xFB, 0xF4), (0xE4, 0xD8, 0xCA))

# the hamster's left half, its pixels: O outline, F fur, B belly (cream), P the ear's pink; mirrored for the right.
# A round head with its ears on it, cheeks, a body like a pear, sitting
HALF = [
    "...OOO.......",   # 0
    "..OFFPO......",   # 1
    ".OFPPPFO.OOOO",   # 2
    ".OFPPPFOOFFFF",   # 3
    ".OFFPFFFFFFFF",   # 4
    "..OFFFFFFFFFF",   # 5
    "..OFFFFFFFFFF",   # 6
    ".OFFFFFFFFFFF",   # 7
    ".OFFFFFFFFFFF",   # 8
    ".OFFFFFFFFFFF",   # 9   the eyes, 9..11
    ".OFFFFFFFFFFF",   # 10
    ".OFFFFFFFFFBB",   # 11  the muzzle: round, cream, round the nose and the mouth
    "OFFFFFFFFFBBB",   # 12  the nose
    "OFFFFFFFFBBBB",   # 13  the mouth
    "OFFFFFFFBBBBB",   # 14
    "OFFFFFFBBBBBB",   # 15
    "OFFFFFBBBBBBB",   # 16  the paws
    "OFFFFFBBBBBBB",   # 17
    "OFFFFFBBBBBBB",   # 18
    "OFFFFFBBBBBBB",   # 19
    "OFFFFFBBBBBBB",   # 20
    "OFFFFFFBBBBBB",   # 21
    ".OFFFFFBBBBBB",   # 22
    ".OFFFFFFBBBBB",   # 23
    "..OOFFFFFFBBB",   # 24
    "....OOOOOOOOO",   # 25
    ".............",   # 26  (the feet)
]
W0, H = 26, 27                                  # a hamster's own grid
W = 30                                          # every hamster's place: the chubby one's width, the others in its middle

# the chubby one (the eighth, in sunglasses): as tall as the others, its ears as theirs, but wider, round, its cheeks
# and its belly full; its left half (15), mirrored
HALF_CHUBBY = [
    "....OOO........",   # 0   the ears as the others', a little further out on its wider head
    "...OFFPO.......",   # 1
    "..OFPPPFO.OOOOO",   # 2
    "..OFPPPFOOFFFFF",   # 3
    "..OFFPFFFFFFFFF",   # 4
    "..OFFFFFFFFFFFF",   # 5
    ".OFFFFFFFFFFFFF",   # 6
    ".OFFFFFFFFFFFFF",   # 7
    ".OFFFFFFFFFFFFF",   # 8   the eyes (the glasses)
    ".OFFFFFFFFFFFFF",   # 9
    ".OFFFFFFFFFFFFF",   # 10
    "OFFFFFFFFFFFFBB",   # 11  as round as a ball: widest from here
    "OFFFFFFFFFFFBBB",   # 12  the nose; the cheeks full
    "OFFFFFFFFFFBBBB",   # 13  the mouth
    "OFFFFFFFFFBBBBB",   # 14
    "OFFFFFFFFBBBBBB",   # 15
    "OFFFFFFFBBBBBBB",   # 16
    "OFFFFFFBBBBBBBB",   # 17  a big belly
    "OFFFFFFBBBBBBBB",   # 18
    "OFFFFFFBBBBBBBB",   # 19
    "OFFFFFFBBBBBBBB",   # 20
    ".OFFFFFBBBBBBBB",   # 21
    ".OFFFFFFBBBBBBB",   # 22
    "..OFFFFFFBBBBBB",   # 23
    "...OOFFFFFFBBBB",   # 24
    ".....OOOOOOOOOO",   # 25
    "...............",   # 26  (the feet)
]

# the eight: C D E F G A B C, as the page has them
KINDS = [
    {"fur": "golden", "patches": True},
    {"fur": "grey", "blaze": True, "seed": True},
    {"fur": "white", "wink": True, "spot": True},
    {"fur": "orange", "cheese": True},
    {"fur": "brown", "blaze": True},
    {"fur": "orange", "blaze": True, "tongue": True},
    {"fur": "grey", "blaze": True, "happy": True},
    {"fur": "white", "shades": True, "chubby": True},
]


def stamp(g, art, x0, y0, cmap):
    """art: rows of characters; a character in cmap is a pixel of that (a key), '.' or one not in it: left alone"""
    for y, row in enumerate(art):
        for x, c in enumerate(row):
            if c in cmap and 0 <= y0 + y < len(g) and 0 <= x0 + x < len(g[0]):
                g[y0 + y][x0 + x] = cmap[c]


def body(k, rows=H):
    """the hamster's grid of keys: O F B P (and H S b after the shading), before its face"""
    g = [list(h + h[::-1]) for h in (HALF_CHUBBY if k.get("chubby") else HALF)[:rows]]
    if k.get("blaze"):                          # a cream stripe from the forehead down to the muzzle
        for y, (a, b) in {3: (12, 13), 4: (12, 13), 5: (12, 13), 6: (11, 14), 7: (11, 14), 8: (11, 14),
                          9: (11, 14), 10: (11, 14)}.items():
            if y < rows:
                for x in range(a, b + 1):
                    g[y][x] = "B"
    if k.get("patches"):                        # the golden one: a darker cap over its head and down its back
        for y in range(2, 9):
            for x in range(26):
                inside = abs(x - 12.5) < (6.5 if y < 4 else 7.5 + (y - 4) * 1.4) and y < 7 + (abs(x - 12.5) > 6)
                if y < rows and g[y][x] == "F" and inside:
                    g[y][x] = "D"
    if k.get("spot"):                           # the white one's ginger spot on its side
        stamp(g, ["..GG", ".GGGG", "GGGGG", ".GGG"], 20, 15, {"G": "G"})
    return g


def shade(g):
    """light from the upper left: fur against the outline at its right or below in shade, at its left or above
    (upper left) in light; the belly against the outline at its right or below in shade"""
    h, w = len(g), len(g[0])
    out = [r[:] for r in g]
    edge = lambda x, y: x < 0 or y < 0 or x >= w or y >= h or g[y][x] in "O."
    for y in range(h):
        for x in range(w):
            c = g[y][x]
            if c in "FDG":
                if edge(x + 1, y) or edge(x, y + 1):
                    out[y][x] = c.lower() if c != "F" else "S"
                elif (edge(x - 1, y) or edge(x, y - 1)) and x < w // 2 and y < h // 2:
                    out[y][x] = "H" if c == "F" else c
            elif c == "B" and (edge(x + 1, y) or edge(x, y + 1)):
                out[y][x] = "b"
    return out


# its mouth, by pose: at rest a w (or open, the tongue out); singing, open; a sharp (higher, brighter), wide open
MOUTH_REST = ["O.OO.O", ".O..O."]
MOUTH_TONGUE = ["O.OO.O", ".OMMO.", ".OTTO.", "..OO.."]
MOUTH_SING = ["O.OO.O", ".OMMO.", "..OO.."]
MOUTH_SING_TONGUE = ["O.OO.O", "OMMMMO", "OMTTMO", ".OOOO."]
MOUTH_HIGH = [".OOOO.", "OMMMMO", "OMTTMO", ".OOOO."]
MOUTH_HIGH3 = [".OOOO.", "OMTTMO", ".OOOO."]          # (over what it holds: a row less)


def face(g, k, pose="rest"):
    """the eyes, the nose, the mouth, the cheeks, the paws, the feet; and what it holds or wears. pose: "rest", "sing"
    (its mouth open), "sharp" (singing higher: its eyes squeezed shut, > <, its mouth wide; its paws where they are,
    for some hold their food)"""
    dx = (len(g[0]) - W0) // 2                  # (the chubby one is wider: its face in its middle)
    put = lambda art, x0, y0, cmap: stamp(g, art, x0 + dx, y0, cmap)
    held = k.get("seed") or k.get("cheese")
    lo = 1 if held and pose != "rest" else 0    # (what it holds a row lower: its mouth open over it)
    eye = ["WE", "EE", "EE"]
    shut = [".OO.", "O..O"]                     # an eye shut, smiling
    if pose == "sharp":                         # a high note: both eyes squeezed shut, > <
        put(["OO.", "..O", "OO."], 5, 9, {"O": "O"})
        put([".OO", "O..", ".OO"], 18, 9, {"O": "O"})
    elif k.get("happy"):
        put(shut, 5, 10, {"O": "O"})
        put(shut, 17, 10, {"O": "O"})
    else:
        put(eye, 6, 9, {"E": "E", "W": "W"})
        if k.get("wink"):
            put(shut, 17, 10, {"O": "O"})
        else:
            put(eye, 18, 9, {"E": "E", "W": "W"})
    cheek = ["KK", "KK"] if k.get("chubby") else ["KK"]   # the cheeks (the chubby one's fuller)
    put(cheek, 3, 12, {"K": "K"})
    put(cheek, 21, 12, {"K": "K"})
    put(["NN"], 12, 12, {"N": "N"})        # the nose
    if pose == "sharp":
        mouth = MOUTH_HIGH3 if held else MOUTH_HIGH
    elif pose == "sing":
        mouth = MOUTH_SING_TONGUE if k.get("tongue") else MOUTH_SING
    else:
        mouth = MOUTH_TONGUE if k.get("tongue") else MOUTH_REST
    put(mouth, 10, 13, {"O": "O", "M": "M", "T": "T"})
    if k.get("shades"):                         # sunglasses over both eyes
        put(["OOOOOOOOOOOOOOOOOOOO",
                  ".OZZZZZOOOOOOZZZZZO.",
                  ".OZLZZZO....OZLZZZO.",
                  "..OZZZO......OZZZO..",
                  "...OOO........OOO..."], 3, 8, {"O": "O", "Z": "Z", "L": "L"})
    if k.get("seed"):                           # a sunflower seed, held up
        put(["..OO..",
                  ".OZZO.",
                  "OZLZZO",
                  "OZLZZO",
                  "OZLZZO",
                  "OZLZZO",
                  ".OZZO.",
                  "..OO.."], 10, 16 + lo, {"O": "O", "Z": "Z", "L": "L"})
    if k.get("cheese"):                         # a wedge of cheese
        put(["....OO..",
                  "..OOYYO.",
                  "OOYYYYYO",
                  "OYYCYYYO",
                  "OYYYYYCO",
                  "OYCYYYYO",
                  "OyyyyyyO",
                  ".OOOOOO."], 9, 16 + lo, {"O": "O", "Y": "Y", "y": "y", "C": "C"})
    if held:                                    # its paws on what it holds
        y = 19 + lo
        put(["pp", "qq"], 8 if k.get("cheese") else 9, y, {"p": "p", "q": "q"})
        put(["pp", "qq"], 16 if k.get("cheese") else 15, y, {"p": "p", "q": "q"})
    else:                                       # its paws in front of it, soft
        put(["pp", "qq"], 8, 17, {"p": "p", "q": "q"})
        put(["pp", "qq"], 16, 17, {"p": "p", "q": "q"})
    if len(g) > 26:                             # the feet, under it
        for x0 in (5, 18):
            put(["OpppO", ".OOO."], x0 - 1, 25, {"O": "O", "p": "p"})
    return g


def colour(g, k):
    f = FUR[k["fur"]]
    belly = BELLY_W if k["fur"] == "white" else BELLY
    dark = {"golden": ((0xD8, 0x92, 0x4E), (0xB0, 0x6C, 0x34)), "white": ((0xF6, 0xF0, 0xE8), (0xD8, 0xCE, 0xC2))}
    d = dark.get(k["fur"], (f[1], f[2]))
    pal = {"O": INK, "H": f[0], "F": f[1], "S": f[2], "B": belly[0], "b": belly[1], "P": EAR, "E": EYE, "W": GLINT,
           "N": NOSE, "K": BLUSH, "p": PAW, "q": PAW_S, "M": MOUTH, "T": TONGUE, "D": d[0], "d": d[1],
           "G": (0xF2, 0xB0, 0x68), "g": (0xD8, 0x8E, 0x4A),
           "Z": (0x3A, 0x3E, 0x48), "L": (0xEE, 0xEE, 0xF2), "Y": (0xFF, 0xD7, 0x4B), "y": (0xE0, 0xA8, 0x2A),
           "C": (0xE8, 0xB0, 0x30)}
    return [[None if c == "." else pal[c] for c in row] for row in g]


FH = H + 1                                      # a pose's rows: one more, for its ears up


def hamster(k, pose=None):
    """a hamster at rest, its pixels W x H; or in a pose, W x FH, its feet on the last row: "rest"; "sing" (its mouth
    open, a row lower: it ducks); "sharp" (singing higher: it ducks as well, its ears up a row, its eyes squeezed, its
    mouth wide);
    "flat" (THALL's hard press: three rows lower, as the page squashes it). Rows are taken out where it is one colour
    across, and added where its ears are: every outline stays two screen pixels thick"""
    g = face(shade(body(k)), k, "sing" if pose == "flat" else pose or "rest")
    pad = (W - len(g[0])) // 2                  # (in the middle of its place, W wide)
    g = [["."] * pad + r + ["."] * pad for r in g]
    if pose is None:
        return colour(g, k)
    drop = {"sing": {22}, "sharp": {22}, "flat": {6, 21, 22}}.get(pose, set())
    g = [r for i, r in enumerate(g) if i not in drop]
    if pose == "sharp":                         # its ears up: a row more of them
        e = 1
        g = g[:e] + [g[e][:]] + g[e:]
    g = [["."] * W for _ in range(FH - len(g))] + g
    return colour(g, k)


POSES = ("rest", "sing", "sharp", "flat")


def hamsters():
    return [hamster(k) for k in KINDS]


def hamster_poses():
    """the eight, each in its four poses (POSES)"""
    return [[hamster(k, p) for p in POSES] for k in KINDS]


# ---- the drummer: an orange hamster behind its kit, 56 x 45

DW, DH = 56, 45


def blank(w, h):
    return [[None] * w for _ in range(h)]


def layer(dst, src, x0, y0):
    for y, row in enumerate(src):
        for x, c in enumerate(row):
            if c is not None and 0 <= y0 + y < len(dst) and 0 <= x0 + x < len(dst[0]):
                dst[y0 + y][x0 + x] = c


def outlined(mask_fill, w, h):
    """a shape: fill(x, y) -> colour or None; its own outline round it (inside its edge)"""
    g = blank(w, h)
    m = [[mask_fill(x, y) for x in range(w)] for y in range(h)]
    for y in range(h):
        for x in range(w):
            if m[y][x] is None:
                continue
            edge = any(not (0 <= x + dx < w and 0 <= y + dy < h) or m[y + dy][x + dx] is None
                       for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
            g[y][x] = INK if edge else m[y][x]
    return g


def line(g, x0, y0, x1, y1, c, ink=None):
    """a line; with ink, an outline on both of its sides"""
    pts = []
    dx, dy = abs(x1 - x0), -abs(y1 - y0)
    sx, sy = (1 if x0 < x1 else -1), (1 if y0 < y1 else -1)
    err = dx + dy
    while True:
        pts.append((x0, y0))
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 >= dy:
            err += dy
            x0 += sx
        if e2 <= dx:
            err += dx
            y0 += sy
    if ink:
        for x, y in pts:
            for ox, oy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                if 0 <= y + oy < len(g) and 0 <= x + ox < len(g[0]) and (x + ox, y + oy) not in pts:
                    g[y + oy][x + ox] = ink
    for x, y in pts:
        if 0 <= y < len(g) and 0 <= x < len(g[0]):
            g[y][x] = c


GREY = ((0xE2, 0xE4, 0xE8), (0xB0, 0xB4, 0xBC), (0x7C, 0x82, 0x8E))
RED = ((0xF4, 0x8A, 0x84), (0xDE, 0x5A, 0x5E), (0xB0, 0x3C, 0x46))
GOLD = ((0xFF, 0xE8, 0x8C), (0xF6, 0xC4, 0x48), (0xC8, 0x90, 0x28))
STICK = (0xF0, 0xD8, 0xA8)
SPARK = (0xFF, 0xE0, 0x50)


def kit_back():
    """behind the hamster: the hi-hat's stand"""
    g = blank(DW, DH)
    line(g, 50, 25, 50, 41, GREY[1], INK)       # the hi-hat's stand, on three legs
    line(g, 50, 39, 46, 43, GREY[2], INK)
    line(g, 50, 39, 54, 43, GREY[2], INK)
    line(g, 7, 33, 7, 41, GREY[2], INK)         # the snare's
    line(g, 7, 39, 3, 43, GREY[2], INK)
    line(g, 7, 39, 11, 43, GREY[2], INK)
    return g


def kit_front(thump=False):
    """in front of the hamster: the snare (left), the bass drum with its paw (thump: lit, the kick's), the hi-hat
    (right): one for each of the Tromster's rows, the clap its paws"""
    g = blank(DW, DH)
    snare = outlined(lambda x, y: (None if (y == 0 and (x == 0 or x == 12)) else
                                   (0xFF, 0xF8, 0xEE) if y < 2 else GREY[0] if x < 3 else GREY[2] if x > 9 else GREY[1]),
                     13, 7)
    for x in (2, 5, 8, 11):                      # its lugs
        snare[4][x] = INK
    for x in range(1, 12):                       # its rims
        snare[2][x] = GREY[2]
    layer(g, snare, 1, 26)
    r = 10.5                                    # the bass drum: its rim, its cream head
    drum = outlined(lambda x, y: (None if (x - 10.5) ** 2 + (y - 10.5) ** 2 > r * r else
                                  GREY[1] if (x - 10.5) ** 2 + (y - 10.5) ** 2 > 8.6 ** 2 else
                                  (0xFF, 0xF4, 0xE2) if (x - 9.5) + (y - 9.5) < 8 else (0xF2, 0xE2, 0xCC)), 22, 22)
    for y in range(22):                         # the rim's ring: an ink line inside it
        for x in range(22):
            d = (x - 10.5) ** 2 + (y - 10.5) ** 2
            if 7.9 ** 2 < d <= 8.6 ** 2:
                drum[y][x] = GREY[2]
    stamp(drum, [".P....P.",                # a paw print: four toes over its pad
                 "...PP...",
                 "P......P",
                 "..PPPP..",
                 ".PPPPPP.",
                 ".PPPPPP.",
                 "..PPPP.."], 7, 6, {"P": SPARK if thump else RED[1]})
    layer(g, drum, 17, 23)
    for y0 in (22, 25):                         # the hi-hat: two cymbals, a little apart
        cym = outlined(lambda x, y: (GOLD[0] if y < 1 else GOLD[1] if y < 2 else GOLD[2])
                       if (x - 5.5) ** 2 / 36 + (y - 1) ** 2 / 2.3 <= 1 else None, 12, 3)
        layer(g, cym, 44, y0)
    layer(g, [[INK], [INK]], 50, 20)            # its nut
    return g


def arm(g, shoulder, elbow, paw, stick_end, body=None):
    """an arm from its shoulder by its elbow to its paw, round, the paw round its stick (a bead at its tip): the stick
    first, the arm over it. No outline where it leaves the body (body: the hamster's pixels, near its shoulder), so it
    grows out of it"""
    line(g, paw[0], paw[1], stick_end[0], stick_end[1], STICK, INK)
    tx, ty = stick_end
    for ox, oy in ((0, 0), (1, 0), (0, 1), (1, 1)):          # the bead
        if 0 <= ty + oy < len(g) and 0 <= tx + ox < len(g[0]):
            g[ty + oy][tx + ox] = STICK

    def dist(x, y, a, b):                       # from (x, y) to the segment a..b
        (ax, ay), (bx, by) = a, b
        vx, vy, wx, wy = bx - ax, by - ay, x - ax, y - ay
        t = max(0.0, min(1.0, (wx * vx + wy * vy) / float(vx * vx + vy * vy or 1)))
        return ((x - ax - t * vx) ** 2 + (y - ay - t * vy) ** 2) ** .5

    def fill(x, y):
        if (x - paw[0]) ** 2 + (y - paw[1]) ** 2 <= 2.3:
            return PAW_S if y > paw[1] else PAW
        d = min(dist(x, y, shoulder, elbow), dist(x, y, elbow, paw))
        if d <= 1.75:
            up = dist(x, y - 1, shoulder, elbow) > d or dist(x, y - 1, elbow, paw) > d   # (its upper side: light,
            on_body = body and body[y][x]                                              # off the body: over it, the
            return FUR["orange"][0] if up and d > .6 and not on_body else FUR["orange"][1]   # body's own fur)
        return None
    f = [[fill(x, y) for x in range(DW)] for y in range(DH)]
    for y in range(DH):
        for x in range(DW):
            if f[y][x]:
                g[y][x] = f[y][x]
            elif not (body and body[y][x] and (x - shoulder[0]) ** 2 + (y - shoulder[1]) ** 2 < 16) and \
                    any(0 <= y + oy < DH and 0 <= x + ox < DW and f[y + oy][x + ox]
                                                   for ox, oy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                g[y][x] = INK


def drummer_hamster():
    k = {"fur": "orange", "tongue": True}
    g = shade(body(k, 24))
    g = face(g, k)
    return colour(g, k)


def sparks(g, x, y, side):
    """three short strokes, out from where the stick meets the cymbal"""
    for dx, dy in ((0, -1), (0, -2), (2, 0), (3, -1), (2, 2), (3, 2)):
        xx, yy = x + dx * side, y + dy
        if 0 <= xx < len(g[0]) and 0 <= yy < len(g):
            g[yy][xx] = SPARK


def drummer():
    """the drummer's frames, one for each set of drums that hit at once: frame snare | hat << 1 | kick << 2 (0: ready,
    the sticks up a little), 8 + kick: the clap (both arms high, the sticks sparking). The snare: the left stick on the
    snare; the hat: the right one on the hi-hat; the kick: down it goes, the bass drum's paw lit"""
    READY_L = ((18, 20), (14, 21), (12, 18), (7, 12))
    SNARE_L = ((18, 20), (14, 22), (12, 22), (7, 26))
    READY_R = ((37, 20), (40, 22), (42, 18), (47, 12))
    HAT_R = ((37, 20), (40, 21), (41, 19), (45, 22))
    CLAP_L = ((18, 20), (13, 16), (13, 11), (9, 3))
    CLAP_R = ((37, 20), (42, 16), (42, 11), (46, 3))
    frames = []
    ham = drummer_hamster()
    for n in range(10):
        clap, kick = n >= 8, (n - 8 if n >= 8 else n >> 2) & 1
        snare, hat = not clap and n & 1, not clap and n >> 1 & 1
        g = blank(DW, DH)
        layer(g, kit_back(), 0, 0)
        hy = 7 if kick else 6
        layer(g, ham, 15, hy)
        front = kit_front(kick)
        layer(g, front, 0, 0)
        body = [[0 <= y - hy < len(ham) and 0 <= x - 15 < len(ham[0]) and ham[y - hy][x - 15] is not None and
                 not front[y][x] for x in range(DW)] for y in range(DH)]   # (the hamster, where it shows)
        arm(g, *(CLAP_L if clap else SNARE_L if snare else READY_L), body)
        arm(g, *(CLAP_R if clap else HAT_R if hat else READY_R), body)
        if snare:                               # where a stick meets: a flash
            sparks(g, 6, 25, -1)
        if hat:
            sparks(g, 46, 21, 1)
        if clap:                                # (both high: the sticks spark)
            sparks(g, 8, 2, -1)
            sparks(g, 47, 2, 1)
        frames.append(g)
    return frames