The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2007.

Please read the file README and INSTALL for more general STK information.

The project Makefiles are created by configure.  If you have trouble running "make", try invoking "gmake" (for GNU make).  If that doesn't work, try to download the GNU Makefile utilities from the Internet.

With STK version 4.3, realtime support for IRIX has been discontinued due to the inability to test it.  If you need realtime support on an SGI, go back to version 4.2.1.  Release 4.0 of STK is confirmed to compile (with various warnings) using CC version 7.30.

The __IRIX_AL__ and __IRIX_MD__ preprocessor definitions are required for realtime audio and MIDI support.

NOTE REGARDING PTHREADS:

Since release 3.1, STK has used the pthread API under Irix.  It appears that pthread functionality is standard on SGI, so this change shouldn't cause any problems.  If I'm wrong, let me know!
