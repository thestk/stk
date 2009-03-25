The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2004.

Please read the file README and INSTALL for more general STK information.

Realtime support for Macintosh OS X uses the CoreAudio HAL API and is specified during compilation using the __MACOSX_CORE__ preprocessor definition.

It is necessary to download the OS X developer kit in order to compile STK.  STK was successfully tested on OS X version 10.2.

The internal Macintosh audio hardware typically supports a sample rate of 44100 Hz only.  Therefore, it is necessary to either specify this rate as a command-line option to the STK example programs or to change the default sample rate inside the Stk.h file before compilation.  In addition, the RT_BUFFER_SIZE, specified in Stk.h, could be increased (to a higher power of two) for more robust performance.

There is a potential conflict between the STK Delay class and a Delay() function declared in OSUtils.h (which is included via <CoreServices/CoreServices.h>).  In general, this conflict can be avoided via the use of a namespace (an explicit Delay::Delay declaration), though this made the Windows Visual C++ compiler barf.  If you use STK classes within a project that includes the OSUtils.h file, you will likely need to make changes in STK classes that use the Delay class.


Tcl/Tk on OS X:

The tcl/tk interpreter does not ship by default with OS X, but must be downloaded from the internet.  The latest Tcl/Tk Aqua distribution (http://www.apple.com/downloads/macosx/unix_open_source/tcltk.html) has been successfully tested on a 10.2 system.  The default installation will place a link to the wish interpretor at /usr/bin/wish.

Initial tests have shown somewhat poor response between changes made in the tcl/tk script and the resulting audio updates.

It is possible to connect a tcl/tk interface to an STK program via a socket connection.  However, the tcl/tk interpreter does not appear to properly close the socket connection during disconnection.  It is therefore necessary to type "Exit" in the STK program terminal window to properly exit the STK program.
