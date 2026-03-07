#include "sd/sd_ext.h"
#include "sd/sd_incl.h"
#include "spu/spu.h"

#define MEM_STR_BASE    0x9020
#define MEM_STR_BUFSZ   0x800
#define MEM_STR_SIZE    (MEM_STR_BUFSZ * 2)

void init_mem_str_w(void)
{
    for (int i = 0; i < SD_SE_VOICES; i++)
    {
        mem_str_w[i].state = 0;
    }
}

static int mem_str_buffer(int stream, int which)
{
    return MEM_STR_BASE + stream * MEM_STR_SIZE + which * MEM_STR_BUFSZ;
}

int MemSpuTransWithNoLoop(int track)
{
    int stream;
    int voice;
    MEM_STR_W *str_w;
    int addr;

    stream = track - 32;
    voice = track - 24;

    str_w = &mem_str_w[stream];
    switch (str_w->state & 0xF)
    {
    case 2:
        if (voice_tbl[str_w->note].addr == 0xFFFFFFFF)
        {
            SD_WARN("ERROR:invalid streaming sample (%d)\n", str_w->note);
            str_w->state = 0;
            break;
        }

        str_w->total = str_w->remaining = voice_tbl[str_w->note + 1].addr - voice_tbl[str_w->note].addr;
        str_w->addr = &mem_str_buf[voice_tbl[str_w->note].addr];
        str_w->addr[17] = 0;
        str_w->addr[str_w->total - 31] = 0x1;

        spu_set_voice_address(1, voice, mem_str_buffer(stream, 0));
        spu_set_voice_repeat(1, voice, mem_str_buffer(stream, 0));
        spu_write(mem_str_buffer(stream, 0), str_w->addr, MEM_STR_BUFSZ);
        str_w->addr += MEM_STR_BUFSZ;
        str_w->remaining -= MEM_STR_BUFSZ;

        str_w->state++;
        return 1;
    case 3:
        if (str_w->remaining > 0)
        {
            if (str_w->remaining > MEM_STR_BUFSZ && str_w->addr[MEM_STR_BUFSZ - 15] != 1)
            {
                str_w->addr[MEM_STR_BUFSZ - 15] |= 0x3;
            }

            spu_write(mem_str_buffer(stream, 1), str_w->addr, MEM_STR_BUFSZ);
            str_w->addr += MEM_STR_BUFSZ;
            str_w->remaining -= MEM_STR_BUFSZ;

            str_w->state++;
            return 1;
        }

        str_w->state++;
        break;
    case 4:
        spu_set_voice_address(1, voice, mem_str_buffer(stream, 0));
        keyon();
        str_w->buffer = MEM_STR_BUFSZ;

        str_w->state++;
        if (str_w->remaining <= 0)
        {
            str_w->state++;
        }
        break;
    case 5:
        if (spu_get_voice_envelope(1, voice) == 0)
        {
            str_w->state++;
        }

        addr = spu_get_voice_addr(1, voice) - mem_str_buffer(stream, 0);
        if (addr >= MEM_STR_SIZE)
        {
            SD_WARN("ERROR:MemoryStreamingAddress(%x)\n", addr);
            break;
        }

        if (str_w->buffer == (addr & MEM_STR_BUFSZ))
        {
            if (addr < MEM_STR_BUFSZ)
            {
                if (str_w->remaining > MEM_STR_BUFSZ && str_w->addr[MEM_STR_BUFSZ - 15] != 1)
                {
                    str_w->addr[MEM_STR_BUFSZ - 15] |= 0x3;
                }

                spu_write(mem_str_buffer(stream, 1), str_w->addr, MEM_STR_BUFSZ);
                str_w->buffer = MEM_STR_BUFSZ;
            }
            else
            {
                spu_write(mem_str_buffer(stream, 0), str_w->addr, MEM_STR_BUFSZ);
                str_w->buffer = 0;
            }

            str_w->addr += MEM_STR_BUFSZ;
            if (str_w->remaining <= MEM_STR_BUFSZ)
            {
                str_w->state++;
            }
            else
            {
                str_w->remaining -= MEM_STR_BUFSZ;
            }

            return 1;
        }
        break;
    case 6:
        str_w->state++;
        break;
    }

    return 0;
}
