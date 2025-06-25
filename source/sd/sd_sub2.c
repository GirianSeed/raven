#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

void rest_set(void)
{
    sptr->rest_fg = 1;
    keyoff();
    sptr->ngs = mdata2;
    sptr->ngg = 0;
    sptr->vol = 0;
    sptr->ngc = sptr->ngs;
    sptr->ngo = 0;
}

void tie_set(void)
{
    int ngo;

    sptr->rest_fg = 1;
    sptr->ngs = mdata2;
    sptr->ngg = mdata3;
    sptr->ngc = sptr->ngs;

    ngo = (sptr->ngg * sptr->ngc) / 100;
    sptr->ngo = ngo ? ngo : 1;
}

void sno_set(void)
{
    sptr->snos = mdata2;
    keyoff();
    tone_set(mdata2);
}

void svl_set(void)
{
    sptr->snos = mdata2;
    keyoff();
    tone_set(mdata2);
}

void svp_set(void)
{
    sptr->snos = mdata2;
    keyoff();
    tone_set(mdata2);
}

void use_set(void)
{
    /* do nothing */
}

void pan_set(void)
{
    sptr->panmod = mdata2;
    sptr->panf = mdata3 + 20;
    sptr->pand = sptr->panf << 8;
    sptr->panc = 0;
}

void pan_move(void)
{
    unsigned char shift;
    int pan;

    shift = (mdata3 + 20);

    sptr->panc = mdata2;
    sptr->panm = shift << 8;

    pan = shift - sptr->panf;
    if (pan < 0)
    {
        sptr->panad = -(((-pan) << 8) / mdata2);
        if (sptr->panad < -2032)
        {
            sptr->panad = -2032;
        }
    }
    else
    {
        sptr->panad = (pan << 8) / mdata2;
        if (sptr->panad > 2032)
        {
            sptr->panad = 2032;
        }

    }
}

void vib_set(void)
{
    sptr->vibhs = mdata2;
    sptr->vibcad = mdata3;

    if (sptr->vibcad < 32)
    {
        sptr->vib_tc_ofst = 1;
        sptr->vibcad = sptr->vibcad << 3;
    }
    else if (sptr->vibcad < 64)
    {
        sptr->vib_tc_ofst = 2;
        sptr->vibcad = sptr->vibcad << 2;
    }
    else if (sptr->vibcad < 128)
    {
        sptr->vib_tc_ofst = 4;
        sptr->vibcad = sptr->vibcad << 1;
    }
    else if (sptr->vibcad < 255)
    {
        sptr->vib_tc_ofst = 8;
    }
    else
    {
        sptr->vib_tc_ofst = 16;
    }

    sptr->vibd = mdata4 << 8;
    sptr->vibdm = mdata4 << 8;
}

void vib_change(void)
{
    sptr->vibcs = mdata2;
    sptr->vibad = sptr->vibdm / mdata2;
}

void rdm_set(void)
{
    sptr->rdms = mdata2;
    sptr->rdmd = (mdata3 << 8) + mdata4;
    sptr->rdmc = 0;
    sptr->rdmo = 0;
}

void lp1_start(void)
{
    sptr->lp1_addr = mptr;
    sptr->lp1_cnt = 0;
    sptr->lp1_freq = 0;
    sptr->lp1_vol = 0;
}

void lp1_end(void)
{
    if (stop_jouchuu_se && mdata2 == 0)
    {
        sptr->lp1_vol = 0;
        sptr->lp1_freq = 0;
        stop_jouchuu_se++;
        return;
    }

    sptr->lp1_cnt++;

    if (sptr->lp1_cnt != mdata2 || sptr->lp1_cnt == 0)
    {
        sptr->lp1_vol += (signed char)mdata3;
        sptr->lp1_freq += (signed char)mdata4 * 8;
        mptr = sptr->lp1_addr;
    }
    else
    {
        sptr->lp1_vol = 0;
        sptr->lp1_freq = 0;
    }
}

void lp2_start(void)
{
    sptr->lp2_addr = mptr;
    sptr->lp2_cnt = 0;
    sptr->lp2_freq = 0;
    sptr->lp2_vol = 0;
}

void lp2_end(void)
{
    sptr->lp2_cnt++;

    if (sptr->lp2_cnt != mdata2 || sptr->lp2_cnt == 0)
    {
        sptr->lp2_vol += (signed char)mdata3;
        sptr->lp2_freq +=  8 * (signed char)mdata4;
        mptr =  sptr->lp2_addr;
    }
}

void l3s_set(void)
{
    sptr->lp3_addr = mptr;
}

void l3e_set(void)
{
    sng_loop_cnt[mtrack]--;

    if (sng_loop_cnt[mtrack] > 0 && sptr->lp3_addr)
    {
        mptr = sptr->lp3_addr;
    }
    else
    {
        block_end();
    }
}

void tempo_set(void)
{
    sptr->tmp = mdata2;
}

void tempo_move(void)
{
    int tempo;

    sptr->tmpc = mdata2;
    sptr->tmpm = mdata3;
    sptr->tmpw = sptr->tmp << 8;

    tempo = sptr->tmpm - sptr->tmp;
    if (tempo < 0)
    {
        if (tempo < -127)
        {
            tempo = -127;
        }

        sptr->tmpad = -((-tempo << 8) / sptr->tmpc);
        if (sptr->tmpad < -2032u)
        {
            sptr->tmpad = -2032;
        }
    }
    else
    {
        if (tempo > 127)
        {
            tempo = 127;
        }

        sptr->tmpad = (tempo << 8) / sptr->tmpc;
        if (sptr->tmpad > 2032)
        {
            sptr->tmpad = 2032;
        }
    }
}

void trans_set(void)
{
    sptr->ptps = (signed char)mdata2;
}

void tre_set(void)
{
    sptr->trehs = mdata2;
    sptr->trecad = mdata3;
    sptr->tred = mdata4;
}

void vol_chg(void)
{
    sptr->pvod = mdata2 << 8;
    sptr->pvoc = 0;
}

void vol_move(void)
{
    int vol;

    sptr->pvoc = mdata2;
    sptr->pvom = mdata3;

    vol = (mdata3 << 8);
    vol = vol - sptr->pvod;
    if (vol < 0)
    {
        sptr->pvoad = -(-vol / sptr->pvoc);
        if (sptr->pvoad < -2032)
        {
            sptr->pvoad = -2032;
        }
    }
    else
    {
        sptr->pvoad = vol / sptr->pvoc;
        if (sptr->pvoad > 2032)
        {
            sptr->pvoad = 2032;
        }
    }
}

void por_set(void)
{
    sptr->swshc = 0;
    sptr->swsc = mdata2;

    if (mdata2 == 0)
    {
        sptr->swsk = 0;
    }
    else
    {
        sptr->swsk = 1;
    }
}

void sws_set(void)
{
    sptr->swsk = 0;
    sptr->swshc = mdata2;
    sptr->swsc = mdata3;
    sptr->swss = mdata4 << 8;
}

void detune_set(void)
{
    sptr->tund = (signed char)mdata2 << 2;
}

void swp_set(void)
{
    /* do nothing */
}

void echo_set1(void)
{
    /* do nothing */
}

void echo_set2(void)
{
    /* do nothing */
}

void eon_set(void)
{
    if (mtrack > 12 && mtrack < 21 && se_playing[mtrack - 13].kind == 0)
    {
        eons |= 1 << mtrack;
    }
}

void eof_set(void)
{
    if (mtrack > 12 && mtrack < 21 && se_playing[mtrack - 13].kind == 0)
    {
        eoffs |= 1 << mtrack;
    }
}

void kakko_start(void)
{
    sptr->kak1ptr = mptr;
    sptr->kakfg = 0;
}

void kakko_end(void)
{
    switch (sptr->kakfg)
    {
    case 0:
        sptr->kakfg++;
        break;

    case 1:
        sptr->kakfg++;
        sptr->kak2ptr = mptr;
        mptr = sptr->kak1ptr;
        break;

    case 2:
        sptr->kakfg--;
        mptr = sptr->kak2ptr;
        break;
    }
}

void env_set(void)
{
    /* do nothing */
}

void ads_set(void)
{
    spu_tr_wk[mtrack].a_mode = SPU_ADSR_LIN_INC;
    spu_tr_wk[mtrack].ar = ~mdata2 & 0x7F;
    spu_tr_wk[mtrack].dr = ~mdata3 & 0xF;
    spu_tr_wk[mtrack].sl = mdata4 & 0xF;
    spu_tr_wk[mtrack].env1_fg = 1;
}

void srs_set(void)
{
    spu_tr_wk[mtrack].s_mode = SPU_ADSR_LIN_DEC;
    spu_tr_wk[mtrack].sr = ~mdata2 & 0x7F;
    spu_tr_wk[mtrack].env2_fg = 1;
}

void rrs_set(void)
{
    spu_tr_wk[mtrack].r_mode = SPU_ADSR_LIN_DEC;
    spu_tr_wk[mtrack].rr = ~mdata2 & 0x1F;
    spu_tr_wk[mtrack].env3_fg = 1;

    sptr->rrd = spu_tr_wk[mtrack].rr;
}

void pm_set(void)
{
    /* do nothing */
}

void jump_set(void)
{
    /* do nothing */
}

void block_end(void)
{
    keyoffs |= keyd;
}

void no_cmd(void)
{
    /* do nothing */
}
