The Synthesis ToolKit in C++ (STK)

By Perry R. Cook and Gary P. Scavone, 1995--2021.

Please read the file README.md for more general STK information.

The configure script supports MinGW.

STK has been built and tested on Windows platforms using Visual Studio.  It is assumed here that you're familiar with Visual C++ and its particular idiosyncrasies. The currently supported version is VS2017. You can download the free non-commercial community edition from the Microsoft website. The folders in the projects directory contain VS2017 solution files.
If you are creating a new stk application, it's easiest to use the supplied template:
- Copy stk\projects\stk-template.zip to C:\Users\<user>\Documents\Visual Studio 2017\Templates\ProjectTemplates\Visual C++ Project\
- Start VS2017.
- Select create new project...
- Select Visual C++.
- Select stk-template and enter your preferred project name and location. Note that if you do not put the project at the same level as stk\projects you will have to fix all paths in the project properties to match.
- The template is based on one of the projects in the examples directory. Add/remove files as needed and edit main.cpp to taste.

To use the Tcl/Tk GUIs, you will have to install Tcl/Tk and build using MinGW.

With Windows XP and later, piping works as under unix.  Simply fire up the script files (e.g. StkDemo.bat) by either double-clicking on them or from within a shell.

The DirectSound, WASAPI and Steinberg ASIO audio APIs are supported for realtime audio input/output.  The VS2017 project files included with this distribution are configured to use all supported APIs.  The default (as in stk-template) is the DirectSound API (preprocessor definition __WINDOWS_DS__). In order to use the ASIO API, it is necessary to use the preprocessor definition __WINDOWS_ASIO__, as well as include most of the files in the /src/include/ directory (i.e. asio.h, asio.cpp, ...).  If you have a good quality soundcard and a native ASIO driver (not emulated), you are likely to get much better input/output response using that.

When using the DirectSound API for audio input, latency can be high.  If you experience realtime audio "stuttering", you should experiment with different "buffer size" and "number of buffers" values.

Realtime MIDI input/output is supported by RtMidi using the winmm.lib API and requires the __WINDOWS_MM__ preprocessor definition.
