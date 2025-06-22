#include "vector.h"
#include "wave.h"

#include "sd/sd_cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TICK_SAMPLES     448
#define BUFFER_SIZE_LOG2 12 // 4KiB, enough for ~9 ticks

int main(int argc, char **argv)
{
    vector samples;

    const char *mdx = NULL;
    const char *wvx = NULL;

    int debug = 0;
    int loop = 0;
    int song = 1;

    if (argc < 2)
    {
        printf("usage: raven [-dl] [-s song] mdx wvx\n");
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-d") == 0)
        {
            debug = 1;
            continue;
        }

        if (strcmp(argv[i], "-l") == 0)
        {
            loop = 1;
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

        if (!wvx)
        {
            wvx = argv[i];
            continue;
        }

        printf("unknown argument: %s\n", argv[i]);
    }

    if (!mdx)
    {
        printf("error: missing required argument mdx\n");
        return 1;
    }

    if (!wvx)
    {
        printf("error: missing required argument wvx\n");
        return 1;
    }

    if (vector_init(&samples, 0))
    {
        printf("error: vector_init failed!\n");
        return 1;
    }

    sd_init(debug, loop);
    sd_sng_data_load(mdx);
    sd_wav_data_load(wvx);

    sd_set_cli(0xFF000006, SD_ASYNC); // stereo
    sd_set_cli(0xFF000006, SD_ASYNC); // reverb

    sd_set_cli(0x01000000 + song, SD_ASYNC); // song #1

    do
    {
        sd_tick();
    }
    while (sd_sng_play() || sd_se_play());

    if (write_wave_file("output.wav", samples.data, samples.size))
    {
        printf("error: failed to write output wave file!\n");
    }

    sd_term();
    vector_term(&samples);

    return 0;
}
