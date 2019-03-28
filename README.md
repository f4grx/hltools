# hltools
Hermes-Lite SDR tools

How to install
==============
You need cmake and a working toolchain for your machine.
There are no dependencies. The simplest way is to:

```
cd sourcetree
cmake .
make
```

Binaries will be in the ```src``` subdirectory.

Alternatively you can use an out-of tree build and a distinct install dir:

```
mkdir installdir
mkdir builddir
cd builddir
cmake -DCMAKE_INSTALL_PRFIX=../installdir ../sourcedir
make
make install
```

Binaries will be in the ```bin``` subdirectory of your install dir.

hpsdr_enum
----------
Enumerates protocol1 devices in the local net. There are no command line
arguments.

Python folder
=============

This folder is not managed by the cmake build described before

 * metis-detect.py: attempt to detect hpsdr devices on the local network.
   Send a probe and display all responses. minimal formatting.

 * metis-emu.py: this script emulates the behaviour of a hpsdr card.
   Only the detection is implemented, so I can test the detector script.

