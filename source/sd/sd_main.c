#include <string.h>

#include "sd/sd_ext.h"
#include "sd/sd_incl.h"
#include "spu/spu.h"

static void sd_init_reverb(int enable)
{
    spu_set_reverb_enable(0, 0);
    spu_set_reverb_enable(1, 0);

    spu_reverb_clear();
    spu_set_reverb_mode(0, SPU_REV_MODE_HALL);
    spu_set_reverb_mode(1, SPU_REV_MODE_HALL);
    spu_set_reverb_depth(0, 0x2000, 0x2000);
    spu_set_reverb_depth(1, 0x2000, 0x2000);

    if (enable)
    {
        spu_set_reverb_enable(0, 1);
        spu_set_reverb_enable(1, 1);
    }

    spu_set_reverb_on(0, 0xffffff);
    spu_set_reverb_on(1, 0x3fffff);

    eoffs[0] = 0;
    eoffs[1] = 0;

    eons[0] = 0xffffff;
    eons[1] = 0x3fffff;
}

static void sd_init_volume(void)
{
    spu_set_master_volume(0, 0x3fff, 0x3fff);
    spu_set_master_volume(1, 0x3fff, 0x3fff);
}

static void sd_mem_alloc(void)
{
    se_header = (SETBL *)se_data_area;
    se_data = se_data_area + 4096;

    se_exp_header = (SETBL *)se_exp_data_area;
    se_exp_data = se_exp_data_area + 4096;
}

void sd_init(int debug, int loops, int reverb)
{
    sd_debug_mode = debug;
    song_loop_count = loops;

    memset(voice_tbl, 0xff, sizeof(voice_tbl));
    memset(drum_tbl, 0xff, sizeof(drum_tbl));
    memset(spu_tr_wk, 0, sizeof(spu_tr_wk));

    SD_PRINT("SD:START\n");

    sd_mem_alloc();

    spu_init();

    sd_init_reverb(reverb);
    sd_init_volume();

    init_sng_work();

    for (int i = 0; i < SD_SE_VOICES; i++)
    {
        se_playing[i].code = 0;
    }

    for(int i = 0; i < SD_BGM_VOICES/2; i++)
    {
        mix_fader[i].step = 0;
        mix_fader[i].vol = 0xFFFF;
        mix_fader[i].target = 0xFFFF;
        mix_fader[i].pan = 32;
    }

    for(int i = SD_BGM_VOICES/2; i < SD_BGM_VOICES; i++)
    {
        mix_fader[i].step = 0;
        mix_fader[i].vol = 0;
        mix_fader[i].target = 0;
        mix_fader[i].pan = 32;
    }
}

void sd_term(void)
{
    spu_set_key_off(0, 0xffffff);
    spu_set_key_off(1, 0x3fffff);
    spu_quit();

    SD_PRINT("SD:TERM\n");
}

void sd_tick(void)
{
    IntSdMain();
}
