# hltools
Hermes-Lite SDR tools

Python folder
=============

 * metis-detect.py: attempt to detect hpsdr devices on the local network.
   Send a probe and display all responses. minimal formatting.

 * metis-emu.py: this script emulates the behaviour of a hpsdr card.
   Only the detection is implemented, so I can test the detector script.

The C files in src/ and include/ are a library for protocol 1 openhpsdr devices, but written from scratch.
