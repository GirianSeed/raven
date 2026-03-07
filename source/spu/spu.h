#ifndef _SPU_H_
#define _SPU_H_

#include <stddef.h>

#define SPU_NCH                 24

#define SPU_ADSR_LIN_INC        1
#define SPU_ADSR_LIN_DEC        3
#define SPU_ADSR_EXP_INC        5
#define SPU_ADSR_EXP_DEC        7

#define SPU_REV_MODE_OFF        0
#define SPU_REV_MODE_ROOM       1
#define SPU_REV_MODE_STUDIO_A   2
#define SPU_REV_MODE_STUDIO_B   3
#define SPU_REV_MODE_STUDIO_C   4
#define SPU_REV_MODE_HALL       5
#define SPU_REV_MODE_SPACE      6
#define SPU_REV_MODE_ECHO       7
#define SPU_REV_MODE_DELAY      8
#define SPU_REV_MODE_PIPE       9
#define SPU_REV_MODE_MAX        10

void spu_init(void);
void spu_quit(void);

void spu_step(int step_size, short *output);

void spu_set_reverb_enable(int core, int enable);
void spu_set_reverb_mode(int core, int mode);
void spu_set_reverb_depth(int core, short l, short r);

void spu_set_reverb_on(int core, unsigned int mask);
void spu_set_reverb_off(int core, unsigned int mask);

void spu_reverb_clear(void);

void spu_write(unsigned int addr, void *ptr, unsigned int size);

int spu_get_voice_envelope(int core, int num);
unsigned int spu_get_voice_addr(int core, int num);

void spu_set_voice_volume(int core, int voice, unsigned short l, unsigned short r);
void spu_set_voice_pitch(int core, int voice, unsigned short pitch);
void spu_set_voice_address(int core, int voice, unsigned int addr);
void spu_set_voice_repeat(int core, int num, unsigned int addr);

void spu_set_voice_attack(int core, int voice, int mode, unsigned short rate);
void spu_set_voice_decay(int core, int voice, unsigned short rate);
void spu_set_voice_sustain(int core, int voice, int mode, unsigned short rate, unsigned short level);
void spu_set_voice_release(int core, int voice, int mode, unsigned short rate);

void spu_set_key_on(int core, unsigned int keys);
void spu_set_key_off(int core, unsigned int keys);

void spu_set_master_volume(int core, unsigned short l, unsigned short r);

#endif /* _SPU_H_ */
