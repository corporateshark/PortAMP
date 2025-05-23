PortAMP [![Build Status](https://github.com/corporateshark/PortAMP/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/corporateshark/PortAMP/actions)
=======

Copyright (c) Sergey Kosarevsky, 2015-2025

https://github.com/corporateshark/PortAMP

portamp@linderdaum.com

=============================

Portable command-line music player for Windows, macOS, and Linux

=============================

Usage:
------

```
portamp <filename> [<filename2> ...] [--loop] [--wav-modplug] [--verbose] [--gain <float>] [--output-file <file.wav>]
```

Features:
---------

* WAV (mono or stereo uncompressed PCM with 8-bit, 16-bit, 24-bit, 32-bit, 32-bit float or 64-bit double format, Microsoft ADPCM, IMA ADPCM, A-Law, Mu-Law - can decode everything from this page http://mauvecloud.net/sounds/index.html)
* MP3 (using the MiniMP3 library https://github.com/lieff/minimp3)
* OGG (using libogg/libvorbis https://xiph.org/downloads)
* FLAC (using libflac https://xiph.org/downloads)
* APE (using Monkey's Audio https://github.com/cristiklein/monkeys-audio)
* OPUS (using libopus https://www.opus-codec.org/downloads and libopusfile http://git.xiph.org/?p=opusfile.git)
* Modules: MOD, S3M, XM, IT, 669, AMF, AMS, DBM, DMF, DSM, FAR, MDL, MED, MTM, OKT, PTM, STM, ULT, UMX, MT2, PSM (using libmodplug http://modplug-xmms.sourceforge.net)

=============================

Clone:
------------------------

```
git clone https://github.com/corporateshark/PortAMP --recursive
```

UNIX build instructions:
------------------------

```
sudo apt-get install libopenal-dev
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make all -j4 -B
```

Windows build instructions:
---------------------------

```
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
start PortAMP.sln
```

=============================


License:
--------

The source code is available under the terms of the MIT License: https://github.com/corporateshark/PortAMP/blob/master/LICENSE
