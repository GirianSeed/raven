#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <stdio.h>

static int dword_800BEFF8;
static int sng_syukan_vol;
static int sng_fadein_time;
static int dword_800BF154;
static int sng_fout_fg;
static int sd_code_read;
static int sng_kaihi_fg;
static int sng_pause_fg;
static SOUND_W sound_w[SD_N_VOICES];
static int sng_fadein_fg;
static int sng_fade_time[SD_BGM_VOICES];
static int sng_syukan_fg;
static int sng_fout_term;
static int sng_fade_value[SD_BGM_VOICES];

int            se_tracks;
int            sd_sng_code_buf[16];
int            eons;
SEPLAYTBL      se_playing[SD_SE_VOICES];
unsigned int   mdata1;
unsigned int   mdata2;
unsigned int   mdata3;
unsigned int   mdata4;
SEPLAYTBL      se_request[SD_SE_VOICES];
int            sng_status;
int            stop_jouchuu_se;
int            se_pan[SD_SE_VOICES];
unsigned int   mtrack;
int            se_vol[SD_SE_VOICES];
int            eoffs;
unsigned int   keyons;
unsigned char *se_data;
SETBL2        *se_exp_table;
unsigned int   keyoffs;
unsigned char  sng_data[0x20000];
unsigned int   song_end;
unsigned int   song_loop_end;
unsigned int   sng_play_code;
int            sound_mono_fg;
unsigned int   keyd;
unsigned char  se_header[0x2800];
WAVE_W         voice_tbl[512];
WAVE_W         voice_exp_tbl[256];
unsigned char *mptr;
int            se_rev_on;
SOUND_W       *sptr;
SPU_TRACK_REG  spu_tr_wk[SD_N_VOICES];
int            sng_master_vol[SD_BGM_VOICES];
unsigned int   skip_intro_loop;

void IntSdMain(void)
{
    unsigned int code;
    unsigned int n_songs;

    if (sd_sng_code_buf[sd_code_read])
    {
        code = sd_sng_code_buf[sd_code_read];
        sd_sng_code_buf[sd_code_read] = 0;
        sd_code_read = (sd_code_read + 1) & 0xF;
        SD_PRINT("SngCode=%x\n", code);
    }
    else
    {
        code = 0;
    }

    switch (code)
    {
    case 0:
        break;

    // Pause
    case 0x01FFFF01:
        sng_pause_fg = 1;
        sng_pause();
        SD_PRINT("SongPauseOn\n");
        break;

    // Unpause
    case 0x01FFFF02:
        sng_pause_off();
        sng_pause_fg = 0;
        SD_PRINT("SongPauseOff\n");
        break;

    // Fade in
    case 0x01FFFF03: /* fallthrough */
    case 0x01FFFF04: /* fallthrough */
    case 0x01FFFF05:
        if (sng_play_code != 0xFFFFFFFF)
        {
            sng_fout_fg = 0;

            if (sng_status < 3)
            {
                sng_fadein_fg = code;
            }
            else
            {
                SngFadeIn(code);
            }

            sng_kaihi_fg = 0;
        }

        SD_PRINT("SongFadein\n");
        break;

    // Fade out
    case 0x01FFFF06: /* fallthrough */
    case 0x01FFFF07: /* fallthrough */
    case 0x01FFFF08: /* fallthrough */
    case 0x01FFFF09:
        SngFadeOutP(code);
        SD_PRINT("SongFadeout&Pause\n");
        break;

    // Fade out and stop
    case 0x01FFFF0A: /* fallthrough */
    case 0x01FFFF0B: /* fallthrough */
    case 0x01FFFF0C: /* fallthrough */
    case 0x01FFFF0D:
        SngFadeOutS(code);
        SD_PRINT("SongFadeout&Stop\n");
        break;

    // Evasion Mode
    case 0x01FFFF10:
        SngKaihiP();
        SD_PRINT("SongKaihiMode\n");
        break;

    // First Person Mode
    case 0x01FFFF20:
        sng_syukan_fg = 1;
        SD_PRINT("SongSyukanMode On\n");
        break;

    // Exit first person
    case 0x01FFFF21:
        sng_syukan_fg = 0;
        SD_PRINT("SongSyukanMode Off\n");
        break;

    case 0x01FFFFFF:
        sng_play_code = 0;
        sng_off();
        SD_PRINT("SongStop\n");
        break;

    case 0x01000001: /* fallthrough */
    case 0x01000002: /* fallthrough */
    case 0x01000003: /* fallthrough */
    case 0x01000004: /* fallthrough */
    case 0x01000005: /* fallthrough */
    case 0x01000006: /* fallthrough */
    case 0x01000007: /* fallthrough */
    case 0x01000008:
        if (sng_play_code == code)
        {
            SD_PRINT("SameSongHasAlreadyPlayed\n");
            break;
        }

        if (sng_status < 2)
        {
            SD_PRINT("sng_status=%x\n", sng_status);
            break;
        }

        n_songs = sng_data[0];
        if ((code & 0xF) > n_songs)
        {
            SD_PRINT("ERROR:SNG PLAY CODE(%x/%x)\n", code & 0xF, n_songs);
            break;
        }

        sng_status = 2;
        if (sng_play_code == 0xFFFFFFFF)
        {
            for (int track = 0; track < SD_BGM_VOICES; track++)
            {
                sng_fade_time[track] = 0;
                sng_fade_value[track] = 0;
                sng_master_vol[track] = 0;
            }

            sng_fout_term = 0;
        }

        sng_play_code = code;

        sng_off();
        sng_pause_fg = 0;
        sng_kaihi_fg = 0;
        skip_intro_loop = 0;
        break;

    default:
        SD_PRINT("ERROR:UNK SNG PLAY CODE(%x)\n", code);
        break;
    }

    switch (sng_status)
    {
        case 2:
            if (sng_play_code == 0 || sng_play_code == 0xFFFFFFFF)
            {
                break;
            }

            sng_adrs_set(sng_play_code);
            SngFadeWkSet();
            sng_status = 3;
            dword_800BEFF8 = 0;
            break;

        case 3:
            if ((dword_800BEFF8 != 0) && ((dword_800BEFF8 & SD_BGM_MASK) != (song_end & SD_BGM_MASK)))
            {
                SD_PRINT("*** SOUND WORK IS BROKEN !!! ***\n");
                SD_PRINT("*** SOUND WORK IS BROKEN !!! ***\n");
                SD_PRINT("*** song_end:%x -> %x        ***\n", song_end & SD_BGM_MASK, dword_800BEFF8 & SD_BGM_MASK);
                SD_PRINT("*** SOUND WORK IS BROKEN !!! ***\n");
                SD_PRINT("*** SOUND WORK IS BROKEN !!! ***\n");
            }

            SngFadeInt();
            SngTempoInt();

            for (mtrack = 0; mtrack < SD_BGM_VOICES; mtrack++)
            {
                keyd = 1 << mtrack;

                if ((song_end & keyd) == 0)
                {
                    sptr = &sound_w[mtrack];

                    if (!sptr->mpointer)
                    {
                        song_end |= keyd;
                        song_loop_end |= keyd;
                    }
                    else
                    {
                        mptr = sptr->mpointer;

                        if (sound_sub())
                        {
                            song_end |= keyd;
                            song_loop_end |= keyd;
                            sptr->mpointer = 0;
                        }
                        else
                        {
                            sptr->mpointer = mptr;
                        }
                    }
                }
            }

            dword_800BEFF8 = song_end;

            if ((song_loop_end & SD_BGM_MASK) == SD_BGM_MASK)
            {
                song_loop_end = song_end;

                if (--song_loop_count < 0)
                {
                    song_end = SD_BGM_MASK;
                }
            }

            if ((song_end & SD_BGM_MASK) == SD_BGM_MASK)
            {
                sng_status = 4;
            }

            if (skip_intro_loop > 1)
            {
                skip_intro_loop = 0;
            }
            break;

        case 4:
            sng_off();
            sng_play_code = 0;
            sng_status = 2;
            break;

    }

    if (stop_jouchuu_se >= 2)
    {
        stop_jouchuu_se = 0;
    }

    spuwr();
}

void SngFadeIn(unsigned int mode)
{
    switch (mode)
    {
    case 0x01FFFF03:
        sng_fadein_time = 327;
        break;
    case 0x01FFFF04:
        sng_fadein_time = 109;
        break;
    case 0x01FFFF05:
        sng_fadein_time = 65;
        break;
    }

    if (sng_fadein_time == 0)
    {
        sng_fadein_time = 1;
    }

    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        sng_fade_time[i] = 0;
    }

    sng_fout_term = 0;
}

int SngFadeOutP(unsigned int code)
{
    int time = 1;

    if (sng_status && sng_fout_term != -1)
    {
        switch (code)
        {
        case 0x01FFFF06:
            time = 655;
            break;
        case 0x01FFFF07:
            time = 327;
            break;
        case 0x01FFFF08:
            time = 109;
            break;
        case 0x01FFFF09:
            time = 65;
            break;
        }

        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            if (!(sng_fout_term & (1 << i)))
            {
                sng_fade_time[i] = time;
            }
        }

        sng_fadein_time = 0;
        return 0;
    }

    return -1;
}

int SngFadeOutS(unsigned int code)
{
    int time = 1;

    if ((sng_status != 0) && ((sng_fout_term != -1) || (sng_fadein_time != 0)))
    {
        switch (code)
        {
        case 0x01FFFF0A:
            time = 655;
            break;
        case 0x01FFFF0B:
            time = 327;
            break;
        case 0x01FFFF0C:
            time = 109;
            break;
        case 0x01FFFF0D:
            time = 65;
            break;
        }

        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            if (!(sng_fout_term & (1 << i)))
            {
                sng_fade_time[i] = time;
            }
        }

        sng_fadein_time = 0;
        sng_play_code = 0xFFFFFFFF;

        SD_PRINT("SNG FADEOUT START(status=%x)\n", sng_status);
        return 0;
    }

    SD_PRINT("SNG FADEOUT CANCELED(status=%x)\n", sng_status);
    return -1;
}

int SngKaihiP(void)
{
    /* TODO */

    if (sng_kaihi_fg)
    {
        for (int i = 0; i < 16; i++)
        {
            // mix_fader[i].field_8 = 0xffff;
            // mix_fader[i].field_0 = (mix_fader[i].field_8 - mix_fader[i].field_4) / 100;
        }

        for (int i = 16; i < 32; i++)
        {
            // mix_fader[i].field_8 = 0;
            // mix_fader[i].field_0 = (mix_fader[i].field_8 - mix_fader[i].field_4) / 400;
        }

        sng_kaihi_fg = 0;
    }
    else
    {
        for (int i = 0; i < 16; i++)
        {
            // mix_fader[i].field_8 = 0;
            // mix_fader[i].field_0 = (mix_fader[i].field_8 - mix_fader[i].field_4) / 1200;
        }

        for (int i = 16; i < 32; i++)
        {
            // mix_fader[i].field_8 = 0xffff;
            // mix_fader[i].field_0 = (mix_fader[i].field_8 - mix_fader[i].field_4) / 1000;
        }

        sng_kaihi_fg = 1;
    }

    return 0;
}

void SngFadeWkSet(void)
{
    if (sng_fadein_fg == 0)
    {
         sng_fadein_time = 0;

        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            sng_fade_time[i] = 0;
        }

        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            sng_fade_value[i] = 0;
        }

        sng_fout_term = 0;
        sng_fout_fg = 0;
        return;
    }

    switch (sng_fadein_fg)
    {
    case 0x01FFFF03: /* fallthrough */
    case 0x01FFFF04: /* fallthrough */
    case 0x01FFFF05:
        SngFadeIn(sng_fadein_fg);

        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            sng_fade_value[i] = 65536;
        }

        sng_fadein_fg = 0;
        break;
    }

    sng_fout_term = 0;
    sng_fout_fg = 0;
}

void SngFadeInt(void)
{
    int has_fade_time;
    int has_fade_value;
    int mod;
    int vol;
    int fade;

    has_fade_time = 0;
    has_fade_value = 0;

    if (sng_status < 3)
    {
        return;
    }

    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        has_fade_time |= sng_fade_time[i];
    }

    if (has_fade_time != 0)
    {
        for (int i = 0; i < SD_BGM_VOICES; i++)
        {
            if (sng_fade_time[i] == 0)
            {
                continue;
            }

            sng_fade_value[i] += sng_fade_time[i];

            if (sng_fade_value[i] >= 0x10000)
            {
                sng_fout_term |= (1 << i);
                sng_fade_value[i] = 0x10000;
                sng_fade_time[i] = 0;
            }

            if (sng_fout_term == -1)
            {
                if (sng_play_code == 0xFFFFFFFF)
                {
                    sng_status = 4;
                }
                else
                {
                    sng_fout_fg = 1;
                }
            }
            else
            {
                sng_fout_fg = 0;
            }
        }
    }
    else
    {
        if (dword_800BF154 != 0)
        {
            dword_800BF154 -= 245;

            if (dword_800BF154 < 0)
            {
                dword_800BF154 = 0;
            }
        }

        if (sng_fadein_time != 0)
        {
            for (int i = 0; i < SD_BGM_VOICES; i++)
            {
                if (sng_fadein_time >= sng_fade_value[i])
                {
                    sng_fade_value[i] = 0;
                }
                else
                {
                    sng_fade_value[i] -= sng_fadein_time;
                }

                has_fade_value |= sng_fade_value[i];
            }

            if (has_fade_value == 0)
            {
                sng_fadein_time = 0;
            }
        }
    }

    if (sng_syukan_fg != 0)
    {
        if (sng_syukan_vol < 0x5000)
        {
            sng_syukan_vol += 204;

            if (sng_syukan_vol > 0x5000)
            {
                sng_syukan_vol = 0x5000;
            }
        }
    }
    else
    {
        if (sng_syukan_vol != 0)
        {
            sng_syukan_vol -= 102;

            if (sng_syukan_vol < 0)
            {
                sng_syukan_vol = 0;
            }
        }
    }

    if (dword_800BF154 <= sng_syukan_vol)
    {
        mod = sng_syukan_vol;
    }
    else
    {
        mod = dword_800BF154;
    }

    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        vol = 0x10000;

        if (mod < sng_fade_value[i])
        {
            fade = sng_fade_value[i];
        }
        else
        {
            fade = mod;
        }

        if (fade > vol)
        {
            vol = 0;
        }
        else
        {
            vol -= fade;
        }

        /* TODO */
        // if (mix_fader[i].field_4 != mix_fader[i].field_8)
        // {
        //     mix_fader[i].field_4 += mix_fader[i].field_0;
        //
        //     if (mix_fader[i].field_0 < 0)
        //     {
        //         if (mix_fader[i].field_4 < mix_fader[i].field_8)
        //         {
        //             mix_fader[i].field_4 = mix_fader[i].field_8;
        //             mix_fader[i].field_0 = 0;
        //         }
        //     }
        //     else if (mix_fader[i].field_4 > mix_fader[i].field_8)
        //     {
        //         mix_fader[i].field_4 = mix_fader[i].field_8;
        //         mix_fader[i].field_0 = 0;
        //     }
        // }
        // sng_master_vol[i] = (vol * mix_fader[i].field_4) / 0xffff;

        sng_master_vol[i] = vol;
    }
}

void SngTempoInt(void)
{
    /* do nothing */
}

void init_sng_work(void)
{
    SOUND_W *track;

    for (mtrack = 0; mtrack < SD_N_VOICES; mtrack++)
    {
        track = &sound_w[mtrack];
        sptr = track;
        track->mpointer = 0;
        track->lp3_addr = 0;
        track->lp2_addr = 0;
        track->lp1_addr = 0;
        sng_track_init(track);
    }

    keyons = 0;
    keyoffs = 0;
    sng_play_code = 0;
}

void sng_adrs_set(int num)
{
    int song_addr;
    int track_addr;

    song_addr  = sng_data[(num & 0xF) * 4 + 2] << 16;
    song_addr += sng_data[(num & 0xF) * 4 + 1] << 8;
    song_addr += sng_data[(num & 0xF) * 4];

    song_end = 0;
    song_loop_end &= 0;

    for (int i = 0; i < SD_BGM_VOICES; i++)
    {
        track_addr  = sng_data[song_addr + i * 4 + 2] << 16;
        track_addr += sng_data[song_addr + i * 4 + 1] << 8;
        track_addr += sng_data[song_addr + i * 4];

        if (track_addr != 0)
        {
            sound_w[i].mpointer = &sng_data[track_addr];
            sng_track_init(&sound_w[i]);
        }
        else
        {
            song_end |= 1 << i;
            song_loop_end |= 1 << i;
        }
    }

    keyons = 0;
}

void se_adrs_set(int num)
{
    se_playing[num].code      = se_request[num].code;
    se_playing[num].pri       = se_request[num].pri;
    se_playing[num].kind      = se_request[num].kind;
    se_playing[num].character = se_request[num].character;
    se_playing[num].addr      = se_request[num].addr;

    se_request[num].code      = 0;
    se_request[num].pri       = 0;
    se_request[num].character = 0;

    sng_track_init(&sound_w[num + SD_SE_START]);

    se_vol[num] = (se_playing[num].code & 0x3f000) >> 12;
    se_pan[num] = (se_playing[num].code & 0xfc0000) >> 18;

    sound_w[num + SD_SE_START].mpointer = se_playing[num].addr;
}

void sng_track_init(SOUND_W *track)
{
    track->ngc      = 1;
    track->pvod     = 127;
    track->vol      = 127;
    track->pand     = 5120;
    track->panf     = 20;
    track->tmpd     = 1;
    track->rdmd     = 0;
    track->ngo      = 0;
    track->ngs      = 0;
    track->ngg      = 0;
    track->lp1_cnt  = 0;
    track->lp2_cnt  = 0;
    track->lp1_vol  = 0;
    track->lp2_vol  = 0;
    track->lp1_freq = 0;
    track->lp2_freq = 0;
    track->pvoc     = 0;
    track->panoff   = 0;
    track->panmod   = 0;
    track->swsk     = 0;
    track->swsc     = 0;
    track->vibd     = 0;
    track->vibdm    = 0;
    track->tred     = 0;
    track->snos     = 0;
    track->ptps     = 0;
    track->dec_vol  = 0;
    track->tund     = 0;
    track->tmp      = 255;
    track->tmpc     = 0;
}
