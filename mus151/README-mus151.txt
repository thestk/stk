MUS151: An example project using STK

By Gary P. Scavone
CCRMA, Stanford University, 1998.

It is highly recommended that you keep your personal STK projects separate from the STK core distribution, to make upgrading to newer releases more simple.  The files in this directory demonstrate how one might manage this.

This simple project, named MUS151 for the pyschoacoustics class taught at CCRMA, plays two sine waves and allows independent control of their amplitudes and frequencies.  The file MUS151.cpp is essentially the same as syntmono.cpp, though simplified to a great extent because of its more specific use.  In particular, only one "instrument" is currently supported (TwoOsc) and only real-time output is used (thus, this project cannot be compiled for NeXTStep).  A simple Tcl/Tk GUI has been created to control the instrument.  The GUI and STK project can be run under Linux and SGI using the simple GUITwoOsc executable script.

The mus151 project directory has been distributed as a subdirectory of the core STK file directory.  In this location, the project should compile without any problems (on Linux, SGI, and Win platforms).  Should you choose to relocate this directory, several path variables will need to be updated as described below.

Three "personal" STK files were created for this project: MUS151.cpp and the TwoOsc class files TwoOsc.h and TwoOsc.cpp.  A specific Tcl/Tk GUI script was written (TwoOsc.tcl) and is located in the "tcl" subdirectory.  Depending on where you choose to locate the mus151 directory, several #include statements may need to be updated to reflect the location of the STK core distribution.  For example, mus151/ is currently a subdirectory of the STK core files, so the various core STK header files are referenced in TwoOsc.h as: #include "../Instrmnt.h".  You could move the mus151 files to a completely different point on your directory tree, but you would have to update those statements to correctly reference your core files.  Likewise, the "sinewave.raw" file must be correctly referenced in TwoOsc.cpp.

The unix Makefile must also be updated to correctly reference the STK core distribution.  A variable, STK_PATH, has been created to simplify this procedure.  Note that all STK core object files are referenced with a single, general statement.  All personal object files need their own dependency definitions.

Finally, it is simple to create a VC++ project directory that is completely separate from the STK core distribution.  However, it appears that there is no simple way to redefine the location of the project files should you choose to relocate the project after it is created.  You will probably have to manually "Add" all the core STK files to the VC++ project after moving the project directory.