PortAMP
=======

Copyright (c) Sergey Kosarevsky, 2015-2016

https://github.com/corporateshark/PortAMP

portamp@linderdaum.com

=============================

Portable command-line music player for Windows, OS X and Linux

=============================

Usage:
------

```
portamp <filename> [<filename2> ...] [--loop] [--wav-modplug] [--verbose]
```

Features:
---------

* WAV (mono or stereo uncompressed PCM with 8-bit, 16-bit, 24-bit, 32-bit, 32-bit float or 64-bit double format, Microsoft ADPCM, IMA ADPCM, A-Law, Mu-Law - can decode everything from this page http://mauvecloud.net/sounds/index.html)
* MP3 (using the MiniMP3 library http://keyj.emphy.de/minimp3)
* OGG (using libogg/libvorbis https://xiph.org/downloads)
* FLAC (using libflac https://xiph.org/downloads)
* APE (using Monkey's Audio https://github.com/cristiklein/monkeys-audio)
* OPUS (using libopus https://www.opus-codec.org/downloads and libopusfile http://git.xiph.org/?p=opusfile.git)
* Modules: MOD, S3M, XM, IT, 669, AMF, AMS, DBM, DMF, DSM, FAR, MDL, MED, MTM, OKT, PTM, STM, ULT, UMX, MT2, PSM (using libmodplug http://modplug-xmms.sourceforge.net)

=============================

UNIX build instructions:
------------------------

```
git clone https://github.com/corporateshark/PortAMP
cd PortAMP
mkdir build
cd build
cmake .. -G "Unix Makefiles"
make all -j4 -B
```

Windows build instructions:
---------------------------

```
git clone https://github.com/corporateshark/PortAMP
cd PortAMP
mkdir build
cd build
cmake .. -G "Visual Studio 12 2013"
start PortAMP.sln
```

=============================


License:
--------

The source code is available under the terms of the MIT License: https://github.com/corporateshark/PortAMP/blob/master/LICENSE
