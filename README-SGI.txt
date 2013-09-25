STK98v2: A ToolKit of Audio Synthesis Classes and Instruments in C++

By Perry R. Cook, 1995-98
With recent help by Gary P. Scavone

Please read the file README.txt for more general STK information.

STK98 should compile without much trouble, after you make the appropriate settings in Object.h and select an appropriate Makefile.  Apparently, SGI systems are not distributed with the GNU makefile utilities, so your safest bet is to use Makefile.sgi (which should be renamed "Makefile" before using).  If you do have the GNU makefile utilities on your system, you could also use Makefile.all (which is convenient if you are on a network with a variety of different computer systems).  Typing "make" should initiate the compilation process.

Once everything is compiled, you can use the scripts in "TCLSpecs" to run GUIs or invoke direct MIDI input controls.

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

