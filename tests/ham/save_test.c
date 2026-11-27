/* SPDX-License-Identifier: GPL-3.0-only */
/* HamsterSynth's settings in flash: a save of 4 (before the sound knobs) loads, its patterns, its tempo, the sound as
 * ever; a save of 5 keeps the knobs and the steps' own */
#define main host_main
#include "hamster.c"
#undef main
int main(void)
{
    save_t old;
    uint32_t n = SV4_SIZE, ok;
    memset(&old, 0, sizeof old);
    old.ver = 4; old.bpm = 133; old.active = 2; old.cur_step = 3; old.song_len = 1; old.lvl_ham = 16; old.lvl_drum = 16;
    old.pat[2][5] = ST_MAKE(4, 0);
    host_set[0] = 1; host_set[1] = (uint8_t)n; host_set[2] = (uint8_t)(n >> 8); host_set[3] = 0;
    memcpy(host_set + 4, &old, n);
    store_load();
    ok = bpm == 133u && active == 2u && hq.pat[2][5] == ST_MAKE(4, 0) && hq.snd[SND_LYS] == 32u && hq.snd[SND_LEN] == 16u &&
         !hq.plock[2][5][0];
    printf("a save of 4 (%u B) loads, the sound as ever: %s\n", n, ok ? "ok" : "FAIL");
    if (!ok) return 1;
    hq.snd[SND_ROM] = 7; hq.plock[1][9][SND_PIP] = 12;
    store_save();
    hq.snd[SND_ROM] = 16; hq.plock[1][9][SND_PIP] = 0;
    store_load();
    ok = hq.snd[SND_ROM] == 7u && hq.plock[1][9][SND_PIP] == 12u && bpm == 133u;
    printf("a save of 5 (%u B) keeps the knobs and the steps' own: %s\n", (unsigned)sizeof(save_t), ok ? "ok" : "FAIL");
    return ok ? 0 : 1;
}
