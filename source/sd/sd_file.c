#include "sd/sd_ext.h"
#include "sd/sd_incl.h"

#include <assert.h>
#include <stdio.h>

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

// TODO: SETBL.addr rely on size(char *) being 4
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

    if (size > sizeof(se_exp_table))
    {
        SD_PRINT("ERROR:se data size exceeds max %zx\n", size);
        goto error;
    }

    rb = fread(se_exp_table, size, 1, fp);
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

int sd_wav_data_load(const char *name)
{
    FILE *fp;

    fp = fopen(name, "rb");
    if (fp == NULL)
    {
        return 1;
    }

    // TODO
    assert(0);

    fclose(fp);
    return 0;
}
