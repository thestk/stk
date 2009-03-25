TK96CPP
A ToolKit of Audio Synthesis Classes 
        and Instruments in C++
Perry Cook, 1995-96 

Please read the Legal and Ethical notes at the
bottom of this document.

For instant fun, if you get it to compile (see
below) type TestAllNext or TestAllSGIRT.  The
former makes sound files of each instrument and
saves them under the instrument name.  The latter
plays in real time from a SKINI scorefile.

For even more potential fun, try the GUI and MIDI
interface demos in the directory TCLSpecs.  MIDI
is SGI specific for today, look for more support
later.  TCL works on SGI, maybe elsewhere, but
you must have and install Tcl/TK.

For more documentation on this ToolKit, the classes,
etc, read the file HIERARCH.txt and the individual 
class definitions.

SGI vs. NeXT vs. Intel vs. the world:
See Object.h and Makefile for machine-specific
items.

Initial public release.  Some objects still beta.

This whole world was created with no particular
hardware in mind.  These examples are intended 
to be tutorial in nature, as a platform for the
continuation of my research, and as a possible 
starting point for a software synthesis system.
The basic motivation was to create the necessary
unit generators to do the synthesis, processing, 
and control that I want to do and teach about.
Little thought for optimization was given (see
Object.cpp), and therefore improvements, especially 
speed enhancements, should be possible with 
these classes.  It was written with some basic 
concepts in mind about how to let compilers 
optimize (see Adrian Freed's home page for some 
nice experience-based thoughts on that topic).

Your question at this point might be, "But Perry,
with CMix, CMusic, CSound, CShells, CMonkeys, etc.
already cluttering the landscape, why a new set
of stupid C functions for music synthesis and
processing?"  The answers lie below.

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

***********************************************************  
Legal and Ethical:

This software was designed and created to be 
made publicly available for free, primarily for
academic purposes, so if you use it, pass it on 
with this documentation, and for free.  

If you make a million dollars with it, give me some.
If you make compositions with it, put me in the
program notes.

Some of the concepts are covered by various patents, 
some known to me and likely others which are unknown.  
Many of the ones known to me are administered by the 
Stanford Office of Technology and Licensing.  

The good news is that large hunks of the techniques
used here are public domain.  To avoid subtle legal
issues, I'll not state what's freely useable here, 
but I'll try to note within the various classes 
where certain things are likely to be protected by
patents.
***********************************************************
