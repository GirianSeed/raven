#include "attr.h"
#include "spu.h"

#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define SPU_ADPCM_LOOP_END   0x100
#define SPU_ADPCM_LOOP       0x200
#define SPU_ADPCM_LOOP_START 0x400

enum
{
    SPU_STEP_ATTACK,
    SPU_STEP_DECAY,
    SPU_STEP_SUSTAIN,
    SPU_STEP_RELEASE,
    SPU_STEP_OFF,
};

typedef struct spu_voice
{
    short voll, volr;   /* volume left/right */
    unsigned short pitch;
    unsigned int   ssa; /* sound start address */
    int a_mode;         /* attack mode */
    int ar;             /* attack rate */
    int dr;             /* decay rate */
    int s_mode;         /* sustain mode */
    int sr;             /* sustain rate */
    int sl;             /* sustain level */
    int r_mode;         /* release mode */
    int rr;             /* release rate */

    int step;           /* adsr step */

    int env;            /* envelope volume */
    int env_counter;
    int env_rate;
    int env_increasing;
    int env_exponential;
    int env_step;
    int env_increment;

    int pitch_counter;

    unsigned int addr; /* sample address */
    unsigned int lsa;  /* loop start address */

    short block_samples[28];
    unsigned short block_header;
    int has_block;

    short adpcm_hist[2];
    short interp_hist[3];
} spu_voice;

static short mvoll, mvolr; /* master volume left/right */
static short rvoll, rvolr; /* reverb volume left/right */

static unsigned int vmixr; /* voice mix reverb */

static int ren;            /* reverb enable */
static unsigned int rsize; /* reverb work area size */
static reverb_attr rattr;  /* reverb attributes */
static unsigned int raddr; /* reverb index */

static short last_rev_l;
static short last_rev_r;

static int endx; /* bitmask of ended voices */

static spu_voice spu_voices[SPU_NCH];

static unsigned short waveform_data[262144];
static unsigned short reverb_work_area[49184]; /* based on max size */

static size_t output_index;

static int adpcm_filters[][2] =
{
    {0, 0},
    {60, 0},
    {115, -52},
    {98, -55},
    {122, -60},
};

/* windowed sinc with a blackman window */
static short interp_table[] =
{
    -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001,
    -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001, -0x001,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0002, 0x0002, 0x0002, 0x0003, 0x0003,
    0x0003, 0x0004, 0x0004, 0x0005, 0x0005, 0x0006, 0x0007, 0x0007,
    0x0008, 0x0009, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e,
    0x000f, 0x0010, 0x0011, 0x0012, 0x0013, 0x0015, 0x0016, 0x0018,
    0x0019, 0x001b, 0x001c, 0x001e, 0x0020, 0x0021, 0x0023, 0x0025,
    0x0027, 0x0029, 0x002c, 0x002e, 0x0030, 0x0033, 0x0035, 0x0038,
    0x003a, 0x003d, 0x0040, 0x0043, 0x0046, 0x0049, 0x004d, 0x0050,
    0x0054, 0x0057, 0x005b, 0x005f, 0x0063, 0x0067, 0x006b, 0x006f,
    0x0074, 0x0078, 0x007d, 0x0082, 0x0087, 0x008c, 0x0091, 0x0096,
    0x009c, 0x00a1, 0x00a7, 0x00ad, 0x00b3, 0x00ba, 0x00c0, 0x00c7,
    0x00cd, 0x00d4, 0x00db, 0x00e3, 0x00ea, 0x00f2, 0x00fa, 0x0101,
    0x010a, 0x0112, 0x011b, 0x0123, 0x012c, 0x0135, 0x013f, 0x0148,
    0x0152, 0x015c, 0x0166, 0x0171, 0x017b, 0x0186, 0x0191, 0x019c,
    0x01a8, 0x01b4, 0x01c0, 0x01cc, 0x01d9, 0x01e5, 0x01f2, 0x0200,
    0x020d, 0x021b, 0x0229, 0x0237, 0x0246, 0x0255, 0x0264, 0x0273,
    0x0283, 0x0293, 0x02a3, 0x02b4, 0x02c4, 0x02d6, 0x02e7, 0x02f9,
    0x030b, 0x031d, 0x0330, 0x0343, 0x0356, 0x036a, 0x037e, 0x0392,
    0x03a7, 0x03bc, 0x03d1, 0x03e7, 0x03fc, 0x0413, 0x042a, 0x0441,
    0x0458, 0x0470, 0x0488, 0x04a0, 0x04b9, 0x04d2, 0x04ec, 0x0506,
    0x0520, 0x053b, 0x0556, 0x0572, 0x058e, 0x05aa, 0x05c7, 0x05e4,
    0x0601, 0x061f, 0x063e, 0x065c, 0x067c, 0x069b, 0x06bb, 0x06dc,
    0x06fd, 0x071e, 0x0740, 0x0762, 0x0784, 0x07a7, 0x07cb, 0x07ef,
    0x0813, 0x0838, 0x085d, 0x0883, 0x08a9, 0x08d0, 0x08f7, 0x091e,
    0x0946, 0x096f, 0x0998, 0x09c1, 0x09eb, 0x0a16, 0x0a40, 0x0a6c,
    0x0a98, 0x0ac4, 0x0af1, 0x0b1e, 0x0b4c, 0x0b7a, 0x0ba9, 0x0bd8,
    0x0c07, 0x0c38, 0x0c68, 0x0c99, 0x0ccb, 0x0cfd, 0x0d30, 0x0d63,
    0x0d97, 0x0dcb, 0x0e00, 0x0e35, 0x0e6b, 0x0ea1, 0x0ed7, 0x0f0f,
    0x0f46, 0x0f7f, 0x0fb7, 0x0ff1, 0x102a, 0x1065, 0x109f, 0x10db,
    0x1116, 0x1153, 0x118f, 0x11cd, 0x120b, 0x1249, 0x1288, 0x12c7,
    0x1307, 0x1347, 0x1388, 0x13c9, 0x140b, 0x144d, 0x1490, 0x14d4,
    0x1517, 0x155c, 0x15a0, 0x15e6, 0x162c, 0x1672, 0x16b9, 0x1700,
    0x1747, 0x1790, 0x17d8, 0x1821, 0x186b, 0x18b5, 0x1900, 0x194b,
    0x1996, 0x19e2, 0x1a2e, 0x1a7b, 0x1ac8, 0x1b16, 0x1b64, 0x1bb3,
    0x1c02, 0x1c51, 0x1ca1, 0x1cf1, 0x1d42, 0x1d93, 0x1de5, 0x1e37,
    0x1e89, 0x1edc, 0x1f2f, 0x1f82, 0x1fd6, 0x202a, 0x207f, 0x20d4,
    0x2129, 0x217f, 0x21d5, 0x222c, 0x2282, 0x22da, 0x2331, 0x2389,
    0x23e1, 0x2439, 0x2492, 0x24eb, 0x2545, 0x259e, 0x25f8, 0x2653,
    0x26ad, 0x2708, 0x2763, 0x27be, 0x281a, 0x2876, 0x28d2, 0x292e,
    0x298b, 0x29e7, 0x2a44, 0x2aa1, 0x2aff, 0x2b5c, 0x2bba, 0x2c18,
    0x2c76, 0x2cd4, 0x2d33, 0x2d91, 0x2df0, 0x2e4f, 0x2eae, 0x2f0d,
    0x2f6c, 0x2fcc, 0x302b, 0x308b, 0x30ea, 0x314a, 0x31aa, 0x3209,
    0x3269, 0x32c9, 0x3329, 0x3389, 0x33e9, 0x3449, 0x34a9, 0x3509,
    0x3569, 0x35c9, 0x3629, 0x3689, 0x36e8, 0x3748, 0x37a8, 0x3807,
    0x3867, 0x38c6, 0x3926, 0x3985, 0x39e4, 0x3a43, 0x3aa2, 0x3b00,
    0x3b5f, 0x3bbd, 0x3c1b, 0x3c79, 0x3cd7, 0x3d35, 0x3d92, 0x3def,
    0x3e4c, 0x3ea9, 0x3f05, 0x3f62, 0x3fbd, 0x4019, 0x4074, 0x40d0,
    0x412a, 0x4185, 0x41df, 0x4239, 0x4292, 0x42eb, 0x4344, 0x439c,
    0x43f4, 0x444c, 0x44a3, 0x44fa, 0x4550, 0x45a6, 0x45fc, 0x4651,
    0x46a6, 0x46fa, 0x474e, 0x47a1, 0x47f4, 0x4846, 0x4898, 0x48e9,
    0x493a, 0x498a, 0x49d9, 0x4a29, 0x4a77, 0x4ac5, 0x4b13, 0x4b5f,
    0x4bac, 0x4bf7, 0x4c42, 0x4c8d, 0x4cd7, 0x4d20, 0x4d68, 0x4db0,
    0x4df7, 0x4e3e, 0x4e84, 0x4ec9, 0x4f0e, 0x4f52, 0x4f95, 0x4fd7,
    0x5019, 0x505a, 0x509a, 0x50da, 0x5118, 0x5156, 0x5194, 0x51d0,
    0x520c, 0x5247, 0x5281, 0x52ba, 0x52f3, 0x532a, 0x5361, 0x5397,
    0x53cc, 0x5401, 0x5434, 0x5467, 0x5499, 0x54ca, 0x54fa, 0x5529,
    0x5558, 0x5585, 0x55b2, 0x55de, 0x5609, 0x5632, 0x565b, 0x5684,
    0x56ab, 0x56d1, 0x56f6, 0x571b, 0x573e, 0x5761, 0x5782, 0x57a3,
    0x57c3, 0x57e2, 0x57ff, 0x581c, 0x5838, 0x5853, 0x586d, 0x5886,
    0x589e, 0x58b5, 0x58cb, 0x58e0, 0x58f4, 0x5907, 0x5919, 0x592a,
    0x593a, 0x5949, 0x5958, 0x5965, 0x5971, 0x597c, 0x5986, 0x598f,
    0x5997, 0x599e, 0x59a4, 0x59a9, 0x59ad, 0x59b0, 0x59b2, 0x59b3,
};

static void spu_set_adsr(spu_voice *voice)
{
    int rate;
    int clamp;

    int increasing;
    int exponential;
    int step;
    int increment;

    switch (voice->step)
    {
    case SPU_STEP_ATTACK:
        rate = voice->ar;
        clamp = voice->ar != 0x7f;
        increasing = 1;
        exponential = voice->a_mode == SPU_ADSR_EXP_INC;
        break;

    case SPU_STEP_DECAY:
        rate = voice->dr << 2;
        clamp = 1; /* decay always clamps */
        increasing = 0;
        exponential = 1;
        break;

    case SPU_STEP_SUSTAIN:
        rate = voice->sr;
        clamp = voice->sr != 0x7f;
        increasing = voice->s_mode == SPU_ADSR_LIN_INC || voice->s_mode == SPU_ADSR_EXP_INC;
        exponential = voice->s_mode == SPU_ADSR_EXP_INC || voice->s_mode == SPU_ADSR_EXP_DEC;
        break;

    case SPU_STEP_RELEASE:
    case SPU_STEP_OFF:
        rate = voice->rr << 2;
        clamp = voice->rr != 0x1f;
        increasing = 0;
        exponential = voice->r_mode == SPU_ADSR_EXP_DEC;
        break;

    default:
        assert(0); /* unreachable */
        break;
    }

    step = 7 - (rate & 0x3);
    step = increasing ? step : ~step;

    increment = 0x8000;

    if (rate < 44)
    {
        step <<= 11 - (rate >> 2);
    }
    else if (rate >= 48)
    {
        increment >>= (rate >> 2) - 11;

        if (clamp)
        {
            increment = MAX(increment, 1);
        }
    }

    voice->env_counter = 0;
    voice->env_rate = rate;
    voice->env_increasing = increasing;
    voice->env_exponential = exponential;
    voice->env_step = step;
    voice->env_increment = increment;
}

static void spu_tick_adsr(spu_voice *voice)
{
    int step = voice->env_step;
    int increment = voice->env_increment;

    if (voice->env_exponential)
    {
        if (voice->env_increasing)
        {
            if (voice->env >= 0x6000)
            {
                if (voice->env_rate < 40)
                {
                    step >>= 2;
                }
                else if (voice->env_rate >= 44)
                {
                    increment >>= 2;
                }
                else
                {
                    step >>= 1;
                    increment >>= 1;
                }
            }
        }
        else
        {
            step = (step * voice->env) >> 15;
        }
    }

    voice->env_counter += increment;
    if (!(voice->env_counter & 0x8000))
    {
        return;
    }

    voice->env_counter = 0;
    voice->env += step;

    if (voice->env_increasing)
    {
        voice->env = CLAMP(voice->env, -0x8000, 0x7fff);
    }
    else
    {
        voice->env = CLAMP(voice->env, 0, 0x7fff);
    }

    if (voice->step == SPU_STEP_ATTACK && voice->env == 0x7fff)
    {
        voice->step = SPU_STEP_DECAY;
        spu_set_adsr(voice);
    }
    else if (voice->step == SPU_STEP_DECAY && voice->env <= ((voice->sl + 1) * 0x800))
    {
        voice->step = SPU_STEP_SUSTAIN;
        spu_set_adsr(voice);
    }
    else if (voice->step == SPU_STEP_RELEASE && voice->env == 0)
    {
        voice->step = SPU_STEP_OFF;
        endx |= 1 << (voice - spu_voices);
        spu_set_adsr(voice);
    }
}

static short spu_saturate(int value)
{
    return CLAMP(value, -0x8000, 0x7fff);
}

static void spu_load_block(spu_voice *voice)
{
    int addr;
    unsigned short header;
    int shift;
    int filter;
    int filt0, filt1;
    unsigned short word;
    int sample;
    int diff;

    addr = voice->addr * 4;
    header = waveform_data[addr++];
    shift = header & 0xf;
    filter = (header >> 4) & 0xf;

    assert(shift <= 12);
    assert(filter <= 4);

    voice->block_header = header;

    if (header & SPU_ADPCM_LOOP_START)
    {
        voice->lsa = voice->addr;
    }

    filt0 = adpcm_filters[filter][0];
    filt1 = adpcm_filters[filter][1];

    for (int w = 0; w < 7; w++)
    {
        word = waveform_data[addr++];

        for (int i = 0; i < 4; i++)
        {
            sample = (short)(word << 12);
            sample >>= shift;

            word >>= 4;

            diff = 0;
            diff += voice->adpcm_hist[0] * filt0;
            diff += voice->adpcm_hist[1] * filt1;

            sample = spu_saturate(sample + (diff + 32) / 64);

            voice->adpcm_hist[1] = voice->adpcm_hist[0];
            voice->adpcm_hist[0] = sample;

            voice->block_samples[w * 4 + i] = sample;
        }
    }
}

static short spu_sample(spu_voice *voice)
{
    int sample_index;
    int interp_index;
    int sample;
    int output;

    if (!voice->has_block)
    {
        spu_load_block(voice);
        voice->has_block = 1;
    }

    sample_index = voice->pitch_counter >> 12;
    interp_index = (voice->pitch_counter >> 4) & 0xff;

    sample = voice->block_samples[sample_index];

    output = 0;
    output += (interp_table[255 - interp_index] * voice->interp_hist[2]) >> 15;
    output += (interp_table[511 - interp_index] * voice->interp_hist[1]) >> 15;
    output += (interp_table[256 + interp_index] * voice->interp_hist[0]) >> 15;
    output += (interp_table[interp_index] * sample) >> 15;

    voice->interp_hist[2] = voice->interp_hist[1];
    voice->interp_hist[1] = voice->interp_hist[0];
    voice->interp_hist[0] = sample;

    return output;
}

static void spu_update_pitch(spu_voice *voice)
{
    voice->pitch_counter += MIN(voice->pitch, 0x4000);
    if (voice->pitch_counter >= 0x1c000)
    {
        voice->pitch_counter -= 0x1c000;

        voice->addr += 2;
        voice->has_block = 0;

        if (voice->block_header & SPU_ADPCM_LOOP_END)
        {
            voice->addr = voice->lsa;

            if (!(voice->block_header & SPU_ADPCM_LOOP))
            {
                voice->step = SPU_STEP_OFF;
                voice->env = 0;
                spu_set_adsr(voice);
            }
        }
    }
}

static int apply_volume(short sample, short volume)
{
    return (sample * volume) >> 15;
}

static void spu_tick_voice(spu_voice *voice, int *l, int *r)
{
    int sample;

    spu_tick_adsr(voice);

    sample = spu_sample(voice);
    sample = apply_volume(sample, voice->env);

    *l = apply_volume(sample, voice->voll << 1);
    *r = apply_volume(sample, voice->volr << 1);

   spu_update_pitch(voice);
}

static float i16_to_f32(short value)
{
    return (float)value / ((value < 0) ? 32768 : 32767);
}

static short f32_to_i16(float value)
{
    return CLAMP(value, -1.0f, 1.0f) * ((value < 0.0f) ? 32768.0f : 32767.0f);
}

static float spu_reverb_read(unsigned int addr)
{
    short data = reverb_work_area[(raddr + addr) % rsize];
    return i16_to_f32(data);
}

static void spu_reverb_write(unsigned int addr, float value)
{
    reverb_work_area[(raddr + addr) % rsize] = f32_to_i16(value);
}

static void spu_process_reverb(int l, int r)
{
    float in;
    float same;
    float diff;
    float out;
    float temp;

    if (!(output_index & 1))
    {
        /* apply reverb volume to input */
        in = i16_to_f32(l) * i16_to_f32(rattr.lin);

        /* apply same-side reflection */
        temp = spu_reverb_read(rattr.lsamed * 4);
        same = in + temp * i16_to_f32(rattr.wall);

        temp = spu_reverb_read(rattr.lsamem * 4 - 1);
        same = temp + (same - temp) * i16_to_f32(rattr.iir);

        spu_reverb_write(rattr.lsamem * 4, same);

        /* apply opposite-side reflection */
        temp = spu_reverb_read(rattr.rdiffd * 4);
        diff = in + temp * i16_to_f32(rattr.wall);

        temp = spu_reverb_read(rattr.ldiffm * 4 - 1);
        diff = temp + (diff - temp) * i16_to_f32(rattr.iir);

        spu_reverb_write(rattr.ldiffm * 4, diff);

        /* apply early echo */
        temp = spu_reverb_read(rattr.lcomb1 * 4);
        out = temp * i16_to_f32(rattr.comb1);

        temp = spu_reverb_read(rattr.lcomb2 * 4);
        out += temp * i16_to_f32(rattr.comb2);

        temp = spu_reverb_read(rattr.lcomb3 * 4);
        out += temp * i16_to_f32(rattr.comb3);

        temp = spu_reverb_read(rattr.lcomb4 * 4);
        out += temp * i16_to_f32(rattr.comb4);

        /* apply first reverb apf */
        temp = spu_reverb_read((rattr.lapf1 - rattr.apfd1) * 4);
        out -= temp * i16_to_f32(rattr.apfv1);

        spu_reverb_write(rattr.lapf1 * 4, out);

        out = out * i16_to_f32(rattr.apfv1) + temp;

        /* apply second reverb apf */
        temp = spu_reverb_read((rattr.lapf2 - rattr.apfd2) * 4);
        out -= temp * i16_to_f32(rattr.apfv2);

        spu_reverb_write(rattr.lapf2 * 4, out);

        out = out * i16_to_f32(rattr.apfv2) + temp;

        /* apply output volume */
        out = out * i16_to_f32(rvoll);
        last_rev_l = f32_to_i16(out);
    }
    else
    {
       /* apply reverb volume to input */
        in = i16_to_f32(r) * i16_to_f32(rattr.rin);

        /* apply same-side reflection */
        temp = spu_reverb_read(rattr.rsamed * 4);
        same = in + temp * i16_to_f32(rattr.wall);

        temp = spu_reverb_read(rattr.rsamem * 4 - 1);
        same = temp + (same - temp) * i16_to_f32(rattr.iir);

        spu_reverb_write(rattr.rsamem * 4, same);

        /* apply opposite-side reflection */
        temp = spu_reverb_read(rattr.ldiffd * 4);
        diff = in + temp * i16_to_f32(rattr.wall);

        temp = spu_reverb_read(rattr.rdiffm * 4 - 1);
        diff = temp + (diff - temp) * i16_to_f32(rattr.iir);

        spu_reverb_write(rattr.rdiffm * 4, diff);

        /* apply early echo */
        temp = spu_reverb_read(rattr.rcomb1 * 4);
        out = temp * i16_to_f32(rattr.comb1);

        temp = spu_reverb_read(rattr.rcomb2 * 4);
        out += temp * i16_to_f32(rattr.comb2);

        temp = spu_reverb_read(rattr.rcomb3 * 4);
        out += temp * i16_to_f32(rattr.comb3);

        temp = spu_reverb_read(rattr.rcomb4 * 4);
        out += temp * i16_to_f32(rattr.comb4);

        /* apply first reverb apf */
        temp = spu_reverb_read((rattr.rapf1 - rattr.apfd1) * 4);
        out -= temp * i16_to_f32(rattr.apfv1);

        spu_reverb_write(rattr.rapf1 * 4, out);

        out = out * i16_to_f32(rattr.apfv1) + temp;

        /* apply second reverb apf */
        temp = spu_reverb_read((rattr.rapf2 - rattr.apfd2) * 4);
        out -= temp * i16_to_f32(rattr.apfv2);

        spu_reverb_write(rattr.rapf2 * 4, out);

        out = out * i16_to_f32(rattr.apfv2) + temp;

        /* apply output volume */
        out = out * i16_to_f32(rvolr);
        last_rev_r = f32_to_i16(out);
    }

    /* increment the reverb index */
    raddr = (raddr + 1) % rsize;
}

static void spu_tick(short *output)
{
    int dryl = 0;
    int dryr = 0;
    int wetl = 0;
    int wetr = 0;
    int outl;
    int outr;

    for (int i = 0; i < SPU_NCH; i++)
    {
        spu_voice *voice = &spu_voices[i];
        int vl, vr;

        if (voice->step == SPU_STEP_OFF)
        {
            continue;
        }

        spu_tick_voice(voice, &vl, &vr);

        dryl += vl;
        dryr += vr;

        if (ren && (vmixr & (1 << i)))
        {
            wetl += vl;
            wetr += vr;
        }
    }

    if (ren)
    {
        spu_process_reverb(wetl, wetr);
    }

    outl = spu_saturate(dryl + last_rev_l);
    outr = spu_saturate(dryr + last_rev_r);

    output[output_index++] = apply_volume(outl, mvoll << 1);
    output[output_index++] = apply_volume(outr, mvolr << 1);
}

void spu_init(void)
{
    mvoll = 0;
    mvolr = 0;
    rvoll = 0;
    rvolr = 0;

    memset(waveform_data, 0xff, sizeof(waveform_data));
    memset(reverb_work_area, 0, sizeof(reverb_work_area));

    spu_set_key_on(0xffffff);
    spu_set_key_off(0xffffff);
    endx = 0xffffff;

    vmixr = 0;

    for (int i = 0; i < SPU_NCH; i++)
    {
        spu_voices[i].voll = 0;
        spu_voices[i].volr = 0;
        spu_voices[i].pitch = 0x3fff;
        spu_voices[i].ssa = 0x200;
        spu_voices[i].a_mode = SPU_ADSR_LIN_INC;
        spu_voices[i].ar = 0;
        spu_voices[i].dr = 0;
        spu_voices[i].s_mode = SPU_ADSR_LIN_INC;
        spu_voices[i].sr = 0;
        spu_voices[i].sl = 0;
        spu_voices[i].r_mode = SPU_ADSR_LIN_DEC;
        spu_voices[i].rr = 0;
        spu_voices[i].step = SPU_STEP_OFF;
        spu_voices[i].has_block = 0;

        spu_set_adsr(&spu_voices[i]);
    }

    ren = 0;
    rsize = 64;
    raddr = 0;
}

void spu_quit(void)
{
    /* do nothing */
}

void spu_step(int step_size, short *output)
{
    output_index = 0;

    for (int i = 0; i < step_size; i++)
    {
        spu_tick(output);
    }
}

int spu_endx(void)
{
    return endx;
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

void spu_write(unsigned int addr, char *ptr, unsigned int size)
{
    addr = (addr + 0x7) & ~0x7;
    size = (size + 0x3f) & ~0x3f;
    memcpy((char *)waveform_data + addr, ptr, size);
}

void spu_set_voice_volume(int num, unsigned short l, unsigned short r)
{
    if (l & 0x8000 || r & 0x8000)
    {
        printf("warning: unsupported volume envelope on voice %d\n", num);
    }

    spu_voices[num].voll = l & 0x7fff;
    spu_voices[num].volr = r & 0x7fff;
}

void spu_set_voice_pitch(int num, unsigned short pitch)
{
    spu_voices[num].pitch = pitch;
}

void spu_set_voice_address(int num, unsigned int addr)
{
    assert((addr % 8) == 0);
    assert(addr < sizeof(waveform_data));
    spu_voices[num].ssa = (addr + 0x7) >> 3;
}

void spu_set_voice_attack(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_EXP_INC);

    spu_voices[num].a_mode = mode;
    spu_voices[num].ar = (rate > 0x7f) ? 0x7f : rate;

    spu_set_adsr(&spu_voices[num]);
}

void spu_set_voice_decay(int num, unsigned short rate)
{
    spu_voices[num].dr = (rate > 0xf) ? 0xf : rate;

    spu_set_adsr(&spu_voices[num]);
}

void spu_set_voice_sustain(int num, int mode, unsigned short rate, unsigned short level)
{
    assert(mode == SPU_ADSR_LIN_INC || mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_INC || mode == SPU_ADSR_EXP_DEC);

    spu_voices[num].s_mode = mode;
    spu_voices[num].sr = (rate > 0x7f) ? 0x7f : rate;
    spu_voices[num].sl = (level > 0xf) ? 0xf : level;

    spu_set_adsr(&spu_voices[num]);
}

void spu_set_voice_release(int num, int mode, unsigned short rate)
{
    assert(mode == SPU_ADSR_LIN_DEC || mode == SPU_ADSR_EXP_DEC);

    spu_voices[num].r_mode = mode;
    spu_voices[num].rr = (rate > 0x1f) ? 0x1f : rate;

    spu_set_adsr(&spu_voices[num]);
}

void spu_set_key_on(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1 << i))
        {
            endx &= ~(1 << i);

            spu_voices[i].step = SPU_STEP_ATTACK;
            spu_voices[i].addr = spu_voices[i].ssa;
            spu_voices[i].env = 0;
            spu_voices[i].env_counter = 0;
            spu_voices[i].pitch_counter = 0;
            spu_voices[i].has_block = 0;

            spu_set_adsr(&spu_voices[i]);

            spu_voices[i].adpcm_hist[0] = 0;
            spu_voices[i].adpcm_hist[1] = 0;

            spu_voices[i].interp_hist[0] = 0;
            spu_voices[i].interp_hist[1] = 0;
            spu_voices[i].interp_hist[2] = 0;
        }
    }
}

void spu_set_key_off(unsigned int keys)
{
    for (int i = 0; i < SPU_NCH; i++)
    {
        if (keys & (1 << i))
        {
            spu_voices[i].step = SPU_STEP_RELEASE;
            spu_set_adsr(&spu_voices[i]);
        }
    }
}

void spu_set_master_volume(unsigned short l, unsigned short r)
{
    if (l & 0x8000 || r & 0x8000)
    {
        printf("warning: unsupported master volume envelope\n");
    }

    mvoll = l & 0x7fff;
    mvolr = r & 0x7fff;
}
