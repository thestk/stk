The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2004.

Please read the file README and INSTALL for more general STK information.

Realtime support for Macintosh OS X uses the CoreAudio HAL API and is specified during compilation using the __MACOSX_CORE__ preprocessor definition.

It is necessary to install the OS X developer kit in order to compile STK.  STK was successfully tested on OS X versions 10.2 and 10.3.

The internal Macintosh audio hardware typically supports a sample rate of 44100 Hz only.  The default STK sample rate is now 44100 Hz and all current example programs use this rate.  However, it is possible to manually override this value in some programs from the command-line.  The default sample rate is set in Stk.h.  In addition, the RT_BUFFER_SIZE, specified in Stk.h, could be increased (to a higher power of two) for more robust performance.

There is a potential conflict between the STK Delay class and a Delay() function declared in OSUtils.h (which is included via <CoreServices/CoreServices.h>).  In general, this conflict can be avoided via the use of a namespace (an explicit Delay::Delay declaration), though this made the Windows Visual C++ compiler barf.  If you use STK classes within a project that includes the OSUtils.h file, you will likely need to make changes in STK classes that use the Delay class.


Tcl/Tk on OS X:

The tcl/tk interpreter does not ship by default with OS X, but must be downloaded from the internet.  The latest Tcl/Tk Aqua distribution (http://www.apple.com/downloads/macosx/unix_open_source/tcltk.html) has been successfully tested on 10.2 and 10.3 systems.  The default installation will place a link to the wish interpretor at /usr/bin/wish.

It appears that socket support in Tcl/Tk on OS X uses the Nagle algorithm, which produces poor response between changes made in the tcl/tk script and the resulting audio updates.  Note that this is only a problem when using a socket connection from a Tcl/Tk script.

