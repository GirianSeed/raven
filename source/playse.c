#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "encoder/flac.h"
#include "encoder/opus.h"
#include "sd/sd_cli.h"
#include "spu/spu.h"

int main(int argc, char **argv)
{
    ENCODER enc;
    short buffer[STEP_SIZE * 2];

    const char *sdx[2] = {NULL, NULL};

    int debug = 0;
    int reverb = 1;
    int se_id = -1;
    int min = 0;
    int max = 512;
    const char *output = NULL;
    const char *encoder = NULL;
    char name[128];
    int count;

    if (argc < 2)
    {
        printf("usage: raven [-d] [-r] [-s se_id] -o outpath -e encoder sdx [sdx2]\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug = 1;
            continue;
        }

        if (strcmp(argv[i], "-r") == 0)
        {
            reverb = 0;
            continue;
        }

        if (strcmp(argv[i], "-s") == 0)
        {
            se_id = atoi(argv[i + 1]);
            i++; // skip value
            continue;
        }

        if (strcmp(argv[i], "-o") == 0)
        {
            output = argv[i + 1];
            i++; // skip value
            continue;
        }

        if (strcmp(argv[i], "-e") == 0)
        {
            encoder = argv[i + 1];
            i++; // skip value
            continue;
        }

        if (argv[i][0] == '-')
        {
            printf("warning: unrecognised option: %s\n", argv[i]);
            continue;
        }

        if (!sdx[0])
        {
            sdx[0] = argv[i];
            continue;
        }

        if (!sdx[1])
        {
            sdx[1] = argv[i];
            continue;
        }

        printf("warning: unknown argument: %s\n", argv[i]);
    }

    if (!output)
    {
        printf("error: missing required argument output\n");
        return 1;
    }

    if (!encoder)
    {
        printf("error: missing required argument encoder\n");
        return 1;
    }

    if (!sdx[0])
    {
        printf("error: missing required argument sdx\n");
        return 1;
    }

    if (se_id >= 0)
    {
        min = se_id;
        max = se_id + 1;
    }

    for (int i = min; i < max; i++)
    {
        count = 0;

        sd_init(debug, 1, reverb);
        sd_pack_data_load(sdx[0]);

        if (sdx[1])
        {
            sd_pack_data_load(sdx[1]);
        }

        sd_set_cli(0xFF000006, SD_ASYNC); // stereo
        sd_set_cli((32 << 18) | (63 << 12) | i, SD_ASYNC);

        if (strcmp(encoder, "flac") == 0)
        {
            snprintf(name, sizeof(name), "%s/se%03d.flac", output, i);
            flac_open(&enc, name);
        }
        else if (strcmp(encoder, "opus") == 0)
        {
            snprintf(name, sizeof(name), "%s/se%03d.ogg", output, i);
            opus_open(&enc, name);
        }
        else
        {
            printf("error: unknown encoder %s\n", encoder);
            return 1;
        }

        do
        {
            sd_tick();
            spu_step(STEP_SIZE, buffer);
            enc.write(&enc, buffer, STEP_SIZE);
            count++;

            if (count == 500)
            {
                sd_set_cli(0xFF0000FE, SD_ASYNC); // stop resident sounds.
            }
        }
        while (sd_sng_play() || sd_se_play());

        enc.close(&enc);
    }

    sd_term();
    return 0;
}
