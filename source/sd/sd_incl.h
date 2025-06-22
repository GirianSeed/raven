#ifndef _SD_INCL_H_
#define _SD_INCL_H_

typedef struct WAVE_H
{
    unsigned int   offset;
    unsigned int   size;
    char           unused[8];
} WAVE_H;

typedef struct WAVE_W
{
    unsigned int   addr;
    char           sample_note;
    char           sample_tune;
    unsigned char  a_mode;
    unsigned char  ar;
    unsigned char  dr;
    unsigned char  s_mode;
    unsigned char  sr;
    unsigned char  sl;
    unsigned char  r_mode;
    unsigned char  rr;
    unsigned char  pan;
    unsigned char  decl_vol;
} WAVE_W;

typedef struct SPU_TRACK_REG
{
    unsigned short vol_l;
    unsigned short vol_r;
    int            vol_fg;
    unsigned short pitch;
    int            pitch_fg;
    unsigned int   addr;
    int            addr_fg;
    int            a_mode;
    unsigned short ar;
    unsigned short dr;
    int            env1_fg;
    int            s_mode;
    unsigned short sr;
    unsigned short sl;
    int            env2_fg;
    int            r_mode;
    unsigned short rr;
    int            env3_fg;
} SPU_TRACK_REG;

typedef struct SETBL
{
    unsigned char  pri;
    unsigned char  tracks;
    unsigned char  kind;
    unsigned char  character;
    unsigned char *addr[3];
} SETBL;

typedef struct SEPLAYTBL
{
    unsigned char  pri;
    unsigned char  kind;
    unsigned char  character;
    unsigned char *addr;
    unsigned int   code;
} SEPLAYTBL;

typedef struct SOUND_W
{
    /* command stream pointer */
    unsigned char *mpointer;

    /* note */
    unsigned char  ngc;
    unsigned char  ngo;
    unsigned char  ngs;
    unsigned char  ngg;

    /* loop */
    unsigned char  lp1_cnt;
    unsigned char  lp2_cnt;
    int            lp1_vol;
    int            lp2_vol;
    int            lp1_freq;
    int            lp2_freq;
    unsigned char *lp1_addr;
    unsigned char *lp2_addr;
    unsigned char *lp3_addr;

    /* kakko (?) */
    unsigned char  kakfg;
    unsigned char *kak1ptr;
    unsigned char *kak2ptr;

    unsigned char  pvoc;
    int            pvod;
    int            pvoad;
    unsigned int   pvom;

    /* volume */
    unsigned char  vol;

    /* pan */
    unsigned char  panc;
    int            pand;
    int            panad;
    int            panm;
    unsigned char  panf;
    char           panoff;
    char           panmod;

    unsigned char  swpc;
    unsigned char  swphc;
    unsigned int   swpd;
    int            swpad;
    unsigned int   swpm;
    unsigned char  swsc;
    unsigned char  swshc;
    char           swsk;
    int            swss;

    /* vibrato */
    unsigned char  vibhc;
    unsigned int   vib_tmp_cnt;
    unsigned char  vib_tbl_cnt;
    unsigned char  vib_tc_ofst;
    unsigned char  vibcc;
    unsigned int   vibd;
    unsigned int   vibdm;
    unsigned char  vibhs;
    unsigned char  vibcs;
    unsigned char  vibcad;
    unsigned int   vibad;

    /* random */
    unsigned int   rdmc;
    unsigned int   rdmo;
    unsigned char  rdms;
    unsigned int   rdmd;

    /* tremolo */
    signed char    trec;
    unsigned char  trehc;
    unsigned char  tred;
    unsigned char  trecad;
    unsigned char  trehs;

    unsigned int   snos;
    int            ptps;

    /* decline volume */
    unsigned int   dec_vol;

    /* detune */
    int            tund;

    /* tempo */
    unsigned int   tmpd;
    unsigned char  tmp;
    unsigned int   tmpad;
    unsigned char  tmpc;
    unsigned int   tmpw;
    unsigned char  tmpm;

    /* unused */
    unsigned int   rest_fg;

    /* tone */
    unsigned char  macro;
    signed char    micro;

    /* reset release decay */
    unsigned short rrd;
} SOUND_W;

#endif // _SD_INCL_H_
