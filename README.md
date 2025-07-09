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
In addition the reverb algorithm isn't anywhere near perfect, and lacks proper low-pass filtering at 11.025 kHz.

This project only currently supports writing the output to a wave file.

## building
`make`

## usage

`raven [-d] [-r] [-o output] [-l loops] [-s song] [-p phase] sdx [sdx2]`

Up to two sdx files can be provided.<br>
Resident sound files in `r_tnk` and `r_plt*` contain common samples used by all songs in their respective stages.<br>
Normally the first file provided would be a resident file, and the second would be the file containing the song to play.

The following options are supported:
```
-d          Enables debug output.
-r          Forces reverb off.
-o output   Specifies the name of the output wave file.
-l loops    Specifies the number of times to loop the song.
-s song     Specifies which of the songs from the sequence data to play.
-p phase    Specifies which phase to play for alert/evasion songs.
```

## sol branch

This branch adds changes to the original library required to play songs used by Metal Gear Solid 2:
- Expands the number of BGM channels from MGS's 13 to MGS2's count of 32.
- Adds support for the automated mixing used to transition between alert and evasion music.
- Increased the sampling rate of the SPU from 44.1 kHz to 48 kHz to match the PS2.
- Changes the timestep from 448 samples per tick to 240 samples per tick to match MGS2's 5 ms timer.
- Added support for loading the sdx data format used in MGS2.
