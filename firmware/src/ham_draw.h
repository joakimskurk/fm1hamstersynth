/* SPDX-License-Identifier: GPL-3.0-only */
/* HamsterSynth's screen: the page's look on 240 x 240. Everything is drawn into one picture in RAM (fb, the pool)
 * and the rows that changed are sent (fb_flush): nothing flickers, and a hamster can hop without a thought for what
 * is behind it.
 *
 *   y   0 ..  23   the logo: HAMSTERSYNTH in the page's pixel letters (HAMSTERCORE on black in THALL), its notes
 *   y  24 .. 143   the eight hamsters, four a row, on a soft sage floor, each its shadow under it (or the Tromster,
 *                  the song, the help, the settings); no frames, no letters: the notes are in the steps
 *   y 144 .. 207   the 16 steps, eight a row in two beats of four: each a tile in its hamster's colour
 *   y 208 .. 223   the patterns A-H (each its colour; the empty dim), the tempo, what it is doing
 *   y 224 .. 239   what was just done; else a hint for what one is doing
 * The sign (the hamster's wisdom, THALL's) is a speech bubble from one of the hamsters.
 * Two fonts: the big one (5 x 7, 2 x) for what matters, the small one (3 x 5, 2 x) for what is said beside it. */
#include "ham_art.h"

#define C_PAPER RGB(0xE8, 0xDF, 0xCB)
#define C_CREAM RGB(0xF7, 0xEC, 0xD7)
#define C_CREAM2 RGB(0xFF, 0xF3, 0xDD)
#define C_LABEL RGB(0xF1, 0xE7, 0xD2)
#define C_INK RGB(0x09, 0x1A, 0x25)
#define C_INK2 RGB(0x17, 0x31, 0x3F)
#define C_BLUE RGB(0x42, 0x6B, 0x7C)
#define C_BLUE3 RGB(0x29, 0x46, 0x53)
#define C_CELL0 RGB(0x1D, 0x3A, 0x48)
#define C_EDGE RGB(0x5F, 0x78, 0x83)
#define C_PINK RGB(0xF1, 0x8E, 0x8C)
#define C_PINK2 RGB(0xD9, 0x5F, 0x67)
#define C_YELLOW RGB(0xFF, 0xD7, 0x4B)
#define C_ORANGE RGB(0xF0, 0xA8, 0x51)
#define C_SIGN RGB(0xD9, 0x9B, 0x58)
#define C_MUTED RGB(0xC7, 0xCF, 0xCC)
#define C_WHITE2 RGB(0xFF, 0xF6, 0xE8)
#define C_SKY RGB(0x9A, 0xD0, 0xE8)
#define C_GREEN RGB(0xA8, 0xD5, 0x8A)
#define C_STEP0 RGB(0x1D, 0x3A, 0x48)               /* a step: empty, empty on a beat, with a note, on a beat */
#define C_STEP0B RGB(0x23, 0x44, 0x53)
#define C_STEP1 RGB(0x2C, 0x4B, 0x59)
#define C_STEP1B RGB(0x34, 0x56, 0x65)
static const uint16_t PAT_COL[8] = {        /* the patterns A..H, everywhere */
    C_YELLOW, C_PINK, C_SKY, C_GREEN, C_ORANGE, RGB(0xC3, 0xA6, 0xE6), RGB(0x7F, 0xD6, 0xC2), RGB(0xC8, 0xCF, 0xDA)};

/* the stage and the steps: soft colours (THALL: dark) */
#define C_STAGE RGB(0xB8, 0xCB, 0xC6)               /* the hamsters' floor, a dusty sage */
#define C_SHADOW RGB(0xA3, 0xB8, 0xB3)              /* a hamster's shadow on it */
#define C_SPOT RGB(0xEA, 0xDE, 0xC4)                /* the chosen one's: warm */
#define C_SPOT_SING RGB(0xF2, 0xA7, 0xA2)           /* the one that sings: pink */
#define C_STAGE_D RGB(0x2D, 0x33, 0x39)
#define C_SHADOW_D RGB(0x22, 0x27, 0x2C)
#define C_SEQ RGB(0x3D, 0x58, 0x5E)                 /* under the steps */
#define C_SEQ_D RGB(0x1E, 0x23, 0x28)
#define C_SEQ_DOT RGB(0x58, 0x76, 0x7C)             /* a rest; on a beat brighter */
#define C_SEQ_BEAT RGB(0x76, 0x96, 0x9B)
static uint32_t scr_dark;                       /* THALL: the stage and the steps dark */

#define LOGO_H 24
#define Y_HAM 24
#define HAM_ROW_H 60
#define Y_STEPS 144
#define Y_BOT 208

static uint16_t hfb[240u * 240u] __attribute__((section(".pool")));   /* as the screen takes it: bytes swapped */
static uint8_t fb_dirty[15];                                        /* bands of 16 rows */

static void fb_mark(int32_t y, int32_t h)
{
    int32_t b;
    if (y < 0) { h += y; y = 0; }
    if (y + h > 240) h = 240 - y;
    for (b = y / 16; h > 0 && b <= (y + h - 1) / 16; b++)
        fb_dirty[b] = 1;
}
static void fb_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)
{
    int32_t x1 = x + w, y1 = y + h, i, j;
    uint16_t s = swap16(c);
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x1 > 240) x1 = 240;
    if (y1 > 240) y1 = 240;
    if (x >= x1 || y >= y1)
        return;
    for (j = y; j < y1; j++)
        for (i = x; i < x1; i++)
            hfb[j * 240 + i] = s;
    fb_mark(y, y1 - y);
}
static inline void fb_px(int32_t x, int32_t y, uint16_t c)   /* (the caller marks) */
{
    if ((uint32_t)x < 240u && (uint32_t)y < 240u)
        hfb[y * 240 + x] = swap16(c);
}
static void fb_frame(int32_t x, int32_t y, int32_t w, int32_t h, int32_t t, uint16_t c)   /* a border, t thick */
{
    fb_rect(x, y, w, t, c);
    fb_rect(x, y + h - t, w, t, c);
    fb_rect(x, y, t, h, c);
    fb_rect(x + w - t, y, t, h, c);
}

/* the text: UTF-8, the 5 x 7 font, sc pixels a pixel; what the font has not shows as '?' */
static uint32_t pf_next(const char **s)
{
    const uint8_t *p = (const uint8_t *)*s;
    uint32_t c = *p++;
    if (c >= 0xF0u && p[0] && p[1] && p[2]) { c = (c & 7u) << 18 | (p[0] & 63u) << 12 | (p[1] & 63u) << 6 | (p[2] & 63u); p += 3; }
    else if (c >= 0xE0u && p[0] && p[1]) { c = (c & 15u) << 12 | (p[0] & 63u) << 6 | (p[1] & 63u); p += 2; }
    else if (c >= 0xC0u && p[0]) { c = (c & 31u) << 6 | (p[0] & 63u); p += 1; }
    *s = (const char *)p;
    return c;
}
static uint32_t pf_glyph(uint32_t c)
{
    uint32_t i;
    if (c >= 'a' && c <= 'z') c -= 32u;
    if (c == 0xE6u) c = 0xC6u;                  /* ae oe aa: the capitals */
    if (c == 0xF8u) c = 0xD8u;
    if (c == 0xE5u) c = 0xC5u;
    for (i = 0; i < PF_N; i++)
        if (PF_CODE[i] == c)
            return i;
    return pf_glyph('?');
}
static int32_t text_px(const char *s, int32_t sc)
{
    int32_t n = 0;
    while (*s) { pf_next(&s); n++; }
    return n ? n * 6 * sc - sc : 0;
}
static int32_t fb_text(int32_t x, int32_t y, const char *s, uint16_t c, int32_t sc)
{
    while (*s) {
        const uint8_t *g = PF_GLYPH[pf_glyph(pf_next(&s))];
        int32_t r, k, i, j;
        for (r = 0; r < 7; r++)
            for (k = 0; k < 5; k++)
                if (g[r] >> (4 - k) & 1u)
                    for (j = 0; j < sc; j++)
                        for (i = 0; i < sc; i++)
                            fb_px(x + k * sc + i, y + r * sc + j, c);
        x += 6 * sc;
    }
    fb_mark(y, 7 * sc);
    return x;
}
static void fb_text_mid(int32_t x, int32_t w, int32_t y, const char *s, uint16_t c, int32_t sc)
{
    fb_text(x + (w - text_px(s, sc)) / 2, y, s, c, sc);
}

/* the small font: 5 rows, proportional; sc pixels a pixel */
static uint32_t sf_glyph(uint32_t c)
{
    uint32_t i;
    if (c >= 'a' && c <= 'z') c -= 32u;
    if (c == 0xE6u) c = 0xC6u;
    if (c == 0xF8u) c = 0xD8u;
    if (c == 0xE5u) c = 0xC5u;
    for (i = 0; i < SF_N; i++)
        if (SF_CODE[i] == c)
            return i;
    return sf_glyph('?');
}
static int32_t sf_px(const char *s, int32_t sc)
{
    int32_t n = 0;
    while (*s)
        n += ((int32_t)SF_W[sf_glyph(pf_next(&s))] + 1) * sc;
    return n ? n - sc : 0;
}
static int32_t sf_text(int32_t x, int32_t y, const char *s, uint16_t c, int32_t sc)
{
    while (*s) {
        uint32_t gi = sf_glyph(pf_next(&s)), w = SF_W[gi];
        int32_t r, k;
        for (r = 0; r < 5; r++)
            for (k = 0; k < (int32_t)w; k++)
                if (SF_GLYPH[gi][r] >> (w - 1u - (uint32_t)k) & 1u)
                    fb_rect(x + k * sc, y + r * sc, sc, sc, c);
        x += ((int32_t)w + 1) * sc;
    }
    return x;
}
static void sf_text_mid(int32_t x, int32_t w, int32_t y, const char *s, uint16_t c, int32_t sc)
{
    sf_text(x + (w - sf_px(s, sc)) / 2, y, s, c, sc);
}
/* a pill: a box with its corners cut (2 pixels: it looks drawn 2 x 2, as the rest) */
static void fb_pill(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)
{
    fb_rect(x + 2, y, w - 4, h, c);
    fb_rect(x, y + 2, 2, h - 4, c);
    fb_rect(x + w - 2, y + 2, 2, h - 4, c);
}
static void fb_pill_line(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t c)   /* its outline, 2 thick */
{
    fb_rect(x + 2, y, w - 4, 2, c);
    fb_rect(x + 2, y + h - 2, w - 4, 2, c);
    fb_rect(x, y + 2, 2, h - 4, c);
    fb_rect(x + w - 2, y + 2, 2, h - 4, c);
}

/* the rows that changed, to the screen: full rows, up to 64 at a time */
static void fb_flush(void)
{
    uint32_t b = 0;
    while (b < 15u) {
        uint32_t n = 0;
        if (!fb_dirty[b]) { b++; continue; }
        while (b + n < 15u && n < 4u && fb_dirty[b + n]) {
            fb_dirty[b + n] = 0;
            n++;
        }
        lcd_sync();
        lcd_blit(0, b * 16u, 240, n * 16u, hfb + b * 16u * 240u);
        b += n;
    }
    lcd_sync();
}

/* ---- the logo: the page's, two pixels a pixel. dance: its notes a pixel off, on the beat (0: at rest, 1, 2) */
static void lg_px(int32_t x, int32_t y, uint16_t c) { fb_rect(21 + 2 * x, 1 + 2 * y, 2, 2, c); }
static uint16_t dim565(uint16_t c)              /* THALL: the logo darker (the page: brightness 0.78) */
{
    uint32_t r = c >> 11, g = c >> 5 & 63u, b = c & 31u;
    return (uint16_t)((r * 25u / 32u) << 11 | (g * 25u / 32u) << 5 | (b * 25u / 32u));
}
static void draw_logo(uint32_t core, uint32_t dance)
{
    static const int8_t NOTE[9][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {1, 3}, {2, 4}, {2, 5}, {1, 5}, {0, 4}};
    const char *a = "HAMSTER", *b = core ? "CORE" : "SYNTH", *p;
    int32_t x = core ? 5 : 1, pass, i, j, word, lx, rx, ly, ry;   /* (one word, as HAMSTERCORE) */
    uint16_t pink = core ? dim565(C_PINK) : C_PINK;
    if (core)                                   /* black metal: 2a2d2f down to 050607 */
        for (j = 0; j < LOGO_H; j++) {
            uint32_t v = j < 12 ? 42u - (uint32_t)j * 2u : 18u - (uint32_t)(j - 12);
            fb_rect(0, j, 240, 1, RGB(v, v + 2u, v + 4u));
        }
    else
        fb_rect(0, 0, 240, LOGO_H, C_BLUE);
    fb_frame(0, 0, 240, LOGO_H, 1, C_INK);
    for (pass = 0; pass < 3; pass++) {          /* the outline, the letters, their light */
        int32_t gx = x;
        for (word = 0; word < 2; word++) {
            uint16_t col = word ? C_WHITE2 : C_ORANGE, hi = word ? RGB(0xFF, 0xF9, 0xEC) : RGB(0xFF, 0xC2, 0x67);
            if (core) { col = dim565(col); hi = dim565(hi); }
            for (p = word ? b : a; *p; p++) {
                const char *q = LOGO_CHARS;
                uint32_t gi = 0;
                while (q[gi] && q[gi] != *p) gi++;
                for (j = 0; j < 9; j++)
                    for (i = 0; i < 7; i++) {
                        if (!(LOGO_FONT[gi][j] >> (6 - i) & 1u)) continue;
                        if (pass == 0) {
                            int32_t ox, oy;
                            for (oy = -1; oy <= 1; oy++)
                                for (ox = -1; ox <= 1; ox++)
                                    lg_px(gx + i + ox, 1 + j + oy, C_INK);
                        } else if (pass == 1) {
                            lg_px(gx + i, 1 + j, col);
                        } else if (j == 0 || i == 0) {
                            lg_px(gx + i, 1 + j, hi);
                        }
                    }
                gx += 8;
            }
        }
    }
    lx = dance == 1 ? 0 : dance == 2 ? 1 : 0;   /* the notes: the page's little dance, one pixel */
    ly = dance == 1 ? -1 : dance == 2 ? 1 : 0;
    rx = dance == 1 ? -1 : 0;
    ry = dance == 1 ? 1 : dance == 2 ? -1 : 0;
    for (i = 0; i < 9; i++) {
        fb_rect(6 + 2 * (NOTE[i][0] + lx), 5 + 2 * (NOTE[i][1] + ly), 2, 2, pink);
        fb_rect(228 + 2 * (2 - NOTE[i][0] + rx), 5 + 2 * (NOTE[i][1] + ry), 2, 2, pink);
    }
}

/* ---- a hamster on the stage: its place 60 x 60, four a row, no frame, no letter. state: 0 at rest, 1 chosen (its
 * shadow warm), 2 singing (its shadow pink). pose (firmware/gen/ham_art.h, drawn in its own pixels: every outline two
 * screen pixels): HP_REST; HP_SING, a note (its mouth open, a row lower: it ducks); HP_SHARP, a sharp, sung higher and
 * brighter (its ears up, its eyes squeezed shut, its mouth wide, stars in its corners; its paws stay, on its food or
 * its belly); HP_FLAT, THALL's hard press (three rows lower, as the page squashes it) */
static const char *const NOTE_NAME[8] = {"C", "D", "E", "F", "G", "A", "B", "C"};
static void fb_shadow(int32_t cx, int32_t y, int32_t w, uint16_t c)   /* an oval, 6 high: three rows of two */
{
    fb_rect(cx - w * 3 / 8, y, w * 3 / 4, 2, c);
    fb_rect(cx - w / 2, y + 2, w, 2, c);
    fb_rect(cx - w * 3 / 8, y + 4, w * 3 / 4, 2, c);
}
static void fb_sparkle(int32_t x, int32_t y, uint16_t c)   /* a little star, in 2 x 2 pixels */
{
    fb_rect(x + 2, y, 2, 6, c);
    fb_rect(x, y + 2, 6, 2, c);
}
static void draw_ham(uint32_t i, uint32_t state, uint32_t pose)
{
    int32_t cx = (int32_t)(i & 3u) * 60, cy = Y_HAM + (int32_t)(i >> 2) * HAM_ROW_H, x, y;
    int32_t top = cy + 1;                       /* (HAM_FH rows: its feet where the rest's have always been) */
    const uint8_t *sp = HAM_SPR[i][pose & 3u];
    fb_rect(cx, cy, 60, HAM_ROW_H, scr_dark ? C_STAGE_D : C_STAGE);
    if (pose == HP_SHARP) {                     /* (in its place's top corners: clear of it) */
        fb_sparkle(cx, cy, C_YELLOW);
        fb_sparkle(cx + 54, cy, C_YELLOW);
    }
    fb_shadow(cx + 30, cy + 54, 44, state == 2u ? (scr_dark ? C_PINK2 : C_SPOT_SING) :
              state == 1u ? (scr_dark ? C_BLUE3 : C_SPOT) : (scr_dark ? C_SHADOW_D : C_SHADOW));
    for (y = 0; y < (int32_t)HAM_FH; y++)       /* 2 x 2 a pixel, its feet on its shadow */
        for (x = 0; x < (int32_t)HAM_W; x++) {
            uint8_t v = sp[y * (int32_t)HAM_W + x];
            if (v != 255u) {
                uint16_t c = HAM_PAL[v];
                int32_t px = cx + 4 + 2 * x, py = top + 2 * y;
                fb_px(px, py, c);
                fb_px(px + 1, py, c);
                fb_px(px, py + 1, c);
                fb_px(px + 1, py + 1, c);
            }
        }
    fb_mark(cy, HAM_ROW_H);
}

/* ---- the steps: two rows of eight, each two beats of four. A step: a tile, 24 x 26, in its hamster's colour, its foot
 * that colour darker (green: it has its own sound, the knobs recorded). On it its note in big letters, a sharp's ♯,
 * its octave an arrow (and how many); on its foot its effects: glide (blue), arpeggio (yellow, +12 orange), accent
 * (pink), and a tick for each hit of a ratchet. A rest: a dot (on a beat brighter). The cursor: a yellow line round it;
 * the step that plays: a white one */
#define STEP_X(j) (7 + (int32_t)(j) * 28 + ((j) >= 4u ? 2 : 0))
#define STEP_Y(r) (Y_STEPS + 4 + (int32_t)(r) * 30)
static void draw_steps_bg(void) { fb_rect(0, Y_STEPS, 240, Y_BOT - Y_STEPS, scr_dark ? C_SEQ_D : C_SEQ); }
static uint16_t dark565(uint16_t c)              /* a colour, 5/8 as bright */
{
    return (uint16_t)(((((c >> 11) & 31u) * 5u / 8u) << 11) | ((((c >> 5) & 63u) * 5u / 8u) << 5) | ((c & 31u) * 5u / 8u));
}
static uint32_t light565(uint16_t c)             /* light enough for ink on it */
{
    return ((c >> 11) & 31u) * 2u * 8u + ((c >> 5) & 63u) * 5u * 4u + (c & 31u) * 8u > 8u * 150u;
}
#define C_FOOT_SND RGB(0x5E, 0x9A, 0x52)
static void draw_step(uint32_t i, uint32_t s, uint32_t cur, uint32_t playing, uint32_t snd)
{
    int32_t x = STEP_X(i & 7u), y = STEP_Y(i >> 3), beat = (i & 3u) == 0u;
    fb_rect(x, y, 24, 26, scr_dark ? C_SEQ_D : C_SEQ);
    if (s & ST_ON) {
        uint16_t t = HAM_TINT[ST_KEY(s)], ink = light565(t) ? C_INK : C_WHITE2;
        int32_t o = ST_OCT(s), r = (int32_t)ST_RATCHET(s), k;
        fb_pill(x, y, 24, 26, snd ? C_FOOT_SND : dark565(t));   /* the foot, then the body over it */
        fb_rect(x + 2, y, 20, 19, t);
        fb_rect(x, y + 2, 24, 17, t);
        fb_text(x + 3, y + 3, NOTE_NAME[ST_KEY(s)], ink, 2);
        if (ST_SHARP(s))
            sf_text(x + 14, y, "♯", ink, 2);
        if (o) {
            char d[2] = {(char)('0' + (o < 0 ? -o : o)), 0};
            sf_text(x + 14, y + 10, o < 0 ? "↓" : "↑", ink, 2);
            if (o < -1 || o > 1)
                sf_text(x + 20, y + 13, d, ink, 1);
        }
        if (ST_GLIDE(s)) fb_rect(x + 2, y + 20, 4, 3, C_SKY);
        if (ST_ARP(s)) fb_rect(x + 6, y + 20, 4, 3, ST_ARP(s) == 2u ? C_ORANGE : C_YELLOW);
        if (ST_ACCENT(s)) fb_rect(x + 10, y + 20, 4, 3, C_PINK);
        for (k = 0; r > 1 && k < r; k++)        /* a ratchet: its hits */
            fb_rect(x + 15 + k * 2, y + 20, 1, 3, C_WHITE2);
    } else {
        fb_rect(x + 10, y + 11, 4, 4, playing ? C_WHITE2 : snd ? C_FOOT_SND : beat ? C_SEQ_BEAT : C_SEQ_DOT);
    }
    if (playing)
        fb_pill_line(x, y, 24, 26, C_WHITE2);
    else if (cur)
        fb_pill_line(x, y, 24, 26, C_YELLOW);
    fb_mark(y, 26);
}

/* ---- the sign: a speech bubble from hamster `who` (of the lower row) over the upper row; dark: THALL's */
static void draw_bubble(const char *l1, const char *l2, uint32_t who, int dark)
{
    int32_t x = 8, y = 28, w = 224, h = 46, tx = (int32_t)(who & 3u) * 60 + 30, k;
    uint16_t bg = dark ? C_INK : C_CREAM2, fg = dark ? C_PINK : C_INK, edge = dark ? C_PINK2 : C_INK;
    fb_pill(x - 2, y - 2, w + 4, h + 4, edge);
    fb_pill(x, y, w, h, bg);
    for (k = 0; k < 14; k++) {                  /* its tail, down to the hamster's head */
        int32_t half = (14 - k) * 6 / 14;
        fb_rect(tx - half - 2, y + h + k, 2 * half + 4, 1, edge);
        if (half > 0)
            fb_rect(tx - half, y + h - 2 + k, 2 * half, 1, bg);
    }
    if (l2 && l2[0]) {
        fb_text_mid(x, w, y + 6, l1, fg, 2);
        fb_text_mid(x, w, y + 26, l2, fg, 2);
    } else {
        fb_text_mid(x, w, y + 16, l1, fg, 2);
    }
}

/* ---- the Tromster's drummer, a frame of four */
static void draw_drummer(uint32_t frame)
{
    int32_t x, y;
    const uint8_t *sp = DRUM_SPR[frame & 3u];
    fb_rect(2, 27, 118, 100, C_EDGE);            /* its frame, with air round it */
    fb_rect(3, 28, 116, 98, RGB(0x17, 0x34, 0x40));
    for (y = 0; y < (int32_t)DRUM_H; y++)
        for (x = 0; x < (int32_t)DRUM_W; x++) {
            uint8_t v = sp[y * (int32_t)DRUM_W + x];
            if (v != 255u)
                fb_rect(5 + 2 * x, 32 + 2 * y, 2, 2, DRUM_PAL[v]);
        }
}
