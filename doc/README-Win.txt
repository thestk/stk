The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995-2002.

Please read the file README for more general STK information.

DirectX and WindowsNT Issues:
-----------------------------

STK is currently distributed with Visual C++ 6.0 project and workspace files.  It has been tested using both Visual C++ 6.0 and Visual C++ .NET.

IMPORTANT VC++ NOTE: When compiling "release" versions of STK programs, link to the release multithreaded library.  When compiling "debug" versions, link to the debug multithreaded library.  Compiler errors will result otherwise.  Also, the Microsoft folk are up to their old habits of trying to change standards.  The .NET compiler will complain about cerr for some reason.

The STK realtime sound input capabilities under Windoze are only supported using the DirectSoundCapture API.  The latency is pretty horrendous, but what do you expect?  Also, there is a chance you don't have DirectSoundCapture support on your computer.  If not, you should download the DirectX 6.0 (or higher) runtime libraries from Microsoft's WWW site (http://www.microsoft.com/directx/download.asp) in order to run the pre-compiled STK executables for Windoze.  The last time I checked, there was no DirectSoundCapture support for WindowsNT ... you'll have to switch to Windows 2000 or XP.  I stopped supporting the WinMM audio output code with release 3.2.

Realtime sound output under Windoze is supported using the DirectSound (dsound.lib) API. All new versions of WindowsXX come with the DirectSound library, but early versions did not.  If you have trouble running the distributed executables, then you probably don't have DirectSound installed on your system.  You can download the necessary DirectSound stuff from Microsoft's WWW pages (http://www.microsoft.com/directx/download.asp).

Realtime MIDI input is supported using the winmm.lib API.

Visual C++ 6.0 workspaces have been created for the various STK projects.  Everything has already been configured for you.  The intermediate .obj files will be written to either the "release" or "debug" directories, but the executable files will be written to the main project directories (where they need to be for proper execution).  If you should somehow lose or hose the VC++ workspace file for a project, then you will have to do a LOT of configuring to recreate it ... it's probably easier just to download the distribution again from our WWW sites.  Anyway, for your benefit and mine, here is a list of things that need to be added to the various "Project Settings":

1. Under General: Set "Output files:" to <blank> (this will put the executable in the main project directory.

2. Under C/C++ > Code Generation: Set "Use run-time library:" to Multithreaded (use "debug" versions for the debug configuration).

3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.

4. Under C/C++ > Preprocessor: Add "../../include" directory to the "extra include" field.

5. Under C/C++ > Preprocessor: Add "__WINDOWS_DS__" to the definitions field.

6. Add all the necessary files to the project.

Remember that items 1-5 above need to be done for each project and for each configuration.  There might be an easy way to make global changes, but I couldn't figure it out.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk.  I got version 8.0 and it works very well (and installed easily).  The distribution is available on the WWW and is free.

In order for socketing to work, it is necessary to have the TCP protocol installed on your computer.  This can be done from the "Network" control panel.

Finally, to use it all -


PLAY SKINI SCOREFILES IN REALTIME:

	demo Clarinet -or < scores/streetsf.ski


USE TCL/TK GUIs FOR REALTIME CONTROL:

1. Open a DOS console window and start syntmono (eg. demo Clarinet -or -is).

2. Double click on a Tcl/Tk file in TCLSpecs (eg. TCLPhys.tcl) from the Windows Explorer to start the GUI.  Select the "communications" menu item and "Socket" and make the connection.

3. Start moving the sliders to control the instrument.


USE REALTIME MIDI INPUT FOR CONTROL:

1. Open a DOS console window and start syntmono with MIDI input (eg. demo Clarinet -or -im).

   This assumes you already have MIDI setup correctly for your computer.


WINDOWS 2000/XP:

There is a big advantage in using Windows 2000/XP over 95/98 with STK in that piping works, just as under unix.  Also, the scheduler in 2000/XP seems to be much better, so socketed messages don't get clumped together like they do in Windows 95/98.  The script files (ex. Demo) can be renamed with .bat extensions, allowing them to work in the same way as in unix systems.


WINDOWS NT ONLY:

Realtime piping seems to work under WindowsNT in much the same way as on Unix platforms.  Thus, it is possible to pipe realtime control data to syntmono under WindowsNT as well.


