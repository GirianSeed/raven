#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sd/sd_ext.h"
#include "sd/sd_incl.h"
#include "spu/spu.h"

static int sd_sng_data_load(FILE *fp, size_t size)
{
    if (size > sizeof(sng_data))
    {
        SD_WARN("ERROR:song data size exceeds max %zx\n", size);
        return 1;
    }

    if (fread(sng_data, size, 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read song data\n");
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
    WAVE_H tbl_header;
    WAVE_H spu_header;
    int drum_offset;
    unsigned int last_addr;
    unsigned int addr;
    WAVE_W *table;
    char *data;

    (void)size;

    /* read wave table header */
    if (fread(&tbl_header, sizeof(tbl_header), 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read wave table header\n");
        return 1;
    }

    tbl_header.offset = read_uint32(&tbl_header.offset);
    tbl_header.size = read_uint32(&tbl_header.size);

    assert((tbl_header.offset % sizeof(tbl_header)) == 0);
    assert((tbl_header.size % sizeof(tbl_header)) == 0);

    SD_PRINT("wave table offset = %x\n", tbl_header.offset);
    SD_PRINT("wave table size = %x\n", tbl_header.size);

    table = malloc(tbl_header.size);
    if (table == NULL)
    {
        SD_WARN("ERROR:unable to allocate wave table temp\n");
        return 1;
    }

    /* read wave table */
    if (fread(table, tbl_header.size, 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read wave table\n");
        free(table);
        return 1;
    }

    drum_offset = -1;
    last_addr = 0;
    for (unsigned int i = 0; i < tbl_header.size / sizeof(WAVE_W); i++)
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
        memcpy((char *)voice_tbl + tbl_header.offset, table, tbl_header.size);
    }
    else
    {
        /* load each section to the correct table */
        memcpy((char *)voice_tbl + tbl_header.offset, table, drum_offset);
        memcpy(drum_tbl, (char *)table + drum_offset, tbl_header.size - drum_offset);
    }

    free(table);

    /* read wave data header */
    if (fread(&spu_header, sizeof(spu_header), 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read wave data header\n");
        return 1;
    }

    spu_header.offset = read_uint32(&spu_header.offset);
    spu_header.size = read_uint32(&spu_header.size);

    SD_PRINT("wave spu offset = %x\n", spu_header.offset);
    SD_PRINT("wave spu size = %x\n", spu_header.size);

    data = malloc(spu_header.size);
    if (data == NULL)
    {
        SD_WARN("ERROR:unable to allocate wave data temp\n");
        return 1;
    }

    /* read wave data */
    if (fread(data, spu_header.size, 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read wave data\n");
        free(data);
        return 1;
    }

    /* entries 0-255 are SPU samples, entries 256-511 are streamed samples */
    if (tbl_header.offset < 0x1000)
    {
        spu_write(spu_header.offset + 0x20000, data, spu_header.size);
    }
    else
    {
        SD_PRINT("copying data to memory streaming buffer\n");
        memcpy(mem_str_buf + spu_header.offset, data, spu_header.size);
    }

    free(data);
    return 0;
}

static int sd_efx_data_load(FILE *fp, size_t size)
{
    unsigned char table[2048];

    if (fread(table, sizeof(table), 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read efx table\n");
        return 1;
    }

    if (table[0] == 0xfe && table[1] == 0xfe)
    {
        SD_PRINT("found resident efx data\n");

        if (size > 0x7800)
        {
            SD_WARN("ERROR: efx data too large\n");
            return 1;
        }

        if (fread(se_header, size - 2048, 1, fp) != 1)
        {
            SD_WARN("ERROR:unable to read efx data\n");
            return 1;
        }
    }
    else
    {
        SD_PRINT("found additional efx data\n");

        if (size > 0x4800)
        {
            SD_WARN("ERROR: efx data too large\n");
            return 1;
        }

        memcpy(se_exp_table, table, 2048);

        if (fread(se_exp_header, size - 2048, 1, fp) != 1)
        {
            SD_WARN("ERROR:unable to read efx data\n");
            return 1;
        }
    }

    return 0;
}

#define PACK_BLOCK_SIZE 0x800

int sd_pack_data_load(const char *name)
{
    FILE *fp;
    PACK_H header;
    size_t size;
    size_t wvx1_size;
    size_t wvx2_size;
    size_t efx_size;
    size_t mdx_size;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        SD_WARN("ERROR:unable to open sound pack file\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (fread(&header, sizeof(header), 1, fp) != 1)
    {
        SD_WARN("ERROR:unable to read sound pack header\n");
        goto error;
    }

    header.wvx1_offset *= PACK_BLOCK_SIZE;
    header.wvx2_offset *= PACK_BLOCK_SIZE;
    header.efx_offset *= PACK_BLOCK_SIZE;
    header.mdx_offset *= PACK_BLOCK_SIZE;

    wvx1_size = header.wvx2_offset - header.wvx1_offset;
    wvx2_size = header.efx_offset - header.wvx2_offset;
    efx_size = header.mdx_offset - header.efx_offset;
    mdx_size = size - header.mdx_offset;

    fseek(fp, header.wvx1_offset, SEEK_SET);
    if (sd_wav_data_load(fp, wvx1_size))
    {
        SD_WARN("ERROR:unable to read WVX1\n");
        goto error;
    }

    fseek(fp, header.wvx2_offset, SEEK_SET);
    if (sd_wav_data_load(fp, wvx2_size))
    {
        SD_WARN("ERROR:unable to read WVX2\n");
        goto error;
    }

    fseek(fp, header.efx_offset, SEEK_SET);
    if (sd_efx_data_load(fp, efx_size))
    {
        SD_WARN("ERROR:unable to read EFX\n");
        goto error;
    }

    fseek(fp, header.mdx_offset, SEEK_SET);
    if (sd_sng_data_load(fp, mdx_size))
    {
        SD_WARN("ERROR:unable to read MDX\n");
        goto error;
    }

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}
