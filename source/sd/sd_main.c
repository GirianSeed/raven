#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include "spu/spu.h"

#include <string.h>

int sd_debug_mode;
int song_loop_count;

static void sd_init_reverb(int enable)
{
    spu_set_reverb_enable(0);
    spu_reverb_clear();
    spu_set_reverb_mode(SPU_REV_MODE_STUDIO_C);
    spu_set_reverb_depth(0x4000, 0x4000);

    if (enable)
    {
        spu_set_reverb_enable(1);
    }

    spu_set_reverb_on(SD_BGM_MASK);

    eoffs = 0;
    eons = SD_BGM_MASK;
}

static void sd_init_volume(void)
{
    spu_set_master_volume(0x3fff, 0x3fff);
}

void sd_init(int debug, int loops, int reverb)
{
    sd_debug_mode = debug;
    song_loop_count = loops;

    memset(voice_tbl, 0xff, sizeof(voice_tbl));
    memset(spu_tr_wk, 0, sizeof(spu_tr_wk));

    SD_PRINT("SD:START\n");

    spu_init();

    sd_init_reverb(reverb);
    sd_init_volume();

    init_sng_work();

    for (int i = 0; i < 8; i++)
    {
        se_playing[i].code = 0;
    }

    se_exp_table = (SETBL2 *)se_header;
    se_data  = (unsigned char *)(se_exp_table + 128);
}

void sd_term(void)
{
    spu_set_key_off(0xffffff);
    spu_quit();

    SD_PRINT("SD:TERM\n");
}

void sd_tick(void)
{
    IntSdMain();
}
