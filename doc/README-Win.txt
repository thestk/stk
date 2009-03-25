The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2004.

Please read the file README for more general STK information.

DirectX and WindowsNT Issues:
-----------------------------

STK is currently distributed with Visual C++ 6.0 project and workspace files.  It has been tested using both Visual C++ 6.0 and Visual C++ .NET.

IMPORTANT VC++ NOTE: When compiling "release" versions of STK programs, link to the release multithreaded library.  When compiling "debug" versions, link to the debug multithreaded library.  Compiler errors will result otherwise.  Also, the Microsoft folk are up to their old habits of trying to change standards.  The .NET compiler will complain about cerr for some reason.

Both the DirectSound and Steinberg ASIO audio APIs are supported for realtime audio input/output.  The Visual C++ project files included with this distribution are configured to use the DirectSound API.  In order to use the ASIO API, it is necessary to change the preprocessor definition from __WINDOWS_DS__ to __WINDOWS_ASIO__, as well as include all the files in the /src/asio/ directory (i.e. asio.h, asio.cpp, ...).  If you have a good quality soundcard and a native ASIO driver (not emulated), you are likely to get much better input/output response using that.

When using the DirectSound API for audio input, latency is typically pretty horrendous (should we be surprised?).  Also, there is a slight chance you don't have DirectSoundCapture support on your computer.  If not, you should download the DirectX 6.0 (or higher) runtime libraries from Microsoft's WWW site (http://www.microsoft.com/directx/download.asp) in order to run the pre-compiled STK executables for Windoze.  The last time I checked, there was no DirectSoundCapture support for WindowsNT ... you'll have to switch to Windows 2000 or XP or use an ASIO driver.  I stopped supporting the WinMM audio output code with release 3.2.

Realtime MIDI input/output is supported by RtMidi using the winmm.lib API and requires the __WINDOWS_MM__ preprocessor definition.

Visual C++ 6.0 workspaces have been created for the various STK projects.  Everything has already been configured for you.  The intermediate .obj files will be written to either the "Release" or "Debug" directories, but the executable files will be written to the main project directories (where they need to be for proper execution).  If you should somehow lose or hose the VC++ workspace file for a project, then you will have to do a LOT of configuring to recreate it ... it's probably easier just to download the distribution again from our WWW sites.  Anyway, for your benefit and mine, here is a list of things that need to be added to the various "Project Settings":

1. Under General: Set "Output files:" to <blank> (this will put the executable in the main project directory.

2. Under C/C++ > Code Generation: Set "Use run-time library:" to Multithreaded (use "debug" versions for the debug configuration).

3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.

4. Under C/C++ > Preprocessor: Add "../../include" directory to the "extra include" field.

5. Under C/C++ > Preprocessor: Add "__WINDOWS_DS__", "__WINDOWS_MM__", and "__LITTLE_ENDIAN__ to the definitions field.

6. Add all the necessary files to the project.

Remember that items 1-5 above need to be done for each project and for each configuration.  There might be an easy way to make global changes, but I couldn't figure it out.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk.  I got version 8.0 and it works very well (and installed easily).  The distribution is available on the WWW and is free.

In order for socketing to work, it is necessary to have the TCP protocol installed on your computer.  This can be done from the "Network" control panel.

Finally, to use it all -


WINDOWS XP/2000:

There is a big advantage in using Windows XP/2000 over 95/98 with STK in that piping works, just as under unix.  Also, the scheduler in 2000/XP seems to be much better, so socketed messages don't get clumped together like they do in Windows 95/98.  Simply fire up a script file (ex. StkDemo.bat) by either double-clicking on it or typing it within a shell.


WINDOWS 95/98:

PLAY SKINI SCOREFILES IN REALTIME:

	demo Clarinet -or -if scores/streetsf.ski

USE TCL/TK GUIs FOR REALTIME CONTROL:

1. Open a DOS console window and start syntmono (eg. demo Clarinet -or -is).

2. Double click on a Tcl/Tk file in the tcl/ subdirectory of the demo directory (eg. Demo.tcl) from the Windows Explorer to start the GUI.  Select the "communications" menu item and "Socket" and make the connection.

3. Start moving the sliders to control the instrument.


USE REALTIME MIDI INPUT FOR CONTROL:

1. Open a DOS console window and start syntmono with MIDI input (eg. demo Clarinet -or -im).

   This assumes you already have MIDI setup correctly for your computer.


WINDOWS NT ONLY:

Realtime piping seems to work under WindowsNT in much the same way as on Unix platforms.  Thus, it is possible to pipe realtime control data to syntmono under WindowsNT as well.  Note, however, that the DirectSoundCapture API does not exist for WindowsNT, so it is necessary to use an ASIO audio driver and the STK ASIO support.


