# raven

This is a port of the sound library written by Kazuki Muraoka for Metal Gear Solid.<br>
This library was later expanded for Metal Gear Solid 2 as well as the Zone of the Enders series.

The library includes a PlayStation SPU emulator for audio synthesis.<br>
This is adapted from [rpsx](https://github.com/KieronJ/rpsx)'s SPU implementation.<br>

The following sources were referenced to improve rpsx's SPU implementation and fix bugs:
- nocash's [psx-spx](https://psx-spx.consoledev.net) hardware documentation.
- [DuckStation](https://github.com/stenzek/duckstation)'s SPU implementation.
- [Mednafen](https://mednafen.github.io)'s reverb implementation.

This project is still very much a work-in-progress. Expect some songs to sound slightly (or very) incorrect.

This project supports both FLAC and Opus Ogg output.

## required packages
`libflac-dev libopusenc-dev`

## building
`make`

## usage

One mdx file, one efx file and up to three wvx files can be provided:
- mdx files contain the sequence data for songs.
- efx files contain the sequence data for sound effects.
- wvx files contain the waveform data for songs/sound effects.

The resident wvx file `wv00007f.wvx` located in the init stages contains common samples used by all songs.<br>
Some songs require additional wvx files, these are found in the same stage as the mdx file.

### raven
`raven [-d] [-r] [-l loops] [-s song] -o output -e encoder mdx wvx [wvx2] [wvx3]`

The following options are supported:
```
-d          Enables debug output.
-r          Forces reverb off.
-l loops    Specifies the number of times to loop the song.
-s song     Specifies which of the songs from the sequence data to play.
-o output   Specifies the name of the output wave file.
-e encoder  Specifies the encoder to use. ("flac" | "opus")
```

### playse
`playse [-d] [-r] [-s se_id] -o output -e encoder efx wvx [wvx2] [wvx3]`

The following options are supported:
```
-d          Enables debug output.
-r          Forces reverb off.
-s se_id    Specifies the sound effect ID to play, otherwise play all.
-o outpath  Specifies the path of the output wave files.
-e encoder  Specifies the encoder to use. ("flac" | "opus")
```