STK98v2: A ToolKit of Audio Synthesis Classes and Instruments in C++

By Perry R. Cook, 1995-98
With recent help by Gary P. Scavone

Please read the Legal and Ethical notes at the bottom of this document.

STK has undergone a large number of revisions, changes, and additions since its initial release in 1996.  With this version 2.0 release, it has been completely ported to Linux and Win95 (not tested on WinNT but should work), as well as SGI and NeXTStep (no real time capabilities under NeXTStep).  See the individual README's (eg. README-linux) for platform specific information and system requirements.  In general, you will have to specify your system type in Object.h and then use either the Makefile (Unix platforms) or the VC++ workspace file (STK98v2.dsw) to compile the code.

STK now features realtime sound output and MIDI input under SGI, Linux (OSS), and Win95/NT (Direct Sound and Winmm routines).  It is also possible to generate simultaneous .snd, .wav, and .mat (Matlab) output file types, as well as SKINI scorefiles using MD2SKINI.

Two primary executables are created during compilation - syntmono and MD2SKINI.  Syntmono is the core STK synthesis server.  All distributed STK instruments are run using syntmono.  MD2SKINI takes raw MIDI input, converts it to SKINI format, and outputs the result to stdout or a socket port ID (under Windoze).  Control data (in the form of SKINI messages) can be fed to syntmono through three principal means - SKINI scorefiles, MD2SKINI output, and Tcl/Tk GUIs.  A variety of SKINI scorefiles are distributed with STK98 and can be found in the "scores" directory.  Under all platforms, a scorefile can be piped or redirected to syntmono in the following way:

	more scores/streetsf.ski | syntmono Clarinet -r
or
	syntmono Clarinet -r < scores/streetsf.ski

A number of Tcl/Tk GUIs are provided in the "TCLSpecs" directory, though you will have to install Tcl/Tk version 8.0 or higher on your system to use them (older versions of Tcl/Tk under Linux seem to be more forgiving than under IRIX).  Realtime SKINI control data (via MD2SKINI or GUIs) can be piped to syntmono on Unix platforms and perhaps under WinNT in the following way:

	MD2SKINI | syntmono Clarinet -r -i
or
	wish < TCLSpecs/TCLPhys.tcl | syntmono Clarinet -r -i

It is not possible to use realtime pipes under Win95, so socket communication is used instead.  Perhaps in the future, all STK communications will be conducted using sockets.  For socket communication, it is necessary to first start the syntmono socket server.  For the time being, a default (hardwired) socket port of 2001 is being used by syntmono.  After syntmono is running (and waiting for a socket client connection), either MD2SKINI or a Tcl/Tk GUI can be started.  When using the GUI, it is necessary to invoke the "communications" menu item and select "socket" to establish the connection.

For more documentation on this ToolKit, the classes, etc, read the file HIERARCH.txt and the individual class definitions.  Also check the platform specific README's for specific system requirements.

You probably already guessed this, but just to be sure, we don't guarantee anything works.  :-)  It's free ... what do you expect?  If you find a bug, please let us know and we'll try to correct it.  You can also make suggestions, but again, no guarantees.  Send email to prc@cs.princeton.edu and gary@ccrma.stanford.edu. 

Perry's comments from the original distribution:

This whole world was created with no particular hardware in mind.  These examples are intended to be tutorial in nature, as a platform for the continuation of my research, and as a possible starting point for a software synthesis system.  The basic motivation was to create the necessary unit generators to do the synthesis, processing, and control that I want to do and teach about.  Little thought for optimization was given (see Object.cpp), and therefore improvements, especially speed enhancements, should be possible with these classes.  It was written with some basic concepts in mind about how to let compilers optimize.

Your question at this point might be, "But Perry, with CMix, CMusic, CSound, CShells, CMonkeys, etc. already cluttering the landscape, why a new set of stupid C functions for music synthesis and processing?"  The answers lie below.

1) I needed to port many of the things I've done
   into something which is generic enough to port
   further to different machines.

2) I really plan to document this stuff, so that
   you don't have to be me to figure out what's 
   going on. (I'll probably be sorry I said this 
   in a couple of years, when even I can't figure
   out what I was thinking.)

3) The classic difficulties most people have in 
   trying to implement physical models are:

   A) They have trouble understanding the papers, 
      and/or in turning the theory into practice.

   B) The Physical Model instruments are a pain to get 
      to oscillate, and coming up with stable and 
      meaningful parameter values is required to 
      get the models to work at all.

   This set of C++ unitgenerators and instruments
   might help to diminish the scores of EMails I
   get asking what to do with those block diagrams
   I put in my papers.

4) I wanted to try some new stuff with modal synthesis, 
   and implement some classic FM patches as well.

5) I wanted to reimplement, and newly implement
   more of the intelligent and physical performer
   models I've talked about in some of my papers.
   But I wanted to do it in a portable way, and in 
   such a way that I can hook up modules quickly.
   I also wanted to make these instruments connectable
   to such player objects, so folks like Brad Garton
   who really think a lot about the players can connect
   them to my instruments, a lot about which I think.

6) More rationalizations to follow . . .


/******************************************************/

Legal and Ethical:

This software was designed and created to be made publicly available for free, primarily for academic purposes, so if you use it, pass it on with this documentation, and for free.  

If you make a million dollars with it, give me some.  If you make compositions with it, put me in the program notes.

Some of the concepts are covered by various patents, some known to me and likely others which are unknown.  Many of the ones known to me are administered by the Stanford Office of Technology and Licensing.  

The good news is that large hunks of the techniques used here are public domain.  To avoid subtle legal issues, I'll not state what's freely useable here, but I'll try to note within the various classes where certain things are likely to be protected by patents.

/******************************************************/

