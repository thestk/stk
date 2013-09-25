STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.2

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the Legal and Ethical notes near the bottom of this document.


OVERVIEW:

STK is a set of audio signal processing C++ classes and instruments for music synthesis.  You can use these classes to create programs which make cool sounds using a variety of synthesis techniques.  This is not a terribly novel concept, except that STK is very portable (it's mostly platform-independent C and C++ code) AND it's completely user-extensible.  So, the code you write using STK actually has some chance of working in another 5-10 years.  STK currently runs with "realtime" support (audio and MIDI) on SGI (Irix), Linux, and Windows computer platforms.  Generic, non-realtime support has been tested under NeXTStep, but should work with any standard C++ compiler.  STK is free for non-commercial use.  The only parts of STK that are platform-dependent concern real-time sound, MIDI, and control input and output ... but we've taken care of that for you.  The interface for MIDI input and the simple Tcl/Tk graphical user interfaces (GUIs) provided is the same, so it's easy to voice and experiment in real time using either the GUIs or MIDI.

STK isn't one particular program.  Rather, STK is a set of C++ classes that you can use to create your own programs.  We've provided a few example applications that demonstrate some of the ways that you could use these classes.  But if you have specific needs you will probably have to either modify the example programs or write a new program altogether.  Further, the example programs don't have a fancy GUI wrapper.  If you feel the need to have a "drag and drop" GUI, you probably don't want to use STK.  Spending hundreds of hours making platform-dependent GUI code would go against one of the fundamental design goals of STK - platform independence.  STK can generate simultaneous .snd, .wav, .aif, and .mat output soundfile formats (as well as realtime sound output), so you can view your results using one of the numerous sound/signal analysis tools already available over the WWW (e.g. Snd, Cool Edit, Matlab).  For those instances where a simple GUI with sliders and buttons is helpful, we use Tcl/Tk (which is freely distributed for all the STK supported platforms).  A number of Tcl/Tk GUI scripts are distributed with the STK release.


SYSTEM REQUIREMENTS:

See the individual README's (eg. README-linux) for platform specific information and system requirements.  In general, you will use either the provided Makefiles (Unix platforms) or the VC++ workspace files to compile the example programs.  To use the Tcl/Tk GUIs, you will need Tcl/Tk version 8.0 or higher.


WHAT'S NEW:

STK has undergone several key revisions, changes, and additions since its last release.  Despite being available in one form or another since 1996, we still consider STK to be alpha software.  Thus, backward compatability has not been a priority.  Please read the ReleaseNotes to see what has changed since the last release.

The control message handling scheme has been simplified greatly with release 3.2 through the use of the Controller class.  It is now possible to have access to simultaneous piped, socketed, and/or MIDI input control messages.  In most cases, this should eliminate the use of the MD2SKINI program.

Realtime audio input capabilities were added to STK with release 3.0, though the behavior of such is very hardware dependent.  Under Linux and Irix, audio input and output are possible with very low latency.  Using the Windoze DirectSound API, minimum dependable output sound latency seems to be around 20 milliseconds or so, while input sound latency is on the order of a hundred milliseconds or more!

As mentioned above, it is possible to record the audio ouput of an STK program to .snd, .wav, .raw, .aif, and .mat (Matlab MAT-file) output file types.  Though somewhat obsolete, the program MD2SKINI can be used to write SKINI scorefiles from realtime MIDI input.  Finally, STK should compile with non-realtime functionality on any platform with a generic C++ compiler.

For those who wish to make a library from the core STK classes, there is a Makefile in the src directory that will accomplish that (Linux and SGI only).

GETTING STARTED:

A number of example "projects" are provided with this distribution.  The effects directory contains a program that demonstrates realtime duplex mode (simultaneous audio input and output) operation, as well as several simple delay-line based effects algorithms.  RagaMatic is a totally cool application for achieving inner peace.  The examples directory contains several simple programs which demonstrate audio input/output, as well as the use of the audio internet streaming classes.  The syntmono directory offers a program for monophonic STK instrument playback and manipulation.  Syntmono is used to demonstrate most of the current STK instruments.  Control data (in the form of MIDI or SKINI messages) is acquired by syntmono through pipe, socket, or MIDI connections.  Tcl/Tk GUIs are provided which output SKINI formatted messages.  A variety of SKINI scorefiles are distributed with STK and can be found in the "scores" directory of the syntmono project.  MD2SKINI is an executable (currently compiles from the syntmono project) which takes raw MIDI input, converts it to SKINI format, and outputs the result to stdout or any socket host and port ID.

Unless you downloaded the distribution with precompiled Windoze binaries, it is necessary to first compile the sources.  Under Linux or Irix, simply typing "make" in any of the particular project directories will begin the compilation process.  If your Unix system does not have the GNU Makefile utilities, you will have to use one of the platform specific Makefiles (eg. make -f Makefile.sgi).  To compile the projects under Windoze, you should use the VC++ 6.0 project files provided with the STK distribution.


SYNTMONO:

Syntmono is used to demonstrate most of the current STK instruments.  Syntmono can take realtime control input via MIDI and/or SKINI format via pipes or sockets, or it can be fed SKINI scorefile (non-realtime) input.  Syntmono can output data in realtime, .wav, .snd, .aif, .mat (Matlab MAT-file), and/or .raw formats.  Assuming you have successfully compiled the syntmono executable, a scorefile can be redirected to syntmono and the output heard in realtime in the following way:

    syntmono Clarinet -or < scores/streetsf.ski

The "-or" flag specifies the realtime output option.  Typing syntmono without arguments will provide a brief description of the instruments possible and the various input/output option flags.  Tcl/Tk GUIs are provided in the "tcl" directory of each project, though you will have to install Tcl/Tk version 8.0 or higher on your system to use them (older versions of Tcl/Tk under Linux seem to be more forgiving than under IRIX).  Realtime SKINI control data can be piped to syntmono from a Tcl/Tk GUI on Unix platforms and WinNT in the following way:

    wish < TCLSpecs/TCLPhys.tcl | syntmono Clarinet -or -ip

The "-ip" flag specifies piped realtime input.  It is not possible to use realtime pipes under Windoze95/98, so socket communication must be used instead.    For socket communication, it is necessary to first start the syntmono socket server using the "-is" flag (socketed realtime input).  For the time being, a default (hardwired) socket port of 2001 is being used by syntmono.  After syntmono is running (and waiting for a socket client connection), a Tcl/Tk GUI can be started.  When using the GUI, it is necessary to invoke the "communications" menu item and select "socket" to establish the connection.  The same procedure is also possible on Unix platforms.  Finally, realtime MIDI control input can be used to control syntmono by typing:

    syntmono Clarinet -or -im

The "-im" flag specifies realtime MIDI input.  It is possible to use piped, socketed, and/or MIDI control input simultaneously.


DISCLAIMER:

You probably already guessed this, but just to be sure, we don't guarantee anything works.  :-)  It's free ... what do you expect?  If you find a bug, please let us know and we'll try to correct it.  You can also make suggestions, but again, no guarantees.  Send email to prc@cs.princeton.edu and gary@ccrma.stanford.edu. 


LEGAL AND ETHICAL:

This software was designed and created to be made publicly available for free, primarily for academic purposes, so if you use it, pass it on with this documentation, and for free.  

If you make a million dollars with it, give us some.  If you make compositions with it, put us in the program notes.

Some of the concepts are covered by various patents, some known to us and likely others which are unknown.  Many of the ones known to us are administered by the Stanford Office of Technology and Licensing.  

The good news is that large hunks of the techniques used here are public domain.  To avoid subtle legal issues, we'll not state what's freely useable here, but we'll try to note within the various classes where certain things are likely to be protected by patents.


FURTHER READING:

For more documentation on this ToolKit, the classes, etc., read the file Hierarchy.txt and the individual class definitions.  Also check the platform specific README's for specific system requirements.


PERRY'S NOTES FROM THE ORIGINAL DISTRIBUTION:

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

   This set of C++ unit generators and instruments
   might help to diminish the scores of emails I
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




