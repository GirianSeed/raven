#ifndef _LIBSPU_H_
#define _LIBSPU_H_

#define SPU_OFF               0
#define SPU_ON                1

#define SPU_NCH               24

#define SPU_00CH              (1 << 0)
#define SPU_01CH              (1 << 1)
#define SPU_02CH              (1 << 2)
#define SPU_03CH              (1 << 3)
#define SPU_04CH              (1 << 4)
#define SPU_05CH              (1 << 5)
#define SPU_06CH              (1 << 6)
#define SPU_07CH              (1 << 7)
#define SPU_08CH              (1 << 8)
#define SPU_09CH              (1 << 9)
#define SPU_10CH              (1 << 10)
#define SPU_11CH              (1 << 11)
#define SPU_12CH              (1 << 12)
#define SPU_13CH              (1 << 13)
#define SPU_14CH              (1 << 14)
#define SPU_15CH              (1 << 15)
#define SPU_16CH              (1 << 16)
#define SPU_17CH              (1 << 17)
#define SPU_18CH              (1 << 18)
#define SPU_19CH              (1 << 19)
#define SPU_20CH              (1 << 20)
#define SPU_21CH              (1 << 21)
#define SPU_22CH              (1 << 22)
#define SPU_23CH              (1 << 23)
#define SPU_ALLCH             ((1 << SPU_NCH) - 1)

#define	SPU_VOICE_VOLL        (1 << 0)
#define	SPU_VOICE_VOLR        (1 << 1)
#define	SPU_VOICE_PITCH       (1 << 4)
#define	SPU_VOICE_WDSA        (1 << 7)
#define	SPU_VOICE_ADSR_AMODE  (1 << 8)
#define	SPU_VOICE_ADSR_SMODE  (1 << 9)
#define	SPU_VOICE_ADSR_RMODE  (1 << 10)
#define	SPU_VOICE_ADSR_AR     (1 << 11)
#define	SPU_VOICE_ADSR_DR     (1 << 12)
#define	SPU_VOICE_ADSR_SR     (1 << 13)
#define	SPU_VOICE_ADSR_RR     (1 << 14)
#define	SPU_VOICE_ADSR_SL     (1 << 15)

#define	SPU_COMMON_MVOLL      (1 << 0)
#define	SPU_COMMON_MVOLR      (1 << 1)

#define SPU_REV_MODE          (1 << 0)
#define SPU_REV_DEPTHL        (1 << 1)
#define SPU_REV_DEPTHR        (1 << 2)

#define SPU_REV_MODE_STUDIO_C 4

typedef struct {
    short left;
    short right;
} SpuVolume;

typedef struct {
    unsigned int   voice;

    unsigned int   mask;
    SpuVolume      volume;
    SpuVolume      volmode;
    SpuVolume      volumex;
    unsigned short pitch;
    unsigned short note;
    unsigned short sample_note;
    short          envx;
    unsigned int   addr;
    unsigned int   loop_addr;
    int            a_mode;
    int            s_mode;
    int            r_mode;
    unsigned short ar;
    unsigned short dr;
    unsigned short sr;
    unsigned short rr;
    unsigned short sl;
    unsigned short adsr1;
    unsigned short adsr2;
} SpuVoiceAttr;

typedef struct {
    SpuVolume volume;
    int       reverb;
    int       mix;
} SpuExtAttr;

typedef struct {
    unsigned int mask;

    SpuVolume   mvol;
    SpuVolume   mvolmode;
    SpuVolume   mvolx;
    SpuExtAttr  cd;
    SpuExtAttr  ext;
} SpuCommonAttr;

typedef struct {
    unsigned int mask;

    int       mode;
    SpuVolume depth;
    int       delay;
    int       feedback;
} SpuReverbAttr;

extern void SpuInit(void);
extern void SpuQuit(void);

extern unsigned int SpuSetNoiseVoice(int on_off, unsigned int voice_bit);

extern int SpuSetReverb(int on_off);
extern int SpuSetReverbModeParam(SpuReverbAttr *attr);
extern int SpuSetReverbDepth(SpuReverbAttr *attr);
extern int SpuReserveReverbWorkArea(int on_off);
extern unsigned int SpuSetReverbVoice(int on_off, unsigned int voice_bit);
extern int SpuClearReverbWorkArea(int mode);

extern void SpuSetVoiceAttr(SpuVoiceAttr *attr);
extern void SpuSetKey(int on_off, unsigned int voice_bit);

extern unsigned int SpuSetPitchLFOVoice(int on_off, unsigned int voice_bit);

extern void SpuSetCommonAttr(SpuCommonAttr *attr);

extern int SpuMalloc(int size);

#endif /* _LIBSPU_H_ */