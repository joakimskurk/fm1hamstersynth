/* SPDX-License-Identifier: GPL-3.0-only */
/* HamsterSynth's sound demo (tools/ham_demo.sh): a song in A minor (Am F C G) at 128 BPM, eight patterns, the steps'
 * own sound where it matters. Only in the demo's host build (-DHAM_DEMO); the firmware starts as the page does.
 *   A  intro: dark and long, in a big room, opening a little           (LYS 8 .. 15, LENGDE 30, ROM 30)
 *   B  intro: opening all the way, on to the groove                     (LYS 14 .. 32)
 *   C  the groove: Am, F, the bass under it                             (the knobs: turned by hand in the demo)
 *   D  the groove: C, G
 *   E  the build: ratchets, two, three, four, up and up: F, then E (G sharp: home to A minor)  (LYS 6 .. 32, PIP 8 .. 32)
 *   F  the hook: Am, F, up high
 *   G  the hook: C, G, home to G
 *   H  the end: Am, long, the light going                               (LYS 26 .. 8, LENGDE 32, ROM 32)
 * The song: A B C D C D E F G F G, then C D F G in THALL, then H. */
#define DS(k, o, fx) ((uint16_t)(ST_MAKE((k), (o)) | (fx)))
#define GL (1u << 6)                            /* glide */
#define AC (1u << 7)                            /* accent */
#define A7 (1u << 8)                            /* arpeggio +7 */
#define A12 (2u << 8)                           /* arpeggio +12 */
#define R2 (1u << 10)                           /* ratchet x2, x3, x4 */
#define R3 (2u << 10)
#define R4 (3u << 10)
#define SH ST_SHARP_BIT                         /* a semitone up */
/* the hamsters: C 0, D 1, E 2, F 3, G 4, A 5, B 6, the high C 7; octave 0 is C4 .. C5 */
#define C_ 0
#define D_ 1
#define E_ 2
#define F_ 3
#define G_ 4
#define A_ 5
#define B_ 6
#define CC_ 7
static void ham_demo(void)
{
    static const uint16_t P[8][16] = {
        /* A  intro: Am, F */
        {DS(A_, -1, AC), 0, DS(E_, 0, 0), 0, DS(CC_, 0, 0), 0, DS(E_, 0, 0), DS(B_, 0, 0),
         DS(F_, -1, AC), 0, DS(C_, 0, 0), 0, DS(A_, 0, 0), 0, DS(G_, 0, 0), DS(CC_, 0, GL)},
        /* B  intro: C, G */
        {DS(C_, -1, AC), 0, DS(G_, 0, 0), 0, DS(E_, 1, 0), 0, DS(D_, 1, GL), DS(CC_, 0, 0),
         DS(G_, -1, AC), 0, DS(D_, 0, 0), DS(G_, 0, 0), DS(B_, 0, 0), DS(D_, 1, 0), DS(B_, 0, 0), DS(G_, 0, A12)},
        /* C  the groove: Am, F */
        {DS(A_, -2, AC), DS(E_, 0, 0), DS(A_, 0, 0), DS(CC_, 0, 0), DS(A_, -1, 0), DS(E_, 0, 0), DS(B_, 0, 0), DS(CC_, 0, GL),
         DS(F_, -2, AC), DS(C_, 0, 0), DS(F_, 0, 0), DS(A_, 0, 0), DS(F_, -1, 0), DS(C_, 0, 0), DS(G_, 0, 0), DS(A_, 0, R2)},
        /* D  the groove: C, G */
        {DS(C_, -1, AC), DS(E_, 0, 0), DS(G_, 0, 0), DS(CC_, 0, 0), DS(E_, -1, 0), DS(G_, 0, 0), DS(A_, 0, 0), DS(G_, 0, GL),
         DS(G_, -2, AC), DS(D_, 0, 0), DS(G_, 0, 0), DS(B_, 0, 0), DS(D_, 1, 0), DS(B_, 0, 0), DS(A_, 0, R2), DS(G_, 0, A12)},
        /* E  the build: F, then E major (E G# B D: the way home to A minor), the ratchets closer and closer */
        {DS(F_, -1, R2 | AC), DS(A_, -1, R2), DS(C_, 0, R2), DS(F_, 0, R2), DS(A_, 0, R2), DS(CC_, 0, R2), DS(A_, 0, R3), DS(CC_, 0, R3),
         DS(E_, -1, R3 | AC), DS(G_, -1, R3 | SH), DS(B_, -1, R3), DS(E_, 0, R3), DS(G_, 0, R4 | SH), DS(B_, 0, R4), DS(D_, 1, R4), DS(E_, 1, R4 | AC)},
        /* F  the hook: Am, F */
        {DS(A_, -1, AC), DS(CC_, 0, 0), DS(E_, 1, 0), DS(D_, 1, GL), DS(CC_, 0, 0), 0, DS(B_, 0, 0), DS(CC_, 0, A7),
         DS(F_, -1, AC), DS(A_, 0, 0), DS(CC_, 0, 0), 0, DS(A_, 0, GL), DS(G_, 0, 0), DS(A_, 0, 0), DS(CC_, 0, R3)},
        /* G  the hook: C, G */
        {DS(C_, -1, AC), DS(E_, 1, 0), DS(D_, 1, 0), DS(CC_, 0, 0), DS(G_, 0, GL), 0, DS(E_, 0, 0), DS(G_, 0, A12),
         DS(G_, -2, AC), DS(B_, 0, 0), DS(D_, 1, 0), DS(G_, 1, AC), DS(F_, 1, 0), DS(D_, 1, 0), DS(B_, 0, R2), DS(G_, 0, R4)},
        /* H  the end: Am, F, long */
        {DS(A_, -1, AC), 0, DS(CC_, 0, 0), 0, DS(E_, 1, 0), 0, DS(A_, 1, 0), 0,
         DS(F_, -1, 0), 0, DS(A_, 0, 0), 0, DS(CC_, 0, 0), 0, DS(E_, 1, GL), 0},
    };
    static const uint8_t DR[8][4] = {               /* kick, snare, hat, clap: 8 steps each (twice a pattern) */
        {0x00, 0x00, 0x00, 0x00}, {0x11, 0x00, 0x00, 0x80}, {0x11, 0x44, 0xFF, 0x00}, {0x11, 0x44, 0xFF, 0x00},
        {0x55, 0xF0, 0xFF, 0x80}, {0x91, 0x44, 0xFF, 0x44}, {0x11, 0x44, 0xFF, 0x44}, {0x01, 0x00, 0x00, 0x00}};
    /* the voices (4: the hamster kit: a cheek's pop, a seed's crunch, a sniff, a squeak) */
    static const uint8_t DV[8][4] = {{0, 0, 0, 0}, {0, 0, 0, 3}, {0, 0, 0, 0}, {0, 1, 3, 0},
                                     {1, 0, 0, 3}, {0, 1, 3, 3}, {1, 0, 3, 3}, {3, 0, 0, 0}};
    static const char SONG[] = "ABCDCDEFGFGCDFGH";
    uint32_t p, i, k;
    for (p = 0; p < 8u; p++) {
        for (i = 0; i < 16u; i++) {
            hq.pat[p][i] = P[p][i];
            for (k = 0; k < NSND; k++)
                hq.plock[p][i][k] = 0;
        }
        for (i = 0; i < 4u; i++) {
            hq.drum[p][i] = DR[p][i];
            hq.dvoice[p][i] = DV[p][i];
        }
    }
    for (i = 0; i < 16u; i++) {                 /* the steps' own sound (a value + 1) */
        hq.plock[0][i][SND_LYS] = (uint8_t)(8u + i / 2u + 1u);   /* A: dark, opening a little */
        hq.plock[0][i][SND_LEN] = 30u + 1u;
        hq.plock[0][i][SND_ROM] = 30u + 1u;
        hq.plock[0][i][SND_PIP] = 26u + 1u;
        hq.plock[1][i][SND_LYS] = (uint8_t)(14u + i * 18u / 15u + 1u);   /* B: open */
        hq.plock[1][i][SND_LEN] = (uint8_t)(28u - i * 10u / 15u + 1u);
        hq.plock[1][i][SND_ROM] = 28u + 1u;
        hq.plock[1][i][SND_PIP] = 22u + 1u;
        hq.plock[4][i][SND_LYS] = (uint8_t)(6u + i * 26u / 15u + 1u);   /* E: the build */
        hq.plock[4][i][SND_LEN] = 10u + 1u;
        hq.plock[4][i][SND_PIP] = (uint8_t)(8u + i * 24u / 15u + 1u);
        hq.plock[4][i][SND_ROM] = 12u + 1u;
        hq.plock[7][i][SND_LYS] = (uint8_t)(26u - i * 18u / 15u + 1u);  /* H: the light going */
        hq.plock[7][i][SND_LEN] = 32u + 1u;
        hq.plock[7][i][SND_ROM] = 32u + 1u;
        hq.plock[7][i][SND_PIP] = 28u + 1u;
    }
    for (i = 0; SONG[i]; i++)
        hq.song[i] = (uint8_t)(SONG[i] - 'A');
    hq.song_len = i;
    bpm = 128;
    swing = 8;
    drums_on = 1;
#ifdef HAM_DEMO_PAGE_VOICE
    hvoice = 0;                                 /* (the same demo in the page's voice, to hold against it) */
#endif
}
