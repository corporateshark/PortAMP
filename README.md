PortAMP
=======

Copyright (c) Sergey Kosarevsky, 2015

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

* WAV (uncompressed PCM, 8 or 16-bit, mono or stereo)
* MP3 (using the MiniMP3 library http://keyj.emphy.de/minimp3)
* OGG (using libogg/libvorbis https://xiph.org/downloads)
* FLAC (using libflac https://xiph.org/downloads)
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
