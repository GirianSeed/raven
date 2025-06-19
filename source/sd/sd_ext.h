#ifndef _SD_EXT_H_
#define _SD_EXT_H_

// Do not #include this file externally! Use sd_cli.h instead.

#include "sd_incl.h"

/* sd_main.c */
void sd_init(int debug);
void sd_term(void);
void sd_tick(void);

/* sd_file.c */
int sd_sng_data_load(char *name);
int sd_se_data_load(char *name);
int sd_wav_data_load(char *name);

/* sd_drv.c */
void IntSdMain(void);
void SngFadeIn(unsigned int mode);
int SngFadeOutP(unsigned int code);
int SngFadeOutS(unsigned int code);
int SngKaihiP(void);
void SngFadeWkSet(void);
void SngFadeInt(void);
void SngTempoInt(void);
void init_sng_work(void);
void sng_adrs_set(int idx);
void se_adrs_set(int idx);
void sng_track_init(SOUND_W *ptr);

/* sd_sub1.c */
int sound_sub(void);
int tx_read(void);
void note_set(void);
void adsr_reset(void);
void note_compute(void);
void vol_compute(void);
void pan_generate(void);
void key_cut_off(void);
void keych(void);
int vib_generate(int cnt);
void bendch(void);
void note_cntl(void);
unsigned int random(void);
void tempo_ch(void);
void volxset(unsigned char depth);

/* sd_sub2.c */
void rest_set(void);
void tie_set(void);
void sno_set(void);
void svl_set(void);
void svp_set(void);
void use_set(void);
void pan_set(void);
void pan_move(void);
void vib_set(void);
void vib_change(void);
void rdm_set(void);
void lp1_start(void);
void lp1_end(void);
void lp2_start(void);
void lp2_end(void);
void l3s_set(void);
void l3e_set(void);
void tempo_set(void);
void tempo_move(void);
void trans_set(void);
void tre_set(void);
void vol_chg(void);
void vol_move(void);
void por_set(void);
void sws_set(void);
void detune_set(void);
void swp_set(void);
void echo_set1(void);
void echo_set2(void);
void eon_set(void);
void eof_set(void);
void kakko_start(void);
void kakko_end(void);
void env_set(void);
void ads_set(void);
void srs_set(void);
void rrs_set(void);
void pm_set(void);
void jump_set(void);
void block_end(void);
void no_cmd(void);

/* sd_ioset.c */
void spuwr(void);
void sound_off(void);
void sng_off(void);
void se_off(int i);
void sng_pause(void);
void sng_pause_off(void);
void keyon(void);
void keyoff(void);
void tone_set(unsigned char n);
void pan_set2(unsigned char x);
void vol_set(unsigned int vol_data);
void freq_set(unsigned int note_tune);
void drum_set(unsigned char n);

/* sd_cli.c */
int sd_sng_play(void);
int sd_se_play(void);
int sd_set_cli(int sound_code, int sync_mode);
int sd_sng_code(void);

/* in se_tbl.c */
extern SETBL se_tbl[128];

/* in sd_wk.c */
extern unsigned int  spu_ch_tbl[24+1];

/*---------------------------------------------------------------------------*/
extern  int            sd_debug_mode;
extern  int            se_tracks;
extern  int            sd_sng_code_buf[16];
extern  int            eons;
extern  SEPLAYTBL      se_playing[8];
extern  unsigned int   mdata1;
extern  unsigned int   mdata2;
extern  unsigned int   mdata3;
extern  unsigned int   mdata4;
extern  SEPLAYTBL      se_request[8];
extern  int            sng_status;
extern  int            stop_jouchuu_se;
extern  int            se_pan[8];
extern  unsigned int   mtrack;
extern  int            se_vol[8];
extern  int            eoffs;
extern  unsigned int   keyons;
extern  SETBL          se_header[512];
extern  unsigned int   keyoffs;
extern  unsigned char  sng_data[0x4000];
extern  unsigned int   song_end;
extern  unsigned int   sng_play_code;
extern  int            sound_mono_fg;
extern  unsigned int   keyd;
extern  SETBL          se_exp_table[128];
extern  unsigned int   spu_wave_start_ptr;
extern  WAVE_W         voice_tbl[256];
extern  unsigned char *mptr;
extern  int            se_rev_on;
extern  SOUND_W       *sptr;
extern  SPU_TRACK_REG  spu_tr_wk[23];
extern  int            sng_kaihi_fadein_time;
extern  int            sng_master_vol[13];

#endif // _SD_EXT_H_
