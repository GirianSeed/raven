#ifndef _SD_CLI_H_
#define _SD_CLI_H_

#define SD_SYNC  1
#define SD_ASYNC 0

/* sd_cli.c */
int sd_sng_play(void);
int sd_se_play(void);
int sd_set_cli(int sound_code, int sync_mode);
int sd_sng_code(void);

/* sd_main.c */
void sd_init(int debug, int loop);
void sd_term(void);
void sd_tick(void);

/* sd_file.c */
int sd_sng_data_load(const char *name);
int sd_se_data_load(const char *name);
int sd_wav_data_load(const char *name);

#endif // _SD_CLI_H_
