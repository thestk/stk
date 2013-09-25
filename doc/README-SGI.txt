The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2002.

Please read the file README for more general STK information.

When using the Makefiles provided, first try invoking "gmake" (for GNU make).  If that doesn't work, try to download the GNU Makefile utilities from the Internet and use STK's default Makefile.  If this is not possible, try using Makefile.sgi (make -f Makefile.sgi).

Another issue that has crept up with this release is proper compiler support for C++ error handling.  If you experience problems, you probably don't have a recent version of the C++ compiler.  Otherwise, STK should compile and run on SGI platforms without any problems.  Release 4.0 of STK is confirmed to compile without difficulty using CC version 7.30.

NOTE REGARDING PTHREADS:

Since release 3.1, STK has used the pthread API under Irix.  It appears that pthread functionality is standard on SGI, so this change shouldn't cause any problems.  If I'm wrong, let me know!
