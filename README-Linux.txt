STK98v2: A ToolKit of Audio Synthesis Classes and Instruments in C++

By Perry R. Cook, 1995-98
With recent help by Gary P. Scavone

Please read the file README.txt for more general STK information.

STK98 for Linux is currently using the OSS sound and MIDI API.  The free version of OSS will probably work, though it doesn't work with as many soundcards as the commercial version (costs about $20).

STK98 should compile without much trouble, after you make the appropriate settings in Object.h and select an appropriate Makefile.  Since all Linux systems should come with the GNU makefile utilities, you should be able to use either Makefile.all or Makefile.linux (which should be renamed "Makefile" before using).  Typing "make" should initiate the compilation process.

NOTE REGARDING PTHREADS: The only issue which seems to crop up on different versions of Linux concerns threads.  I am using the MIT pthreads API.  Under RedHat Linux 4.x, I had to specifically include <pthread/mit/pthread.h> (the default pthread library didn't work).  However, under RedHat Linux 5.0, the default works and the <pthread/mit/> path doesn't exist.  I've decided to assume the default works.  If you get errors with regard to pthreads when you compile, you'll have to search your system for the MIT pthread distribution and change the appropriate include statements in MIDIIO.cpp, MD2SKINI.cpp, and syntmono.cpp.

Once everything is compiled, you can use the scripts in "TCLSpecs" to run GUIs or invoke direct MIDI input controls.  However, these scripts have been written for Tcl/Tk 8.0 ... they seem to work OK on older versions of Tcl/Tk under Linux, but not under IRIX.  Tcl/Tk is free, so you might as well download the newest version.

Two primary executables are created during compilation - syntmono and MD2SKINI.  Syntmono is the core STK synthesis server.  All distributed STK instruments are run using syntmono.  MD2SKINI takes raw MIDI input, converts it to SKINI format, and outputs the result to stdout or a socket port ID (under Windoze).  Control data (in the form of SKINI messages) can be fed to syntmono through three principal means - SKINI scorefiles, MD2SKINI output, and Tcl/Tk GUIs.  A variety of SKINI scorefiles are distributed with STK98 and can be found in the "scores" directory.  Under all platforms, a scorefile can be piped or redirected to syntmono in the following way:

	more scores/streetsf.ski | syntmono Clarinet -r
or
	syntmono Clarinet -r < scores/streetsf.ski

A number of Tcl/Tk GUIs are provided in the "TCLSpecs" directory, though you will have to install Tcl/Tk on your system to use them.  Realtime SKINI control data (via MD2SKINI or GUIs) can be piped to syntmono on Unix platforms in the following way:

	MD2SKINI | syntmono Clarinet -r -i
or
	wish < TCLSpecs/TCLPhys.tcl | syntmono Clarinet -r -i


/******************************************************/

Legal and Ethical:

This software was designed and created to be made publicly available for free, primarily for academic purposes, so if you use it, pass it on with this documentation, and for free.  

If you make a million dollars with it, give me some.  If you make compositions with it, put me in the program notes.

Some of the concepts are covered by various patents, some known to me and likely others which are unknown.  Many of the ones known to me are administered by the Stanford Office of Technology and Licensing.  

The good news is that large hunks of the techniques used here are public domain.  To avoid subtle legal issues, I'll not state what's freely useable here, but I'll try to note within the various classes where certain things are likely to be protected by patents.

/******************************************************/

