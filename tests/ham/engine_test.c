/* SPDX-License-Identifier: GPL-3.0-only */
/* HamsterSynth's engine (firmware/src/ham_engine.h) on the host: what must hold when one plays over what plays.
 *   - THALL: a note by hand does not cut the sequence's note off, nor the sequence's the hand's (two lines)
 *   - all voices busy: two notes begun together both sound (a voice that has just begun is never taken)
 *   - a step asked to be skipped (it was played by hand just before it) plays no note, and only that one
 *   - the sound knobs: as the knobs are (16, LYS 32) nothing changes, bit for bit; a step's own LYS darkens only while
 *     that step sounds; LENGDE lengthens, ROM adds room, PIP changes the squeak
 *   - the sub: a step of it sounds, low (its energy under 150 Hz), one at a time (a second note takes the first's
 *     place, no click: no jump in the sound), at the step the melody's does */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
static unsigned young;
#define HENG_ON_STEAL(h) do { if ((h)->amp.i <= 1u || !(h)->amp.left) young++; } while (0)
#include "ham_engine.h"

static int fails;
static void check(const char *what, int ok) { printf("%s: %s\n", what, ok ? "ok" : "FAIL"); fails += !ok; }
static void run(uint32_t frames)
{
    int32_t buf[512];
    uint32_t f;
    for (f = 0; f < frames; f += 256u) {
        memset(buf, 0, sizeof buf);
        heng_render(buf, 256u);
    }
}
/* a bar of two steps (C, then E), the hamster voice, rendered: out[] the left channel */
#define NOUT 44100u
static int32_t out_a[NOUT], out_b[NOUT], out_c[NOUT];
static void bar(int32_t *out, uint32_t key, uint32_t lock)   /* lock: the first step's own value of knob `key`, or 0 */
{
    int32_t buf[512];
    uint32_t f, i, k;
    heng_init();                                /* (and all else as at the start: the noise, the filters, the room) */
    memset(hv, 0, sizeof hv);
    memset(&nz_bp, 0, sizeof nz_bp);
    memset(&nz_lp, 0, sizeof nz_lp);
    memset(lys_z, 0, sizeof lys_z);
    nz_seed = 0x2545F491u; nz_prev = 0; room_i = 0; eng_age = 0;
    hq.thall = 0; hq.hvoice = 1; hq.bpm = 120; hq.drums_on = 0;
    memset((void *)hq.pat, 0, sizeof hq.pat);
    memset((void *)hq.plock, 0, sizeof hq.plock);
    hq.pat[0][0] = ST_MAKE(0, 0);
    hq.pat[0][1] = ST_MAKE(2, 0);
    if (lock) hq.plock[0][0][key] = (uint8_t)lock;
    heng_cmd(HCMD(HC_PLAY, 0));
    for (f = 0; f < NOUT; f += 256u) {
        memset(buf, 0, sizeof buf);
        heng_render(buf, 256u);
        for (i = 0; i < 256u && f + i < NOUT; i++) out[f + i] = buf[2u * i];
        if (f >= 2u * 11025u) for (k = 0; k < 16u; k++) hq.pat[0][k] = 0;   /* (two steps, then quiet) */
    }
    heng_cmd(HCMD(HC_STOP, 0));
}
static double energy(const int32_t *x, uint32_t a, uint32_t b, int diff)
{
    double e = 0;
    uint32_t i;
    for (i = a + 1u; i < b; i++) { double v = diff ? (double)x[i] - x[i - 1u] : (double)x[i]; e += v * v; }
    return e;
}
static double wet(const int32_t *x, const int32_t *dry, uint32_t a, uint32_t b)   /* what x has more than dry */
{
    double e = 0;
    uint32_t i;
    for (i = a; i < b; i++) { double v = (double)x[i] - dry[i]; e += v * v; }
    return e;
}
static int same(const int32_t *x, const int32_t *y, uint32_t a, uint32_t b)
{
    return !memcmp(x + a, y + a, (b - a) * sizeof *x);
}
static uint32_t notes;
static void drain(void)
{
    uint32_t e;
    while ((e = heng_ev()) != 0)
        if (e >> 28 == HE_NOTE) notes++;
}

int main(void)
{
    uint32_t i, line0 = 0, line1 = 0;
    heng_init();
    hq.bpm = 120; hq.thall = 1;
    for (i = 0; i < 16u; i++) hq.pat[0][i] = ST_MAKE(0, -1);
    heng_cmd(HCMD(HC_PLAY, 0));
    run(256u * 20u);                            /* a step has begun: the sequence's note */
    heng_cmd(HCMD(HC_NOTE, 64u | 2u << 7));     /* E by hand */
    run(256u * 8u);
    for (i = 0; i < NTV; i++)
        if (tv[i].on && !tv[i].choked) {
            if (tv[i].line) line1++; else line0++;
        }
    check("THALL: the hand's note and the sequence's both sound", line0 == 1u && line1 == 1u);
    heng_cmd(HCMD(HC_STOP, 0));
    run(256u * 40u);

    heng_init();
    hq.thall = 0; hq.hvoice = 1;
    for (i = 0; i < NHV; i++)                   /* all voices busy */
        heng_cmd(HCMD(HC_NOTE, (60u + i % 12u) | (i % 8u) << 7));
    run(256u * 4u);
    young = 0;
    heng_cmd(HCMD(HC_NOTE, 72u | 7u << 7));     /* two at once */
    heng_cmd(HCMD(HC_NOTE, 48u | 0u << 7));
    run(256u * 2u);
    check("all voices busy: two notes begun together both sound (no voice taken in its attack)", young == 0u);

    heng_init();
    hq.thall = 0; hq.bpm = 120;
    for (i = 0; i < 16u; i++) hq.pat[0][i] = ST_MAKE(i & 7u, 0);
    hq.skip = heng_fired + 3u;                  /* the third step to fire: no note */
    drain();
    notes = 0;
    heng_cmd(HCMD(HC_PLAY, 0));
    for (i = 0; i < 16u * 11025u / 256u - 4u; i += 1u) { run(256u); drain(); }   /* (16 steps of 11025 frames, not the 17th) */
    check("a step asked to be skipped plays no note, the other fifteen do", notes == 15u);

    bar(out_a, SND_LEN, 0);
    bar(out_b, SND_LEN, 16u + 1u);
    check("a step's own LENGDE of 16 (as the knob): the same, bit for bit", same(out_a, out_b, 0, NOUT));
    bar(out_b, SND_LYS, 32u + 1u);
    check("a step's own LYS of 32 (open): the same, bit for bit", same(out_a, out_b, 0, NOUT));
    bar(out_b, SND_LYS, 4u + 1u);
    check("LYS 4 on the first step: darker while it sounds (its highs, of all, half or less; a third of the sound)",
          energy(out_b, 0, 11025u, 1) / energy(out_b, 0, 11025u, 0) < .5 * energy(out_a, 0, 11025u, 1) / energy(out_a, 0, 11025u, 0) &&
          energy(out_b, 0, 11025u, 0) < .4 * energy(out_a, 0, 11025u, 0));
    check("... and only then: the second step as without it, bit for bit", same(out_a, out_b, 11025u + 256u, NOUT));
    bar(out_b, SND_LEN, 32u + 1u);
    check("LENGDE 32 on the first step: its note lasts (at 0.85..1 s, after both have ended as they were: ten times the sound)",
          energy(out_b, 37485u, NOUT, 0) > 10. * energy(out_a, 37485u, NOUT, 0));
    bar(out_c, SND_ROM, 0u + 1u);               /* dry */
    bar(out_b, SND_ROM, 32u + 1u);
    check("ROM: 0 dry (not as it was), 32 the room four times as it was (its energy over 8 times)",
          wet(out_a, out_c, 0, 11025u) > 0 && wet(out_b, out_c, 0, 11025u) > 8. * wet(out_a, out_c, 0, 11025u));
    bar(out_b, SND_PIP, 0u + 1u);
    check("PIP 0 on the first step: its squeak changes (not the same)", !same(out_a, out_b, 0, 11025u));

    {                                           /* the sub: A2 under nothing, then B2, its notes */
        int32_t buf[512];
        uint32_t f, i2;
        double lo = 0, all = 0, jump = 0;
        int32_t prev = 0, lp = 0;
        bar(out_a, SND_LEN, 0);                 /* (to set it all as at the start) */
        memset((void *)hq.pat, 0, sizeof hq.pat);
        hq.sub[0][0] = ST_MAKE(5, 0);           /* A4: it sings A2 */
        hq.sub[0][1] = ST_MAKE(6, 0);           /* B4: B2 */
        heng_cmd(HCMD(HC_PLAY, 0));
        for (f = 0; f < 22050u; f += 256u) {
            memset(buf, 0, sizeof buf);
            heng_render(buf, 256u);
            for (i2 = 0; i2 < 256u; i2++) {
                int32_t x = buf[2u * i2], d = x - prev;
                lp += (x - lp) / 32;            /* (a one-pole low-pass near 220 Hz) */
                lo += (double)lp * lp;
                all += (double)x * x;
                if (f + i2 > 11025u - 600u && f + i2 < 11025u + 600u && (d < 0 ? -d : d) > jump) jump = d < 0 ? -d : d;
                prev = x;
            }
        }
        heng_cmd(HCMD(HC_STOP, 0));
        run(256u);
        memset((void *)hq.sub, 0, sizeof hq.sub);
        check("the sub: its steps sound, low (most of it under the low-pass)", all > 0 && lo > .5 * all);
        check("the sub: the second note takes the first's place without a click (no jump over 1/40 of full)",
              jump < 8388608. / 40.);
    }
    return fails ? 1 : 0;
}
