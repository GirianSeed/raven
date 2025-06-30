#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>

static int sd_sng_data_load(FILE *fp, size_t size)
{
    size_t rb;

    if (size > sizeof(sng_data))
    {
        SD_PRINT("ERROR:song data size exceeds max %zx\n", size);
        return 1;
    }

    rb = fread(sng_data, size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read song data\n");
        return 1;
    }

    sng_play_code = 0;
    sng_status = 2;

    return 0;
}

static int sd_wav_data_load(FILE *fp, size_t size)
{
    WAVE_H header;
    size_t rb;
    unsigned int last_addr;
    unsigned int entry;
    unsigned int addr;
    char *data;

    (void)size;

    /* read wave table header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table header\n");
        return 1;
    }

    header.offset = htonl(header.offset);
    header.size = htonl(header.size);

    assert((header.offset % sizeof(header)) == 0);
    assert((header.size % sizeof(header)) == 0);

    /* read wave table */
    rb = fread((char *)voice_tbl + header.offset, header.size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table\n");
        return 1;
    }

    last_addr = 0;
    for (unsigned int i = 0; i < header.size; i += 16)
    {
        entry = (header.offset + i) / 16;
        addr = voice_tbl[entry].addr;

        if (addr < last_addr)
        {
            SD_PRINT("drum start found at entry %u\n", entry);
            sd_drum_index = entry;
            break;
        }

        last_addr = addr;
    }

    /* read wave data header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave data header\n");
        return 1;
    }

    header.offset = htonl(header.offset);
    header.size = htonl(header.size);

    data = malloc(header.size);
    if (data == NULL)
    {
        SD_PRINT("ERROR:unable to allocate wave data temp\n");
        return 1;
    }

    /* read wave data */
    rb = fread(data, header.size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave data\n");
        free(data);
        return 1;
    }

    spu_write(header.offset, data, header.size);

    free(data);
    return 0;
}

#define PACK_BLOCK_SIZE 0x800

int sd_pack_data_load(const char *name)
{
    FILE *fp;
    PACK_H header;
    size_t size;
    size_t rb;
    size_t bgm_sequence_size;
    size_t bgm_waveform_size;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        SD_PRINT("ERROR:unable to open sound pack file\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read sound pack header\n");
        goto error;
    }

    header.offset_bgm_waveform *= PACK_BLOCK_SIZE;
    header.offset_sfx_waveform *= PACK_BLOCK_SIZE;
    header.offset_sfx_sequence *= PACK_BLOCK_SIZE;
    header.offset_bgm_sequence *= PACK_BLOCK_SIZE;

    bgm_sequence_size = size - header.offset_bgm_sequence;
    bgm_waveform_size = header.offset_sfx_waveform - header.offset_bgm_waveform;

    fseek(fp, header.offset_bgm_waveform, SEEK_SET);
    if (sd_wav_data_load(fp, bgm_waveform_size))
    {
        SD_PRINT("ERROR:unable to read BGM waveform data\n");
        goto error;
    }

    fseek(fp, header.offset_bgm_sequence, SEEK_SET);
    if (sd_sng_data_load(fp, bgm_sequence_size))
    {
        SD_PRINT("ERROR:unable to read BGM sequence data\n");
        goto error;
    }

    return 0;

error:
    fclose(fp);
    return 1;
}
