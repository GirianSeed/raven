#include "sd/sd_cli.h"
#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <stdint.h>
#include <stdio.h>

static int bgm_idx;
static int sd_print_fg;

int sd_sng_play(void)
{
    return sng_status > 2;
}

int sd_se_play(void)
{
    int bits = song_end >> 13;
    for (int i = 0; i < 8; i++)
    {
        if ((bits & 1) == 0 && se_playing[i].pri != 255)
        {
            return se_playing[i].code;
        }
        bits >>= 1;
    }
    return 0;
}

static int SePlay(unsigned int sound_code)
{
    SEPLAYTBL se;
    int       bits;
    int       index;
    int       pri;

    bits = song_end >> 13;
    for (int i = 0; i < 8; i++)
    {
        if ((bits & 1) != 0)
        {
            se_playing[i].code = 0;
            se_playing[i].pri = 0;
            se_playing[i].character = 0;
        }
        bits >>= 1;
    }

    se.code = sound_code;
    sound_code &= 0xFF;

    if (sound_code < 128)
    {
        se_tracks = se_tbl[sound_code].tracks;
        se.character = se_tbl[sound_code].character;
    }
    else
    {
        se_tracks = se_exp_table[sound_code - 128].tracks;
        se.character = se_exp_table[sound_code - 128].character;
    }

    for (int track = 0; track < se_tracks; track++)
    {
        if (sound_code < 128)
        {
            se.pri = se_tbl[sound_code].pri;
            se.kind = se_tbl[sound_code].kind;
            se.addr = se_tbl[sound_code].addr[track];
        }
        else
        {
            se.pri = se_exp_table[sound_code - 128].pri;
            se.kind = se_exp_table[sound_code - 128].kind;
            se.addr = (unsigned char *)se_header + (uintptr_t)se_exp_table[sound_code - 128].addr[track];
        }

        index = -1;
        pri = 256;

        for (int i = 0; i < 8; i++)
        {
            if (((se_playing[i].code & 0xFF) == sound_code) && !se_request[i].code)
            {
                pri = 0;
                index = i;
                break;
            }
            else if (se_tracks == 1 && ((se_request[i].code & 0xFF) == sound_code))
            {
                pri = 0;
                index = i;
                break;
            }
            if (se.character)
            {
                if (se_playing[i].character == se.character)
                {
                    pri = 0;
                    index = i;
                    break;
                }
                else if (se_request[i].character == se.character)
                {
                    pri = 0;
                    index = i;
                    break;
                }
            }
        }

        if (index < 0)
        {
            for (int i = 0; i < 8; i++)
            {
                if ((se_playing[i].code != 0) || (se_request[i].code != 0))
                {
                    continue;
                }

                pri = 0;
                index = i;
                break;
            }

            if (index < 0)
            {
                for (int i = 0; i < 8; i++)
                {
                    if (se_request[i].code == 0 && se_playing[i].pri <= pri)
                    {
                        pri = se_playing[i].pri;
                        index = i;
                    }
                    else if (se_request[i].pri <= pri)
                    {
                        pri = se_request[i].pri;
                        index = i;
                    }
                }
            }
        }

        if (se.pri < pri)
        {
            continue;
        }

        se_request[index].pri = se.pri;
        se_request[index].kind = se.kind;
        se_request[index].character = se.character;
        se_request[index].addr = se.addr;
        se_request[index].code = se.code;

        se.character = 0;

        if (se.pri == 0xFF)
        {
            stop_jouchuu_se = 0;
        }
    }

    se_tracks = 0;
    return 0;
}

static void sd_set(unsigned int sound_code)
{
    unsigned int mode;

    if (sd_print_fg != 0)
    {
        printf("SdCode=%x\n", sound_code);
    }

    mode = sound_code & 0xFF000000;
    if (mode == 0)
    {
        if (sound_code & 0xFF)
        {
            SePlay(sound_code);
        }
    }
    else if (mode == 0x01000000)
    {
        if (sd_sng_code_buf[bgm_idx] == 0)
        {
            sd_sng_code_buf[bgm_idx] = sound_code;
            bgm_idx = (bgm_idx + 1) & 0xF;
            return;
        }

        printf("***TooMuchBGMSoundCode(%x)***\n", sound_code);
    }
    else
    {
        switch (sound_code)
        {
        case 0xFF000005:
            sound_mono_fg = 1;
            return;
        case 0xFF000006:
            sound_mono_fg = 0;
            return;
        case 0xFF000007:
            se_rev_on = 1;
            return;
        case 0xFF000008:
            se_rev_on = 0;
            return;
        case 0xFF000009:
            se_rev_on = 1;
            return;
        case 0xFF0000FE:
            stop_jouchuu_se = 1;
            return;
        }

        printf("sd_set:unknown code (code=%x)\n", sound_code);
    }
}

int sd_set_cli(int sound_code, int sync_mode)
{
    if (sync_mode != SD_ASYNC)
    {
        printf("sd_set_cli:unsupported sync_mode\n");
        return 1;
    }

    sd_set(sound_code);
    return 0;
}

int sd_sng_code(void)
{
    if (sng_play_code == 0xFFFFFFFF || sng_play_code == 0)
    {
        return 0;
    }

    return sng_play_code;
}
