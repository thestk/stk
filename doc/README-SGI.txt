STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.2

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

It seems that SGI systems are not distributed with the GNU Makefile utilities.  The default Make utility has very limited functionality, so your safest bet is to download the GNU Makefile utilities from the Internet and use STK's default Makefile.  If this is not possible, try using Makefile.sgi (make -f Makefile.sgi).

Another issue that has crept up with this release is proper compiler support for C++ error handling.  If you experience problems, you probably don't have a recent version of the C++ compiler.  Otherwise, STK should compile and run on SGI platforms without any problems.

NOTE REGARDING PTHREADS:

Since release 3.1, STK has used the pthread API under Irix.  It appears that pthread functionality is standard on SGI, so this change shouldn't cause any problems.  If I'm wrong, let me know!
