#include "attr.h"
#include "spu.h"

#include <assert.h>
#include <string.h>

enum {
    SPU_STEP_ATTACK,
    SPU_STEP_DECAY,
    SPU_STEP_SUSTAIN,
    SPU_STEP_RELEASE,
};

typedef struct {
    short voll, volr;   /* volume left/right */
    unsigned short pitch;
    unsigned short ssa; /* sound start address */
    int a_mode;         /* attack mode */
    int ar;             /* attack rate */
    int dr;             /* decay rate */
    int s_mode;         /* sustain mode */
    int sr;             /* sustain rate */
    int sl;             /* sustain level */
    int r_mode;         /* release mode */
    int rr;             /* release rate */

    int step;            /* adsr step */
    short env;           /* envelope volume */
    unsigned short addr; /* sample address */
    unsigned short lsa;  /* loop start address */
} spu_voice;

static short mvoll, mvolr; /* master volume left/right */
static short rvoll, rvolr; /* reverb volume left/right */

static unsigned int vmixr; /* voice mix reverb */

static int ren;            /* reverb enable */
static unsigned int rsize; /* reverb work area size */
static reverb_attr rattr;  /* reverb attributes */

static spu_voice voice[SPU_NCH];
static unsigned short reverb_work_area[49184]; /* based on max size */

void spu_init(void)
{
    mvoll = 0;
    mvolr = 0;
    rvoll = 0;
    rvolr = 0;

    spu_set_key_on(0xffffff);
    spu_set_key_off(0xffffff);

    vmixr = 0;

    for (int i = 0; i < SPU_NCH; i++)
    {
        voice[i].voll = 0;
        voice[i].volr = 0;
        voice[i].pitch = 0x3fff;
        voice[i].ssa = 0x200;
        voice[i].a_mode = SPU_ADSR_LIN_INC;
        voice[i].ar = 0;
        voice[i].dr = 0;
        voice[i].s_mode = SPU_ADSR_LIN_INC;
        voice[i].sr = 0;
        voice[i].sl = 0;
        voice[i].r_mode = SPU_ADSR_LIN_DEC;
        voice[i].rr = 0;
    }

    ren = 0;
    rsize = 16;
}

void spu_quit(void)
{
    /* do nothing */
}

void spu_set_reverb_enable(int enable)
{
    ren = enable;
}

void spu_set_reverb_mode(int mode)
{
    assert(mode == SPU_REV_MODE_STUDIO_C);
    assert(ren == 0);

    rvoll = 0;
    rvolr = 0;

    rattr = reverb_mode_attr[mode];
    rsize = reverb_work_area_size[mode];
}

void spu_set_reverb_depth(short l, short r)
{
    rvoll = l;
    rvolr = r;
}

void spu_set_reverb_on(unsigned int mask)
{
    vmixr |= mask & 0xffffff;
}

void spu_set_reverb_off(unsigned int mask)
{
    vmixr &= ~(mask & 0xffffff);
}

void spu_reverb_clear(void)
{
    memset(reverb_work_area, 0, sizeof(reverb_work_area));
}

void spu_set_voice_volume(int num, unsigned short l, unsigned short r)
{
    voice[num].voll = l & 0x7fff;
    voice[num].volr = r & 0x7fff;
}

void spu_set_voice_pitch(int num, unsigned short pitch)
{
    voice[num].pitch = pitch;
}

void spu_set_voice_address(int num, unsigned int addr)
{
    voice[num].ssa = (addr + 7) / 8;
}

void spu_set_voice_attack(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_EXP_INC);

    voice[num].a_mode = mode;
    voice[num].ar = (rate > 0x7f) ? 0x7f : rate;
}

void spu_set_voice_decay(int num, unsigned short rate)
{
    voice[num].dr = (rate > 0xf) ? 0xf : rate;
}

void spu_set_voice_sustain(int num, int mode, unsigned short rate, unsigned short level)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_INC || mode == SPU_ADSR_EXP_DEC);

    voice[num].s_mode = mode;
    voice[num].sr = (rate > 0x7f) ? 0x7f : rate;
    voice[num].sl = (level > 0xf) ? 0xf : level;
}

void spu_set_voice_release(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_DEC);

    voice[num].r_mode = mode;
    voice[num].rr = (rate > 0x1f) ? 0x1f : rate;
}

void spu_set_key_on(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1 << i))
        {
            voice[i].step = SPU_STEP_ATTACK;
            voice[i].addr = voice[i].ssa;
            voice[i].env = 0;
        }
    }
}

void spu_set_key_off(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1 << i))
        {
            voice[i].step = SPU_STEP_RELEASE;
        }
    }
}

void spu_set_master_volume(unsigned short l, unsigned short r)
{
    mvoll = l & 0x7fff;
    mvolr = r & 0x7fff;
}
