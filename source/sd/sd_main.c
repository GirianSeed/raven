#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include "spu/libspu.h"

#include <stdio.h>

int sd_debug_mode;

static void sd_init_voice(void)
{
    SpuSetPitchLFOVoice(SPU_OFF, SPU_ALLCH);
    SpuSetNoiseVoice(SPU_OFF, SPU_ALLCH);
}

static void sd_init_reverb(void)
{
    SpuReverbAttr attr;

    SpuSetReverb(SPU_OFF);
    SpuReserveReverbWorkArea(SPU_ON);
    SpuClearReverbWorkArea(SPU_REV_MODE_STUDIO_C);

    attr.mask = SPU_REV_MODE;
    attr.mode = SPU_REV_MODE_STUDIO_C;
    SpuSetReverbModeParam(&attr);

    attr.mask = SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    attr.depth.left = 0x4000;
    attr.depth.right = 0x4000;
    SpuSetReverbDepth(&attr);

    SpuSetReverb(SPU_ON);
    SpuSetReverbVoice(SPU_ON, SPU_13CH - 1);

    eoffs = 0;
    eons = 0x1FFF;
}

static void sd_init_volume(void)
{
    SpuCommonAttr attr;

    attr.mask = SPU_COMMON_MVOLL | SPU_COMMON_MVOLR;
    attr.mvol.left = 0x3FFF;
    attr.mvol.right = 0x3FFF;
    SpuSetCommonAttr(&attr);
}

void sd_init(int debug)
{
    sd_debug_mode = debug;

    SpuInit();

    spu_wave_start_ptr = SpuMalloc(0x73E00);
    printf("spu_wave_start_ptr=%x\n", spu_wave_start_ptr);

    sd_init_voice();
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
    SpuSetKey(SPU_OFF, SPU_ALLCH);
    SpuQuit();
}

void sd_tick(void)
{
    IntSdMain();
}
