STK98v2: A ToolKit of Audio Synthesis Classes and Instruments in C++

By Perry R. Cook, 1995-98
With recent help by Gary P. Scavone

Please read the file README.txt for more general STK information.

Realtime sound output under Win95 is supported using either the Direct Sound (dsound.lib) API or the old WinMM (winmm.lib) API.  The Direct Sound version appears to well out-perform the older API.  All new versions of Win95/NT come with the Direct Sound library, but early versions did not.  If you have trouble running the distributed executables (compiled for Direct Sound API), then you probably don't have Direct Sound installed on your system.  You can download the necessary Direct Sound stuff from Microsoft's WWW pages (http://www.microsoft.com/directx/download.asp).  If all else fails, you should be able to compile using the winmm.lib routines ... more latency, but at least it will work.

Realtime MIDI input is supported using the winmm.lib API.

A Visual C++ workspace has been created for STK98, with two projects - syntmono and MD2SKINI.  Everything has already been configured for you.  The intermediate .obj files will be written to either the "Release" or "Debug" directories, but the executable files will be written to the main STK98 directory (where they need to be for proper execution).  If you should somehow lose or hose the VC++ workspace file (STK98v2.dsw), then you will have to do a LOT of configuring to recreate it ... it's probably easier just to download the distribution again from our WWW sites.  Anyway, for your benefit and mine, here is a list of things that need to be added to the various "Project Settings":

1. Under General: Set "Output files:" to <blank> (this will put the executable in the main STK directory.

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

1. Open a DOS console window and start syntmono (eg. syntmono Clarinet -r -i).

2. Double click on a Tcl/Tk file in TCLSpecs (eg. TCLPhys.tcl) from the Windows Explorer to start the GUI.  Select the "communications" menu item and "Socket" and make the connection.

3. Start moving the sliders to control the instrument.

USE REALTIME MIDI INPUT FOR CONTROL:

1. Open a DOS console window and start syntmono (eg. syntmono Clarinet -r -i).

2. Open another DOS console window and start MD2SKINI (assumes you already have
   MIDI setup correctly for your computer).


/******************************************************/

Legal and Ethical:

This software was designed and created to be made publicly available for free, primarily for academic purposes, so if you use it, pass it on with this documentation, and for free.  

If you make a million dollars with it, give me some.  If you make compositions with it, put me in the program notes.

Some of the concepts are covered by various patents, some known to me and likely others which are unknown.  Many of the ones known to me are administered by the Stanford Office of Technology and Licensing.  

The good news is that large hunks of the techniques used here are public domain.  To avoid subtle legal issues, I'll not state what's freely useable here, but I'll try to note within the various classes where certain things are likely to be protected by patents.

/******************************************************/

