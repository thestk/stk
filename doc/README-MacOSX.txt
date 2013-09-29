The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2002.

Please read the file README and INSTALL for more general STK information.

Realtime support for Macintosh OS X uses the CoreAudio HAL API and is specified during compilation using the __MACOSX_CORE__ preprocessor definition.

It is necessary to download the OS X developer kit in order to compile STK.  STK was successfully tested on OS X version 10.1.

The internal Macintosh audio hardware typically supports a sample rate of 44100 Hz only.  Therefore, it is necessary to either specify this rate as a command-line option to the STK example programs or to change the default sample rate inside the Stk.h file before compilation.

There is a potential conflict between the STK Delay class and a Delay() function declared in OSUtils.h (which is included via <CoreServices/CoreServices.h>).  In general, this conflict can be avoided via the use of a namespace (an explicit Delay::Delay declaration), though this made the Windows Visual C++ compiler barf.  If you use STK classes within a project that includes the OSUtils.h file, you will likely need to make changes in STK classes that use the Delay class.


Tcl/Tk on OS X:

The tcl/tk interpreter does not ship by default with OS X, but must be downloaded from the internet.  Once installed, it is suggested that a link be made to the executable from a standard search path (/usr/bin/wish).  In any event, it will not be possible to use the supplied startup scripts in the demo directory (i.e. StkDemo.bat) because the tcl/tk script startup sequence is slightly different than for all the other systems.  Assuming you have made the link mentioned above, the STK demo program and tcl/tk script can be started by typing:

  wish tcl/Demo.tcl | demo Clarinet -or -ip

Initial tests have shown somewhat poor response between changes made in the tcl/tk script and the resulting audio updates.

At this point, it is not recommended to connect by socket from a tcl/tk script to an STK program because tcl/tk does not appear to properly close the socket connection and the STK program is left hanging.
