#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <assert.h>

static void (*cntl_tbl[128])(void) = {
    /* 0x00 */ no_cmd,
    /* 0x01 */ no_cmd,
    /* 0x02 */ no_cmd,
    /* 0x03 */ no_cmd,
    /* 0x04 */ no_cmd,
    /* 0x05 */ no_cmd,
    /* 0x06 */ no_cmd,
    /* 0x07 */ no_cmd,
    /* 0x08 */ no_cmd,
    /* 0x09 */ no_cmd,
    /* 0x0a */ no_cmd,
    /* 0x0b */ no_cmd,
    /* 0x0c */ no_cmd,
    /* 0x0d */ no_cmd,
    /* 0x0e */ no_cmd,
    /* 0x0f */ no_cmd,
    /* 0x10 */ no_cmd,
    /* 0x11 */ no_cmd,
    /* 0x12 */ no_cmd,
    /* 0x13 */ no_cmd,
    /* 0x14 */ no_cmd,
    /* 0x15 */ no_cmd,
    /* 0x16 */ no_cmd,
    /* 0x17 */ no_cmd,
    /* 0x18 */ no_cmd,
    /* 0x19 */ no_cmd,
    /* 0x1a */ no_cmd,
    /* 0x1b */ no_cmd,
    /* 0x1c */ no_cmd,
    /* 0x1d */ no_cmd,
    /* 0x1e */ no_cmd,
    /* 0x1f */ no_cmd,
    /* 0x20 */ no_cmd,
    /* 0x21 */ no_cmd,
    /* 0x22 */ no_cmd,
    /* 0x23 */ no_cmd,
    /* 0x24 */ no_cmd,
    /* 0x25 */ no_cmd,
    /* 0x26 */ no_cmd,
    /* 0x27 */ no_cmd,
    /* 0x28 */ no_cmd,
    /* 0x29 */ no_cmd,
    /* 0x2a */ no_cmd,
    /* 0x2b */ no_cmd,
    /* 0x2c */ no_cmd,
    /* 0x2d */ no_cmd,
    /* 0x2e */ no_cmd,
    /* 0x2f */ no_cmd,
    /* 0x30 */ no_cmd,
    /* 0x31 */ no_cmd,
    /* 0x32 */ no_cmd,
    /* 0x33 */ no_cmd,
    /* 0x34 */ no_cmd,
    /* 0x35 */ no_cmd,
    /* 0x36 */ no_cmd,
    /* 0x37 */ no_cmd,
    /* 0x38 */ no_cmd,
    /* 0x39 */ no_cmd,
    /* 0x3a */ no_cmd,
    /* 0x3b */ no_cmd,
    /* 0x3c */ no_cmd,
    /* 0x3d */ no_cmd,
    /* 0x3e */ no_cmd,
    /* 0x3f */ no_cmd,
    /* 0x40 */ no_cmd,
    /* 0x41 */ no_cmd,
    /* 0x42 */ no_cmd,
    /* 0x43 */ no_cmd,
    /* 0x44 */ no_cmd,
    /* 0x45 */ no_cmd,
    /* 0x46 */ no_cmd,
    /* 0x47 */ no_cmd,
    /* 0x48 */ no_cmd,
    /* 0x49 */ no_cmd,
    /* 0x4a */ no_cmd,
    /* 0x4b */ no_cmd,
    /* 0x4c */ no_cmd,
    /* 0x4d */ no_cmd,
    /* 0x4e */ no_cmd,
    /* 0x4f */ no_cmd,
    /* 0x50 */ tempo_set,
    /* 0x51 */ tempo_move,
    /* 0x52 */ sno_set,
    /* 0x53 */ svl_set,
    /* 0x54 */ svp_set,
    /* 0x55 */ vol_chg,
    /* 0x56 */ vol_move,
    /* 0x57 */ ads_set,
    /* 0x58 */ srs_set,
    /* 0x59 */ rrs_set,
    /* 0x5a */ no_cmd,
    /* 0x5b */ no_cmd,
    /* 0x5c */ no_cmd,
    /* 0x5d */ pan_set,
    /* 0x5e */ pan_move,
    /* 0x5f */ trans_set,
    /* 0x60 */ detune_set,
    /* 0x61 */ vib_set,
    /* 0x62 */ vib_change,
    /* 0x63 */ rdm_set,
    /* 0x64 */ swp_set,
    /* 0x65 */ sws_set,
    /* 0x66 */ por_set,
    /* 0x67 */ lp1_start,
    /* 0x68 */ lp1_end,
    /* 0x69 */ lp2_start,
    /* 0x6a */ lp2_end,
    /* 0x6b */ l3s_set,
    /* 0x6c */ l3e_set,
    /* 0x6d */ kakko_start,
    /* 0x6e */ kakko_end,
    /* 0x6f */ no_cmd,
    /* 0x70 */ no_cmd,
    /* 0x71 */ use_set,
    /* 0x72 */ rest_set,
    /* 0x73 */ tie_set,
    /* 0x74 */ echo_set1,
    /* 0x75 */ echo_set2,
    /* 0x76 */ eon_set,
    /* 0x77 */ eof_set,
    /* 0x78 */ no_cmd,
    /* 0x79 */ no_cmd,
    /* 0x7a */ no_cmd,
    /* 0x7b */ no_cmd,
    /* 0x7c */ no_cmd,
    /* 0x7d */ no_cmd,
    /* 0x7e */ no_cmd,
    /* 0x7f */ block_end
};

static unsigned char rdm_tbl[129] = {
    159,  60, 178,  82, 175,  69, 199, 137,
     16, 127, 224, 157, 220,  31,  97,  22,
     57, 201, 156, 235,  87,   8, 102, 248,
     90,  36, 191,  14,  62,  21,  75, 219,
    171, 245,  49,  12,  67,   2,  85, 222,
     65, 218, 189, 174,  25, 176,  72,  87,
    186, 163,  54,  11, 249, 223,  23, 168,
      4,  12, 224, 145,  24,  93, 221, 211,
     40, 138, 242,  17,  89, 111,   6,  10,
     52,  42, 121, 172,  94, 167, 131, 198,
     57, 193, 180,  58,  63, 254,  79, 239,
     31,   0,  48, 153,  76,  40, 131, 237,
    138,  47,  44, 102,  63, 214, 108, 183,
     73,  34, 188, 101, 250, 207,   2, 177,
     70, 240, 154, 215, 226,  15,  17, 197,
    116, 246, 122,  44, 143, 251,  25, 106,
    229
};

static unsigned char VIBX_TBL[32] = {
      0, 32,   56,  80, 104, 128, 144, 160,
    176, 192, 208, 224, 232, 240, 240, 248,
    255, 248, 244, 240, 232, 224, 208, 192,
    176, 160, 144, 128, 104,  80,  56,  32
};


int sound_sub(void)
{
    int fade;

    key_fg = 0;
    sptr->tmpd += sptr->tmp;

    if (mtrack < SD_BGM_VOICES && sng_kaihi_fadein_time != 0)
    {
        fade = sng_kaihi_fadein_time >> 5;

        if (fade < sptr->tmp)
        {
            sptr->tmpd -= fade;
        }
    }

    if (sptr->tmpd > 0xff)
    {
        sptr->tmpd &= 0xff;

        sptr->ngc--;
        if (sptr->ngc != 0)
        {
            keych();
        }
        else if (tx_read())
        {
            keyoff();
            return 1;
        }

        tempo_ch();
        bendch();
        vol_compute();
    }
    else
    {
        note_cntl();
    }

    if (key_fg)
    {
        keyon();
    }

    return 0;
}

int tx_read(void)
{
    int read_fg = 1;
    int loop_count = 0;

    while (read_fg)
    {
        loop_count++;
        if (loop_count == 256)
        {
            return 1;
        }

        mdata1 = mptr[3];
        if (mdata1 == 0)
        {
            return 1;
        }
        mdata2 = mptr[2];
        mdata3 = mptr[1];
        mdata4 = mptr[0];
        mptr += 4;

        assert((mdata1 & 0xFFFFFF00) == 0);

        if (mdata1 >= 128)
        {
            cntl_tbl[mdata1 - 128]();
            if (mdata1 == 0xF2 || mdata1 == 0xF3 || mdata1 == 0xFF)
            {
                read_fg = 0;
            }

            if (mdata1 == 0xFF)
            {
                return 1;
            }
        }
        else
        {
            if (sptr->ngg < 100 && mdata4 != 0)
            {
                key_fg = 1;
            }

            read_fg = 0;
            sptr->rest_fg = 0;
            note_set();
        }
    }

    return 0;
}

void note_set(void)
{
    int ngo;

    sptr->ngs = mdata2;
    sptr->ngg = mdata3;
    sptr->ngc = sptr->ngs;

    ngo = (sptr->ngg * sptr->ngc) / 100;
    sptr->ngo = ngo ? ngo : 1;

    sptr->vol = (mdata4 & 0x7F);
    note_compute();
}

void adsr_reset(void)
{
    spu_tr_wk[mtrack].rr = sptr->rrd;
    spu_tr_wk[mtrack].env3_fg = 1;
}

static void swpadset(int xfreq)
{
    unsigned int div;

    if (sptr->swpc)
    {
        div = sptr->swpc << 8;
        div = div / sptr->tmp;

        if (xfreq < 0)
        {
            xfreq = 0;
        }
        else if (xfreq > 0x5fff)
        {
            xfreq = 0x5fff;
        }

        sptr->swpm = xfreq;

        xfreq -= sptr->swpd;
        if (xfreq < 0)
        {
            xfreq = -xfreq / div;
            sptr->swpad = -xfreq;
        }
        else
        {
            sptr->swpad = xfreq / div;
        }
    }
}

void note_compute(void)
{
    int      x;
    int      swp_ex;
    SOUND_W *pSound;

    if (mdata1 >= 0x48)
    {
        drum_set(mdata1);
        x = 0x24;
    }
    else
    {
        x = mdata1;
    }

    x += sptr->ptps;
    x = (x << 8) + sptr->tund;
    x = x + sptr->lp1_freq + sptr->lp2_freq;

    while (x >= 0x6000)
    {
        x -= 0x6000;
    }

    swp_ex = sptr->swpd;

    pSound = sptr;
    pSound->vibcc = 0;
    pSound->vibhc = 0;
    pSound->swpd = x;

    sptr->vib_tmp_cnt = 0;
    sptr->vib_tbl_cnt = 0;

    pSound = sptr;
    pSound->trehc = 0;
    pSound->trec = 0;
    pSound->vibd = 0;

    spu_tr_wk[mtrack].rr = sptr->rrd;
    spu_tr_wk[mtrack].env3_fg = 1;

    sptr->swpc = sptr->swsc;

    if (sptr->swpc != 0)
    {
        sptr->swphc = sptr->swshc;

        if (sptr->swsk == 0)
        {
            x = sptr->swpd;

            if (sptr->swss >= 0x7F01)
            {
                sptr->swpd += 0x10000 - (sptr->swss & 0xFFFF);
            }
            else
            {
                sptr->swpd -= sptr->swss;
            }

            swpadset(x);
        }
        else
        {
            sptr->swpm = sptr->swpd;
            sptr->swpd = swp_ex;
        }
    }

    freq_set(sptr->swpd);
}

void vol_compute(void)
{
    int          mult;
    unsigned int depth;

    if (sptr->pvoc != 0)
    {
        if (--sptr->pvoc == 0)
        {
            sptr->pvod = sptr->pvom << 8;
        }
        else
        {
            sptr->pvod += sptr->pvoad;
        }
    }

    if (sptr->vol != 0)
    {
        if (sptr->tred == 0)
        {
            depth = 0;
        }
        else
        {
            if (sptr->trehs == sptr->trehc)
            {
                sptr->trec += sptr->trecad;
                mult = sptr->trec;
                if (mult < 0)
                {
                    depth = sptr->tred * -mult;
                }
                else if (mult == 0)
                {
                    depth = 1;
                }
                else
                {
                    depth = sptr->tred * mult;
                }
            }
            else
            {
                sptr->trehc++;
                depth = 0;
            }
        }
        volxset(depth >> 8);
    }
    pan_generate();
}

void pan_generate(void)
{
    if (sptr->panc != 0)
    {
        if (--sptr->panc == 0)
        {
            sptr->pand = sptr->panm;
        }
        else
        {
            sptr->pand += sptr->panad;
        }

        sptr->panf = sptr->pand >> 8;
    }
}

void key_cut_off(void)
{
    if (sptr->rrd > 7)
    {
        spu_tr_wk[mtrack].rr = 7;
        spu_tr_wk[mtrack].env3_fg = 1;
    }
}

static void por_compute(void)
{
    int          freq;
    unsigned int lo, hi;

    freq = sptr->swpm - sptr->swpd;
    if (freq < 0)
    {
        freq = -freq;
        lo = freq & 0xFF;
        hi = freq >> 8;
        lo = (lo * sptr->swsc) >> 8;
        hi *= sptr->swsc;
        freq = hi + lo;

        if (freq == 0)
        {
            freq = 1;
        }
        freq = -freq;
    }
    else if (freq == 0)
    {
        sptr->swpc = 0;
    }
    else
    {
        lo = freq & 0xFF;
        hi = freq >> 8;
        lo = (lo * sptr->swsc) >> 8;
        hi *= sptr->swsc;
        freq = hi + lo;

        if (freq == 0)
        {
            freq = 1;
        }
    }

    sptr->swpd += freq;
}

static int vib_compute(void)
{
    int          tbl;
    unsigned int vib;

    sptr->vib_tbl_cnt = (sptr->vib_tbl_cnt + sptr->vib_tc_ofst) & 0x3f;
    tbl = VIBX_TBL[sptr->vib_tbl_cnt & 0x1F];

    if (sptr->vibd < 0x8000)
    {
        vib = ((sptr->vibd >> 7) & 0xFE);
        vib = (vib * tbl) >> 8;
    }
    else
    {
        vib = ((sptr->vibd >> 8) & 0x7F) + 2;
        vib = (vib * tbl) >> 1;
    }

    if (sptr->vib_tbl_cnt >= 32)
    {
        vib = -vib;
    }

    return vib;
}

void keych(void)
{
    int set_fg;
    int vib;
    int rdm;

    if (sptr->ngg < 100 && sptr->ngc == 1 && sptr->rrd >= 8)
    {
        spu_tr_wk[mtrack].rr = 7;
        spu_tr_wk[mtrack].env3_fg = 1;
    }

    if (sptr->ngo && --sptr->ngo == 0)
    {
        keyoff();
    }

    set_fg = 0;

    if (sptr->swpc)
    {
        if (sptr->swphc != 0)
        {
            sptr->swphc--;
        }
        else if (sptr->swsk == 0)
        {
            if (--sptr->swpc == 0)
            {
                sptr->swpd = sptr->swpm;
            }
            else
            {
                sptr->swpd += sptr->swpad;
            }

            set_fg = 1;
        }
        else
        {
            por_compute();
            set_fg = 1;
        }
    }

    vib = 0;

    if (sptr->vibdm != 0)
    {
        if (sptr->vibhc != sptr->vibhs)
        {
            sptr->vibhc++;
        }
        else
        {
            if (sptr->vibcc == sptr->vibcs)
            {
                sptr->vibd = sptr->vibdm;
            }
            else
            {
                if (sptr->vibcc != 0)
                {
                    sptr->vibd += sptr->vibad;
                }
                else
                {
                    sptr->vibd = sptr->vibad;
                }

                sptr->vibcc++;
            }

            sptr->vib_tmp_cnt += sptr->vibcad;
            if (sptr->vib_tmp_cnt > 0xff)
            {
                sptr->vib_tmp_cnt &= 0xff;
                vib = vib_compute();
                set_fg = 1;
            }
        }
    }

    rdm = random_gen();
    if (rdm != 0)
    {
        vib += rdm;
        set_fg = 1;
    }

    if (set_fg)
    {
        freq_set(sptr->swpd + vib);
    }
}

int vib_generate(int cnt)
{
    unsigned char factor;
    int           vib;

    if (cnt & (1 << 24))
    {
        factor = -cnt * 2;

        if (-cnt & (1 << 25))
        {
            factor = -factor;
        }

        vib = ((sptr->vibd >> 8) & 0xff) * (factor / 4);
        vib = -vib;
    }
    else
    {
        factor = cnt * 2;

        if (cnt & (1 << 25))
        {
            factor = -factor;
        }

        vib = ((sptr->vibd >> 8) & 0xff) * (factor / 4);
    }

    if (sptr->vibdm < 0x8000)
    {
        vib >>= 2;
    }

    return vib;
}

void bendch(void)
{
    int freq;

    if (sptr->swpc != 0)
    {
        return;
    }

    mdata1 = mptr[3];

    if (mdata1 != 0xe4)
    {
        return;
    }

    sptr->swphc = mptr[2];
    sptr->swpc = mptr[1];

    freq = mptr[0];
    mptr += 4;

    freq = (freq + sptr->ptps) << 8;
    freq += sptr->tund;

    swpadset(freq);
}

void note_cntl(void)
{
    int            rdm_data;
    int            fset_fg;
    unsigned int   depth;
    int            frq_data;

    if (sptr->vol != 0 && sptr->tred != 0 && sptr->trehs == sptr->trehc)
    {
        sptr->trec += (sptr->trecad * (sptr->tmpd & 0xff)) >> 8;

        if (sptr->trec < 0)
        {
            depth = sptr->tred * -sptr->trec;
        }
        else if (sptr->trec == 0)
        {
            depth = 1;
        }
        else
        {
            depth = sptr->tred * sptr->trec;
        }

        volxset(depth >> 8);
    }

    fset_fg = 0;
    frq_data = sptr->swpd;

    if (sptr->swpc != 0 && sptr->swphc == 0)
    {
        fset_fg = 1;

        if (sptr->swsk == 0)
        {
            sptr->swpd += sptr->swpad;
        }
        else
        {
            por_compute();
        }

        frq_data = sptr->swpd;
    }

    if (sptr->vibd != 0 && sptr->vibhs == sptr->vibhc)
    {
        sptr->vib_tmp_cnt += sptr->vibcad;

        if (sptr->vib_tmp_cnt > 255)
        {
            sptr->vib_tmp_cnt &= 0xFF;
            frq_data += vib_compute();
            fset_fg = 1;
        }
    }

    rdm_data = random_gen();

    if (rdm_data != 0)
    {
        fset_fg = 1;
        frq_data += rdm_data;
    }

    if (fset_fg)
    {
        freq_set(frq_data);
    }
}

unsigned int random_gen(void)
{
    unsigned int val = 0;

    if (sptr->rdms != 0)
    {
        sptr->rdmc += sptr->rdms;

        if (sptr->rdmc > 256)
        {
            sptr->rdmc &= 255;
            sptr->rdmo = (sptr->rdmo + 1) & 0x7f;

            val = rdm_tbl[sptr->rdmo];
            val += rdm_tbl[sptr->rdmo + 1] << 8;
            val &= sptr->rdmd;
        }
    }

    return val;
}

void tempo_ch(void)
{
    if (sptr->tmpc)
    {
        if (!--sptr->tmpc)
        {
            sptr->tmpw = sptr->tmpm << 8;
        }
        else
        {
            sptr->tmpw += sptr->tmpad;
        }
        sptr->tmp = sptr->tmpw >> 8;
    }
}

void volxset(unsigned char depth)
{
    int vol;
    int pvod;

    vol = sptr->vol;
    vol -= depth;
    vol += sptr->lp1_vol;
    vol += sptr->lp2_vol;

    if (vol < 0)
    {
        vol = 0;
    }
    else if (vol >= 128)
    {
        vol = 127;
    }

    pvod = (sptr->pvod >> 8) & 0xFF;
    vol_set(((pvod * vol) >> 8) & 0xFF);
}
