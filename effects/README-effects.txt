STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.0

By Perry R. Cook, 1995-99
and Gary P. Scavone, 1997-99.


EFFECTS PROJECT:

This directory contains a program that demonstrates realtime duplex mode (simultaneous audio input and output) operation, as well as several simple delay-line based effects algorithms.  Proper duplex mode operation is very hardware dependent.  If you have trouble with this application, make sure your soundcard supports the desired sample rates and sample sizes.  Under Linux, also make sure that your soundcard is supported by OSS (http://www.opensound.com/).

NOTES: 

1. This project will not run under WindowsNT or NeXTStep, due to lack of realtime audio input support.

2. Audio input from either a microphone or line-input device MUST be available to the audio input port when the program is started.