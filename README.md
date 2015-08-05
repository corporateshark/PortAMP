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
portamp <filename> [--loop]
```

Features:
---------

* WAV (uncompressed PCM, 8 or 16-bit, mono or stereo)
* MP3 (using the MiniMP3 library http://keyj.emphy.de/minimp3)
* OGG (using libogg/libvorbis https://xiph.org/downloads)

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
