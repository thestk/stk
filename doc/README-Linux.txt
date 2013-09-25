STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.2

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

STK for Linux is currently using either the Open Sound System (OSS) or the Advanced Linux Sound Architecture (ALSA) sound and MIDI APIs.  The free version of OSS works as well (and in some cases better than the commercial OSS version ... such as with my Maestro 2e chipset).  In general, the ALSA drivers seem to perform well though we have had some problems with them at CCRMA.  You can read more about ALSA at http://www.alsa-project.org/.  ALSA is open source and holds great promise for audio under Linux.  Select (uncomment) the proper API #define statement in Object.h.

STK should compile without much trouble under Linux ... afterall, it is primarily developed on Linux platforms.  Since all Linux distributions typically include the GNU makefile utilities, you should be able to use the default Makefile.  Typing "make" will initiate the compilation process.

MIDIATOR SERIAL PORT MIDI SUPPORT:

STK now has special support for the MIDIator serial port MIDI interface.  This is of primary interest to us laptop users, whose computers usually don't have a gameport.  If you want to buy one of these devices, make sure you get the MS-124w model (www.midiator.com).  For it to work in STK, make sure you uncomment the MIDIATOR define statement in Object.h.  This support currently only works within the OSS API framework, though I should be able to get it to work with ALSA in the future as well.

There are a few things that need to be done on your system to get the MIDIator working. Add the following lines to your bootup sequence in /etc/rc.d/rc.local:

setserial /dev/ttyS0 baud_base 57600
setserial /dev/ttyS0 divisor 1

You may need to specify the full path to the setserial function, depending on how your PATH variable is set up.  Also, you may need to modify the permissions of /dev/ttyS0 (chmod a+rwx).  And finally, the MIDIator should be set for "single addresssed" mode (the S/A switch on S and the A/B switch on A), which puts identical output on all 4 MIDI output ports.  It is possible to use the MIDIator in a "multi-port" mode, though I'm not currently supporting that in STK.

NOTE REGARDING PTHREADS:

There haven't been any problems with threads since the old days of RedHat Linux 5.0.  STK uses the MIT pthreads API.


