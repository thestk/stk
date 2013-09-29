The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2002.

Please read the file README and INSTALL for more general STK information.

Realtime support for Macintosh OS X uses the CoreAudio HAL API and is specified during compilation using the __MACOSX_CORE__ preprocessor definition.

It is necessary to download the OS X developer kit in order to compile STK.  STK was successfully tested on OS X versions 10.1 and 10.2.

The internal Macintosh audio hardware typically supports a sample rate of 44100 Hz only.  Therefore, it is necessary to either specify this rate as a command-line option to the STK example programs or to change the default sample rate inside the Stk.h file before compilation.  In addition, the RT_BUFFER_SIZE, specified in Stk.h, could be increased (to a higher power of two) for more robust performance.

There is a potential conflict between the STK Delay class and a Delay() function declared in OSUtils.h (which is included via <CoreServices/CoreServices.h>).  In general, this conflict can be avoided via the use of a namespace (an explicit Delay::Delay declaration), though this made the Windows Visual C++ compiler barf.  If you use STK classes within a project that includes the OSUtils.h file, you will likely need to make changes in STK classes that use the Delay class.


Tcl/Tk on OS X:

The tcl/tk interpreter does not ship by default with OS X, but must be downloaded from the internet.  Binary distributions exist but it is instead recommended that you download recent tcl and tk source distributions (http://dev.scriptics.com/software/tcltk/downloadnow84.tml) and compile them as follows:

  make -C tcl/macosx deploy
  make -C tk/macosx deploy
  sudo make -C tcl/macosx install-deploy
  sudo make -C tk/macosx  install-deploy

(Note: the tcl and tk directories specified in the above lines will more likely be appended with version numbers)  The default installation will place a link to the wish interpretor at /usr/bin/wish.  The latest 8.4.1 release of tcl/tk has been tested on a 10.2 system and found to work correctly.  In particular, redirection of a tcl/tk script to the interpreter (e.g., wish < test.tcl) works normally (which is not the case with binary distributions tested thus far).

Initial tests have shown somewhat poor response between changes made in the tcl/tk script and the resulting audio updates.

Also, it is not recommended to connect by socket from a tcl/tk script to an STK program because the tcl/tk interpreter does not appear to properly close the socket connection, leaving the STK program in a "hung" state.
