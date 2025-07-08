#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int sd_sng_data_load(const char *name)
{
    FILE *fp;
    size_t size;
    size_t rb;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        SD_PRINT("ERROR:could not open song file %s\n", name);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size > sizeof(sng_data))
    {
        SD_PRINT("ERROR:song data size exceeds max %zx\n", size);
        goto error;
    }

    rb = fread(sng_data, size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read song data\n");
        goto error;
    }

    sng_play_code = 0;
    sng_status = 2;

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
}

int sd_se_data_load(const char *name)
{
    FILE *fp;
    size_t size;
    size_t rb;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        SD_PRINT("ERROR:could not open se file %s\n", name);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size > sizeof(se_header))
    {
        SD_PRINT("ERROR:se data size exceeds max %zx\n", size);
        goto error;
    }

    rb = fread(se_header, size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read se data\n");
        goto error;
    }

    fclose(fp);
    return 0;

error:
    fclose(fp);
    return 1;
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

int sd_wav_data_load(const char *name)
{
    FILE *fp;
    WAVE_H header;
    size_t rb;
    char *data;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        SD_PRINT("ERROR:could not open wave file %s\n", name);
        return 1;
    }

    /* read wave table header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table header\n");
        goto error;
    }

    header.offset = read_uint32(&header.offset);
    header.size = read_uint32(&header.size);

    assert((header.offset % sizeof(header)) == 0);
    assert((header.size % sizeof(header)) == 0);

    /* read wave table */
    rb = fread((char *)voice_tbl + header.offset, header.size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave table\n");
        goto error;
    }

    /* read wave data header */
    rb = fread(&header, sizeof(header), 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave data header\n");
        goto error;
    }

    header.offset = read_uint32(&header.offset);
    header.size = read_uint32(&header.size);

    data = malloc(header.size);
    if (data == NULL)
    {
        SD_PRINT("ERROR:unable to allocate wave data temp\n");
        goto error;
    }

    /* read wave data */
    rb = fread(data, header.size, 1, fp);
    if (rb != 1)
    {
        SD_PRINT("ERROR:unable to read wave data\n");
        goto error2;
    }

    spu_write(header.offset, data, header.size);

    free(data);
    fclose(fp);
    return 0;

error2:
    free(data);

error:
    fclose(fp);
    return 1;
}
