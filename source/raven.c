#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encoder/flac.h"
#include "encoder/opus.h"
#include "sd/sd_cli.h"
#include "spu/spu.h"

#define STEP_SIZE 448

int main(int argc, char **argv)
{
    ENCODER enc;
    short buffer[STEP_SIZE * 2];

    const char *mdx = NULL;
    const char *wvx[3] = {NULL, NULL, NULL};

    int debug = 0;
    const char *output = NULL;
    const char *encoder = NULL;
    int loops = 1;
    int song = 1;
    int reverb = 1;

    if (argc < 2)
    {
        printf("usage: raven [-d] [-r] [-l loops] [-s song] -o output -e encoder mdx wvx [wvx2] [wvx3]\n");
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

        if (strcmp(argv[i], "-l") == 0)
        {
            loops = atoi(argv[i + 1]);
            i++; // skip value
            continue;
        }

        if (strcmp(argv[i], "-s") == 0)
        {
            song = atoi(argv[i + 1]);
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

        if (!mdx)
        {
            mdx = argv[i];
            continue;
        }

        if (!wvx[0])
        {
            wvx[0] = argv[i];
            continue;
        }

        if (!wvx[1])
        {
            wvx[1] = argv[i];
            continue;
        }

        if (!wvx[2])
        {
            wvx[2] = argv[i];
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

    if (!mdx)
    {
        printf("error: missing required argument mdx\n");
        return 1;
    }

    if (!wvx[0])
    {
        printf("error: missing required argument wvx\n");
        return 1;
    }

    sd_init(debug, loops, reverb);
    sd_sng_data_load(mdx);
    sd_wav_data_load(wvx[0]);

    if (wvx[1])
    {
        sd_wav_data_load(wvx[1]);
    }

    if (wvx[2])
    {
        sd_wav_data_load(wvx[2]);
    }

    sd_set_cli(0xFF000006, SD_ASYNC); // stereo
    sd_set_cli(0x01000000 + song, SD_ASYNC); // song n

    if (strcmp(encoder, "flac") == 0)
    {
        flac_open(&enc, output);
    }
    else if (strcmp(encoder, "opus") == 0)
    {
        opus_open(&enc, output);
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
    }
    while (sd_sng_play() || sd_se_play());

    enc.close(&enc);
    printf("output written to %s\n", output);

    sd_term();
    return 0;
}
