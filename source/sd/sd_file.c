#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include "spu/spu.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static inline unsigned int read_uint32(const void *ptr)
{
    unsigned int val = 0;

    val |= ((unsigned char *)ptr)[0] << 24;
    val |= ((unsigned char *)ptr)[1] << 16;
    val |= ((unsigned char *)ptr)[2] << 8;
    val |= ((unsigned char *)ptr)[3];

    return val;
}

static int sd_wav_data_load(FILE *fp, size_t size)
{
    WAVE_H header;
    size_t rb;
    int drum_offset;
    unsigned int last_addr;
    unsigned int addr;
    WAVE_W *table;
    char *data;

    (void)size;

    /* read wave table header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table header\n");
        return 1;
    }

    header.offset = read_uint32(&header.offset);
    header.size = read_uint32(&header.size);

    assert((header.offset % sizeof(header)) == 0);
    assert((header.size % sizeof(header)) == 0);

    SD_PRINT("wave data offset = %x\n", header.offset);
    SD_PRINT("wave data size = %x\n", header.size);

    table = malloc(header.size);
    if (table == NULL)
    {
        SD_PRINT("ERROR:unable to allocate wave table temp\n");
        return 1;
    }

    /* read wave table */
    rb = fread(table, header.size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table\n");
        free(table);
        return 1;
    }

    drum_offset = -1;
    last_addr = 0;
    for (unsigned int i = 0; i < header.size / sizeof(WAVE_W); i++)
    {
        addr = table[i].addr;

        if (addr < last_addr)
        {
            SD_PRINT("drum start found at entry %u\n", i);
            drum_offset = i * sizeof(WAVE_W);
            break;
        }

        last_addr = addr;
    }

    if (drum_offset < 0)
    {
        /* load the entire file to the voice table */
        memcpy((char *)voice_tbl + header.offset, table, header.size);
    }
    else
    {
        /* load each section to the correct table */
        memcpy((char *)voice_tbl + header.offset, table, drum_offset);
        memcpy(drum_tbl, (char *)table + drum_offset, header.size - drum_offset);
    }

    free(table);

    /* read wave data header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave data header\n");
        return 1;
    }

    header.offset = read_uint32(&header.offset);
    header.size = read_uint32(&header.size);

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

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}
