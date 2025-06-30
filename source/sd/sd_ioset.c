#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include "spu/spu.h"

#include <stdio.h>

/* old */
/*
static unsigned int pant[41] = {
    0,   2,   4,   7,   10,  13,  16,  20,  24,  28,  32,  36,  40,  45,
    50,  55,  60,  65,  70,  75,  80,  84,  88,  92,  96,  100, 104, 107,
    110, 112, 114, 116, 118, 120, 122, 123, 124, 125, 126, 127, 127
};
*/

static unsigned int pant[41] = {
    0x0000, 0x0141, 0x0352, 0x06F5, 0x0A7A, 0x0DE3, 0x0F5D, 0x12F7,
    0x1617, 0x1919, 0x1A66, 0x1D40, 0x1FFC, 0x229C, 0x2404, 0x2677,
    0x28CE, 0x2B07, 0x2D24, 0x2E44, 0x2FFF, 0x31A3, 0x329D, 0x344B,
    0x35DB, 0x374F, 0x3811, 0x3958, 0x3A82, 0x3B90, 0x3C81, 0x3CE3,
    0x3DAA, 0x3E55, 0x3EF1, 0x3F25, 0x3F87, 0x3FCB, 0x3FF3, 0x3FFF,
    0x3FFF
};

/* old */
/*
static unsigned int se_pant[65] = {
    0,   2,   4,   6,   8,   10,  14,  18,  22,  28,  34,  40,  46,
    52,  58,  64,  70,  76,  82,  88,  94,  100, 106, 112, 118, 124,
    130, 136, 142, 148, 154, 160, 166, 172, 178, 183, 188, 193, 198,
    203, 208, 213, 217, 221, 224, 227, 230, 233, 236, 238, 240, 242,
    244, 246, 248, 249, 250, 251, 252, 253, 254, 254, 255, 255, 255
};
*/

static unsigned int se_pant[65] = {
    0x0000, 0x00C5, 0x024D, 0x04CF, 0x0688, 0x08F3, 0x0A61, 0x0CB7,
    0x0E51, 0x0FAC, 0x11E3, 0x1369, 0x1589, 0x16FF, 0x18D5, 0x1A3C,
    0x1B9C, 0x1D87, 0x1EA8, 0x207E, 0x21BF, 0x22CD, 0x2485, 0x25B0,
    0x2751, 0x2845, 0x29D1, 0x2ADD, 0x2BE4, 0x2D2C, 0x2E23, 0x2F7A,
    0x2FFF, 0x3082, 0x31C0, 0x3297, 0x33A2, 0x346A, 0x352B, 0x3634,
    0x36CD, 0x37C1, 0x3863, 0x3940, 0x39BF, 0x3A4C, 0x3B0A, 0x3B76,
    0x3C1F, 0x3C8E, 0x3CF6, 0x3D72, 0x3DCB, 0x3E3F, 0x3E88, 0x3EE5,
    0x3F17, 0x3F4A, 0x3F89, 0x3FA8, 0x3FD2, 0x3FE6, 0x3FF9, 0x3FFF,
    0x3FFF
};

static unsigned int freq_tbl[108] = {
    0x010B, 0x011B, 0x012C, 0x013E, 0x0151, 0x0165, 0x017A, 0x0191,
    0x01A9, 0x01C2, 0x01DD, 0x01F9, 0x0217, 0x0237, 0x0259, 0x027D,
    0x02A3, 0x02CB, 0x02F5, 0x0322, 0x0352, 0x0385, 0x03BA, 0x03F3,
    0x042F, 0x046F, 0x04B2, 0x04FA, 0x0546, 0x0596, 0x05EB, 0x0645,
    0x06A5, 0x070A, 0x0775, 0x07E6, 0x085F, 0x08DE, 0x0965, 0x09F4,
    0x0A8C, 0x0B2C, 0x0BD6, 0x0C8B, 0x0D4A, 0x0E14, 0x0EEA, 0x0FCD,
    0x10BE, 0x11BD, 0x12CB, 0x13E9, 0x1518, 0x1659, 0x17AD, 0x1916,
    0x1A94, 0x1C28, 0x1DD5, 0x1F9B, 0x217C, 0x237A, 0x2596, 0x27D2,
    0x2A30, 0x2CB2, 0x2F5A, 0x322C, 0x3528, 0x3850, 0x3BAC, 0x3F36,
    0x0021, 0x0023, 0x0026, 0x0028, 0x002A, 0x002D, 0x002F, 0x0032,
    0x0035, 0x0038, 0x003C, 0x003F, 0x0042, 0x0046, 0x004B, 0x004F,
    0x0054, 0x0059, 0x005E, 0x0064, 0x006A, 0x0070, 0x0077, 0x007E,
    0x0085, 0x008D, 0x0096, 0x009F, 0x00A8, 0x00B2, 0x00BD, 0x00C8,
    0x00D4, 0x00E1, 0x00EE, 0x00FC
};

void spuwr(void)
{
    if (keyoffs)
    {
        spu_set_key_off(keyoffs);
        keyoffs = 0;
    }

    if (eoffs)
    {
        spu_set_reverb_off(eoffs);
        eoffs = 0;
    }

    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        if (spu_tr_wk[i].vol_fg)
        {
            spu_set_voice_volume(i, spu_tr_wk[i].vol_l, spu_tr_wk[i].vol_r);
            spu_tr_wk[i].vol_fg = 0;
        }

        if (spu_tr_wk[i].pitch_fg)
        {
            spu_set_voice_pitch(i, spu_tr_wk[i].pitch);
            spu_tr_wk[i].pitch_fg = 0;
        }

        if (spu_tr_wk[i].addr_fg)
        {
            if (spu_tr_wk[i].addr == 0xFFFFFFFF)
            {
                keyons &= ~(1 << i);
                keyoffs |= 1 << i;
                continue;
            }

            spu_set_voice_address(i, spu_tr_wk[i].addr);
            spu_tr_wk[i].addr_fg = 0;
        }

        if (spu_tr_wk[i].env1_fg)
        {
            spu_set_voice_attack(i, spu_tr_wk[i].a_mode, spu_tr_wk[i].ar);
            spu_set_voice_decay(i, spu_tr_wk[i].dr);
            spu_tr_wk[i].env1_fg = 0;
        }

        if (spu_tr_wk[i].env2_fg)
        {
            spu_set_voice_sustain(i, spu_tr_wk[i].s_mode, spu_tr_wk[i].sr, spu_tr_wk[i].sl);
            spu_tr_wk[i].env2_fg = 0;
        }

        if (spu_tr_wk[i].env3_fg)
        {
            spu_set_voice_release(i, spu_tr_wk[i].r_mode, spu_tr_wk[i].rr);
            spu_tr_wk[i].env3_fg = 0;
        }
    }

    if (eons)
    {
        spu_set_reverb_on(eons);
        eons = 0;
    }

    if (keyons)
    {
        spu_set_key_on(keyons);
        keyons = 0;
    }
}

void sound_off(void)
{
    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        spu_tr_wk[i].r_mode = SPU_ADSR_LIN_DEC;
        spu_tr_wk[i].rr = 7;
        spu_tr_wk[i].env3_fg = 1;
    }

    keyoffs = 0xFFFFFFFF;
    spuwr();
}

void sng_off(void)
{
    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        spu_tr_wk[i].r_mode = SPU_ADSR_LIN_DEC;
        spu_tr_wk[i].rr = 7;
        spu_tr_wk[i].env3_fg = 1;
    }

    song_end |= SD_BGM_MASK;
    song_loop_end |= SD_BGM_MASK;
    keyoffs |= SD_BGM_MASK;
}

void se_off(int i)
{
    spu_tr_wk[i + SD_SE_START].r_mode = SPU_ADSR_LIN_DEC;
    spu_tr_wk[i + SD_SE_START].rr = 0;
    spu_tr_wk[i + SD_SE_START].env3_fg = 1;
}

void sng_pause(void)
{
    spu_set_master_volume(0, 0);
}

void sng_pause_off(void)
{
    spu_set_master_volume(0x3fff, 0x3fff);
}

void keyon(void)
{
    keyons |= keyd;
}

void keyoff(void)
{
    keyoffs |= keyd;
}

void tone_set(unsigned int num)
{
    WAVE_W *sample;

    if (num < 512)
    {
        sample = &voice_tbl[num];
    }
    else
    {
        sample = &voice_exp_tbl[num - 512];
    }

    if (sample->addr == 0xFFFFFFFF)
    {
        SD_PRINT("track %d missing sample %d!\n", mtrack, num);
    }

    spu_tr_wk[mtrack].addr = sample->addr;
    spu_tr_wk[mtrack].addr_fg = 1;

    sptr->macro = sample->sample_note;
    sptr->micro = sample->sample_tune;

    if (sample->a_mode)
    {
        spu_tr_wk[mtrack].a_mode = SPU_ADSR_EXP_INC;
    }
    else
    {
        spu_tr_wk[mtrack].a_mode = SPU_ADSR_LIN_INC;
    }

    spu_tr_wk[mtrack].ar = ~sample->ar & 0x7F;
    spu_tr_wk[mtrack].dr = ~sample->dr & 0xF;
    spu_tr_wk[mtrack].env1_fg = 1;

    switch (sample->s_mode)
    {
    case 0:
        spu_tr_wk[mtrack].s_mode = SPU_ADSR_LIN_DEC;
        break;

    case 1:
        spu_tr_wk[mtrack].s_mode = SPU_ADSR_EXP_DEC;
        break;

    case 2:
        spu_tr_wk[mtrack].s_mode = SPU_ADSR_LIN_INC;
        break;

    default:
        spu_tr_wk[mtrack].s_mode = SPU_ADSR_EXP_INC;
        break;
    }

    spu_tr_wk[mtrack].sr = ~sample->sr & 0x7F;
    spu_tr_wk[mtrack].sl = sample->sl & 0xF;
    spu_tr_wk[mtrack].env2_fg = 1;

    if (!sample->r_mode)
    {
        spu_tr_wk[mtrack].r_mode = SPU_ADSR_LIN_DEC;
    }
    else
    {
        spu_tr_wk[mtrack].r_mode = SPU_ADSR_EXP_DEC;
    }

    spu_tr_wk[mtrack].rr = sptr->rrd = ~sample->rr & 0x1F;
    spu_tr_wk[mtrack].env3_fg = 1;

    if (sptr->panmod == 0)
    {
        pan_set2(sample->pan);
    }

    sptr->dec_vol = sample->decl_vol;
}

void pan_set2(unsigned char pan)
{
    if (sptr->panoff == 0)
    {
        sptr->panf = pan * 2;
        sptr->pand = pan * 512;
    }
}

void vol_set(unsigned int vol)
{
    unsigned int pan;
    unsigned int l, r;

    if (vol >= sptr->dec_vol)
    {
        vol -= sptr->dec_vol;
    }
    else
    {
        vol = 0;
    }

    vol++;

    if (sptr->panmod == 2 && mtrack < SD_BGM_VOICES)
    {
        pan = mix_fader[mtrack].pan;

        if (sound_mono_fg != 0)
        {
            pan = 32;
        }

        l = (vol * se_pant[64 - pan]) / 0x7f;
        r = (vol * se_pant[pan]) / 0x7f;

        spu_tr_wk[mtrack].vol_l = (l * sng_master_vol[mtrack]) >> 16;
        spu_tr_wk[mtrack].vol_r = (r * sng_master_vol[mtrack]) >> 16;
        spu_tr_wk[mtrack].vol_fg = 1;
    }
    else if ((mtrack < SD_BGM_VOICES) || (se_playing[mtrack - SD_SE_START].kind == 0))
    {
        pan = sptr->pand >> 8;

        if (pan > 40)
        {
            pan = 40;
        }

        if (sound_mono_fg != 0)
        {
            pan = 20;
        }

        l = (vol * pant[40 - pan]) / 0x7f;
        r = (vol * pant[pan]) / 0x7f;

        if (mtrack < SD_BGM_VOICES)
        {
            spu_tr_wk[mtrack].vol_l = (l * sng_master_vol[mtrack]) >> 16;
            spu_tr_wk[mtrack].vol_r = (r * sng_master_vol[mtrack]) >> 16;
            spu_tr_wk[mtrack].vol_fg = 1;
        }
        else
        {
            spu_tr_wk[mtrack].vol_l = l;
            spu_tr_wk[mtrack].vol_r = r;
            spu_tr_wk[mtrack].vol_fg = 1;
        }
    }
    else
    {
        pan = se_pan[mtrack - SD_SE_START];
        vol = (vol * se_vol[mtrack - SD_SE_START]) >> 16;

        if (sound_mono_fg != 0)
        {
            pan = 32;
        }

        vol = (vol * se_vol[mtrack - SD_SE_START]) >> 6;
        vol++;

        l = (vol * se_pant[64 - pan]) >> 7;
        r = (vol * se_pant[pan]) >> 7;

        spu_tr_wk[mtrack].vol_l = l;
        spu_tr_wk[mtrack].vol_r = r;
        spu_tr_wk[mtrack].vol_fg = 1;
    }
}

void freq_set(unsigned int base)
{
    unsigned char temp, temp2, temp3, temp4;
    int           freq;
    unsigned int *ptr;

    base += sptr->micro;
    temp4 = base;
    temp3 = ((base >> 8) + sptr->macro) & 0x7f;
    ptr = freq_tbl;
    freq = ptr[temp3 + 1] - ptr[temp3];

    if (freq & 0x8000)
    {
        freq = 0xC9;
    }

    temp = freq;
    temp2 = freq >> 8;
    freq = ((temp * temp4) >> 8) + (temp2 * temp4);
    freq += ptr[temp3];

    spu_tr_wk[mtrack].pitch = freq;
    spu_tr_wk[mtrack].pitch_fg = 1;
}

void drum_set(unsigned int num)
{
    tone_set(num + 440);
}
