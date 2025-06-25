#include "wave.h"

#include <limits.h>
#include <stdio.h>

#define WAVE_PCM 1

typedef struct wave_riff_h
{
    unsigned int   id;
    unsigned int   size;
    unsigned int   format;
} wave_riff_h;

typedef struct wave_fmt_chunk
{
    unsigned int   id;
    unsigned int   size;
    unsigned short format;
    unsigned short channels;
    unsigned int   sample_rate;
    unsigned int   byte_rate;
    unsigned short block_align;
    unsigned short sample_size;
} wave_fmt_chunk;

typedef struct wave_data_chunk
{
    unsigned int   id;
    unsigned int   size;
} wave_data_chunk;

int write_wave_file(const char *name, const char *data, size_t size)
{
    wave_riff_h riff_header;
    wave_fmt_chunk fmt_chunk;
    wave_data_chunk data_chunk;
    FILE *fp;
    size_t rb;

    if (size == 0)
    {
        printf("data size is zero, skipping write\n");
        return 0;
    }

    riff_header.id = 0x46464952; // "RIFF"
    riff_header.size = size + 36;
    riff_header.format = 0x45564157; // "WAVE"

    fmt_chunk.id = 0x20746d66; // "fmt "
    fmt_chunk.size = 16;
    fmt_chunk.format = WAVE_PCM;
    fmt_chunk.channels = 2;
    fmt_chunk.sample_rate = 44100;
    fmt_chunk.byte_rate = fmt_chunk.sample_rate * fmt_chunk.channels * sizeof(short);
    fmt_chunk.block_align = fmt_chunk.channels * sizeof(short);
    fmt_chunk.sample_size = sizeof(short) * CHAR_BIT;

    data_chunk.id = 0x61746164; // "data"
    data_chunk.size = size;

    fp = fopen(name, "wb");
    if (fp == NULL)
    {
        printf("error: failed to open wave file %s for writing\n", name);
        return 1;
    }

    rb = fwrite(&riff_header, sizeof(riff_header), 1, fp);
    if (rb != 1)
    {
        printf("error: failed to write RIFF header to %s\n", name);
        goto error;
    }

    rb = fwrite(&fmt_chunk, sizeof(fmt_chunk), 1, fp);
    if (rb != 1)
    {
        printf("error: failed to write fmt chunk to %s\n", name);
        goto error;
    }

    rb = fwrite(&data_chunk, sizeof(data_chunk), 1, fp);
    if (rb != 1)
    {
        printf("error: failed to write data chunk to %s\n", name);
        goto error;
    }

    rb = fwrite(data, size, 1, fp);
    if (rb != 1)
    {
        printf("error: failed to write data to %s\n", name);
        goto error;
    }

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}
