# raven

This is a port of the sound library written by Kazuki Muraoka for Metal Gear Solid.<br>
This library was later expanded for Metal Gear Solid 2 as well as the Zone of the Enders series.

The library includes a PlayStation SPU emulator for audio synthesis.<br>
This is adapted from [rpsx](https://github.com/KieronJ/rpsx)'s SPU implementation.<br>

The following sources were referenced to improve rpsx's SPU implementation and fix bugs:
- nocash's [psx-spx](https://psx-spx.consoledev.net) hardware documentation.
- [DuckStation](https://github.com/stenzek/duckstation)'s SPU implementation.
- [Mednafen](https://mednafen.github.io)'s half-band reverb resampling implementation.

This project is still very much a work-in-progress. Expect some songs to sound slightly (or very) incorrect.<br>
In addition, the reverb algorithm isn't bit-accurate as it uses floats to avoid dealing with rounding behaviour.

This project only currently supports writing the output to a wave file.

## building
`make`

## usage

`raven [-d] [-r] [-o output] [-l loops] [-s song] mdx wvx [wvx2] [wvx3]`

One mdx file and up to three wvx files can be provided:
- mdx files contain the sequence data for songs.
- wvx files contain the waveform data for songs.

The resident wvx file `wv00007f.wvx` located in the init stages contains common samples used by all songs.<br>
Some songs require additional wvx files, these are found in the same stage as the mdx file.

The following options are supported:
```
-d          Enables debug output.
-r          Forces reverb off.
-o output   Specifies the name of the output wave file.
-l loops    Specifies the number of times to loop the song.
-s song     Specifies which of the songs from the sequence data to play.
```
