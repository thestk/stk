The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995--2017.

Please read the file README for more general STK information.

The configure script supports MinGW.

Alternatively Visual Studio may be used. The demo project contains a VS2017 build file - demo.sln. Note that if
you are creating a brand new VS project it's probably easiest to copy those .sln and .vcxproj files and manually
edit them to taste.
The other projects in the projects directories contain VS6 build files which cannot be used directly with VS2017.
However if you do want to use VS2017 there is an update process. 
- Open the .dsw file with VS2017.
- Accept the one-way upgrade dialog. This will create new .sln and .vcxproj files.
- You will probably get a bad sdk version error. Right-click the solution, select Retarget solution and pick one.
- Open the project properties and select Debug configuration then C/C++ General tab. Set Debug Information Format
  to Program Database (/Zi).
- Optionally in project properties you can disable warning 4996.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk and build using MinGW.

With Windows XP and later, piping works as under unix.  Simply fire up the script files (e.g. StkDemo.bat) by either double-clicking on them or from within a shell.

The DirectSound, WASAPI and Steinberg ASIO audio APIs are supported for realtime audio input/output.  The Visual C++ project files included with this distribution are configured to use all supported APIs.  In order to use the ASIO API, it is necessary to use the preprocessor definition __WINDOWS_ASIO__, as well as include most of the files in the /src/include/ directory (i.e. asio.h, asio.cpp, ...).  If you have a good quality soundcard and a native ASIO driver (not emulated), you are likely to get much better input/output response using that.

When using the DirectSound API for audio input, latency can be high.  If you experience realtime audio "stuttering", you should experiment with different "buffer size" and "number of buffers" values.

Realtime MIDI input/output is supported by RtMidi using the winmm.lib API and requires the __WINDOWS_MM__ preprocessor definition.
