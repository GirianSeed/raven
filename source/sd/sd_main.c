#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include "spu/spu.h"

#include <stdio.h>

int sd_debug_mode;
int sd_loop_mode;

static void sd_init_reverb(void)
{
    spu_set_reverb_enable(0);
    spu_reverb_clear();
    spu_set_reverb_mode(SPU_REV_MODE_STUDIO_C);
    spu_set_reverb_depth(0x4000, 0x4000);
    spu_set_reverb_enable(1);
    spu_set_reverb_on(0x1fff);

    eoffs = 0;
    eons = 0x1FFF;
}

static void sd_init_volume(void)
{
    spu_set_master_volume(0x3fff, 0x3fff);
}

void sd_init(int debug, int loop)
{
    sd_debug_mode = debug;
    sd_loop_mode = loop;

    SD_PRINT("SD:START\n");

    spu_init();

    sd_init_reverb();
    sd_init_volume();

    init_sng_work();

    for (int i = 0; i < 8; i++)
    {
        se_playing[i].code = 0;
    }
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
