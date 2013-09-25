STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.1

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

STK for Linux is currently using the OSS sound and MIDI API.  The free version of OSS will probably work, though it doesn't work with as many soundcards as the commercial version (which costs about $20).  The Advanced Linux Sound Architecture (ALSA) drivers seem to perform well, though I've done only minimal testing with them.  You'll need to install the OSS compatability library.  You can read more about ALSA at http://www.alsa-project.org/.  ALSA is open source and holds great promise for audio under Linux.

STK should compile without much trouble under Linux.  Since all Linux distributions typically include the GNU makefile utilities, you should be able to use the default Makefile.  Typing "make" will initiate the compilation process.

NOTE REGARDING PTHREADS:

The only issue which seems to crop up on different versions of Linux concerns threads.  I am using the MIT pthreads API.  Under RedHat Linux 4.x, I had to specifically include <pthread/mit/pthread.h> (the default pthread library didn't work).  However, under RedHat Linux 5.0 and higher, the default works and the <pthread/mit/> path doesn't exist.  I've decided to assume the default works.  If you get errors with regard to pthreads when you compile, you'll have to search your system for the MIT pthread distribution and change the appropriate include statements in MIDIIO.cpp, MD2SKINI.cpp, and threads.cpp.


