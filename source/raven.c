#include "vector.h"
#include "wave.h"

#include "sd/sd_cli.h"
#include "spu/spu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEP_SIZE 448

int main(int argc, char **argv)
{
    vector samples;
    short buffer[STEP_SIZE * 2];

    const char *mdx = NULL;
    const char *wvx[3] = {NULL, NULL, NULL};

    int debug = 0;
    const char *output = "output.wav";
    int loops = 1;
    int song = 1;
    int reverb = 1;

    if (argc < 2)
    {
        printf("usage: raven [-d] [-r] [-o output] [-l loops] [-s song] mdx wvx [wvx2] [wvx3]\n");
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

        if (strcmp(argv[i], "-o") == 0)
        {
            output = argv[i + 1];
            i++; // skip value
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

        printf("unknown argument: %s\n", argv[i]);
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

    if (vector_init(&samples, 0))
    {
        printf("error: vector_init failed!\n");
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

    do
    {
        sd_tick();
        spu_step(STEP_SIZE, buffer);

        vector_push(&samples, buffer, sizeof(buffer));
    }
    while (sd_sng_play() || sd_se_play());

    if (write_wave_file(output, samples.data, samples.size))
    {
        printf("error: failed to write output wave file!\n");
    }

    sd_term();
    vector_term(&samples);

    return 0;
}
