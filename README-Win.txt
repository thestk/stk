STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.1

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

DirectX and WindowsNT Issues:
-----------------------------

The newly offered STK realtime sound input capabilities under Windoze are only supported using the DirectSoundCapture API.  The latency is pretty horrendous, but what do you expect?  Also, there is a good chance you don't have DirectSoundCapture support on your computer.  You should download the DirectX 6.0 (or higher) runtime libraries from Microsoft's WWW site (http://www.microsoft.com/directx/download.asp) in order to run the pre-compiled STK executables for Windoze.  There is no DirectSoundCapture support for WindowsNT ... you'll have to switch to Windows 2000.  If you wish to compile STK under WindowsNT (without realtime audio input support), you'll have to uncomment the __WINMM_API_ flag (and comment out the __WINDS_API flag) in Object.h and recompile the source code.

Realtime sound output under Windoze is supported using either the DirectSound (dsound.lib) API or the old WinMM (winmm.lib) API.  The DirectSound version appears to well out-perform the older API.  All new versions of Win95/98/NT come with the DirectSound library, but early versions did not.  If you have trouble running the distributed executables (compiled for DirectSound API), then you probably don't have DirectSound installed on your system.  You can download the necessary DirectSound stuff from Microsoft's WWW pages (http://www.microsoft.com/directx/download.asp).  If all else fails, you should be able to compile using the winmm.lib routines ... more latency, but at least it will work.

Realtime MIDI input is supported using the winmm.lib API.

Visual C++ workspaces have been created for the various STK projects.  Everything has already been configured for you.  The intermediate .obj files will be written to either the "Release" or "Debug" directories, but the executable files will be written to the main project directories (where they need to be for proper execution).  If you should somehow lose or hose the VC++ workspace file (STK.dsw), then you will have to do a LOT of configuring to recreate it ... it's probably easier just to download the distribution again from our WWW sites.  Anyway, for your benefit and mine, here is a list of things that need to be added to the various "Project Settings":

1. Under General: Set "Output files:" to <blank> (this will put the executable in the main project directory.

2. Under C/C++ > Code Generation: Set "Use run-time library:" to Multithreaded.

3. Under Link > General:  Add winmm.lib, dsound.lib, and Wsock32.lib to the end of the Object/library modules list.

4. Add all the necessary files to the project.

NOTE: For some of my personal STK files, I've had trouble compiling in VC++ with the "Optimize Speed" optimization setting (in Project Settings -> C/C++ -> Optimizations:).  The files compile, but don't work the way they should when executed.  These same files, however, have worked when compiled with the "Default" optimization setting.  I've heard that there are bugs with some of these optimization settings.

Remember that items 1-3 above need to be done for each project and for each configuration.  There might be an easy way to make global changes, but I couldn't figure it out.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk.  I got version 8.0 and it works very well (and installed easily).  The distribution is available on the WWW and is free.

In order for socketing to work, it is necessary to have the TCP protocol installed on your computer.  This can be done from the "Network" control panel.

Finally, to use it all -


PLAY SKINI SCOREFILES IN REALTIME:

	syntmono Clarinet -r < scores/streetsf.ski


USE TCL/TK GUIs FOR REALTIME CONTROL:

1. Open a DOS console window and start syntmono (eg. syntmono Clarinet -r -is).

2. Double click on a Tcl/Tk file in TCLSpecs (eg. TCLPhys.tcl) from the Windows Explorer to start the GUI.  Select the "communications" menu item and "Socket" and make the connection.

3. Start moving the sliders to control the instrument.


USE REALTIME MIDI INPUT FOR CONTROL:

1. Open a DOS console window and start syntmono (eg. syntmono Clarinet -r -is).

2. Open another DOS console window and start MD2SKINI in the following way:

   MD2SKINI -s <optional hostname>

   This assumes you already have MIDI setup correctly for your computer.


WINDOWS NT ONLY:

Realtime piping seems to work under WindowsNT in much the same way as on Unix platforms.  Thus, it is possible to pipe realtime control data to syntmono under WindowsNT as well.


WINDOWS 2000:

I don't have Windows 2000 and I don't expect to get it anytime soon.  Things should work under 2000 at least as well as they do using NT.  Since 2000 is supposed to ship with DirectX 7.0, the DirectSoundCapture functionality should work as well.