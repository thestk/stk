STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.2

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

DirectX and WindowsNT Issues:
-----------------------------

STK is currently distributed with Visual C++ 6.0 project and workspace files.

The STK realtime sound input capabilities under Windoze are only supported using the DirectSoundCapture API.  The latency is pretty horrendous, but what do you expect?  Also, there is a chance you don't have DirectSoundCapture support on your computer.  If not, you should download the DirectX 6.0 (or higher) runtime libraries from Microsoft's WWW site (http://www.microsoft.com/directx/download.asp) in order to run the pre-compiled STK executables for Windoze.  The last time I checked, there was no DirectSoundCapture support for WindowsNT ... you'll have to switch to Windows 2000.  I stopped supporting the WinMM audio output code with this release.  So, if you wish to compile STK under WindowsNT (without realtime audio input support), you'll have to download an older version of STK, uncomment the __WINMM_API_ flag (and comment out the __WINDS_API flag) in Object.h and recompile the source code.

Realtime sound output under Windoze is supported using the DirectSound (dsound.lib) API. All new versions of Win95/98/NT come with the DirectSound library, but early versions did not.  If you have trouble running the distributed executables, then you probably don't have DirectSound installed on your system.  You can download the necessary DirectSound stuff from Microsoft's WWW pages (http://www.microsoft.com/directx/download.asp).

Realtime MIDI input is supported using the winmm.lib API.

Visual C++ 6.0 workspaces have been created for the various STK projects.  Everything has already been configured for you.  The intermediate .obj files will be written to either the "Release" or "Debug" directories, but the executable files will be written to the main project directories (where they need to be for proper execution).  If you should somehow lose or hose the VC++ workspace file (STK.dsw), then you will have to do a LOT of configuring to recreate it ... it's probably easier just to download the distribution again from our WWW sites.  Anyway, for your benefit and mine, here is a list of things that need to be added to the various "Project Settings":

1. Under General: Set "Output files:" to <blank> (this will put the executable in the main project directory.

2. Under C/C++ > Code Generation: Set "Use run-time library:" to Multithreaded (use "debug" versions for the debug configuration).

3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.

4. Under C/C++ > Preprocessor: Add "../../include" directory to the "extra include" field.

5. Add all the necessary files to the project.

Remember that items 1-3 above need to be done for each project and for each configuration.  There might be an easy way to make global changes, but I couldn't figure it out.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk.  I got version 8.0 and it works very well (and installed easily).  The distribution is available on the WWW and is free.

In order for socketing to work, it is necessary to have the TCP protocol installed on your computer.  This can be done from the "Network" control panel.

Finally, to use it all -


PLAY SKINI SCOREFILES IN REALTIME:

	syntmono Clarinet -or < scores/streetsf.ski


USE TCL/TK GUIs FOR REALTIME CONTROL:

1. Open a DOS console window and start syntmono (eg. syntmono Clarinet -or -is).

2. Double click on a Tcl/Tk file in TCLSpecs (eg. TCLPhys.tcl) from the Windows Explorer to start the GUI.  Select the "communications" menu item and "Socket" and make the connection.

3. Start moving the sliders to control the instrument.


USE REALTIME MIDI INPUT FOR CONTROL:

1. Open a DOS console window and start syntmono with MIDI input (eg. syntmono Clarinet -or -im).

   This assumes you already have MIDI setup correctly for your computer.


WINDOWS NT ONLY:

Realtime piping seems to work under WindowsNT in much the same way as on Unix platforms.  Thus, it is possible to pipe realtime control data to syntmono under WindowsNT as well.


WINDOWS 2000:

I don't have Windows 2000 and I doubt I'll get it anytime soon.  However, we briefly tested release 3.2 of STK on Perry's Win2000 machine and it worked fine.  There is an advantage in using Windows 2000 over 95/98 in that piping works, just as under unix.  Also, the scheduler in Win2000 seems to be much better, so socketed messages don't get clumped together like they do in Win 95/98.  Since 2000 is supposed to ship with DirectX 7.0, the DirectSoundCapture functionality should work as well.