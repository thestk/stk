STK: A ToolKit of Audio Synthesis Classes and Instruments in C++
Version 3.2

By Perry R. Cook, 1995-2000
and Gary P. Scavone, 1997-2000.

Please read the file README.txt for more general STK information.

STK has always worked under NeXTStep without realtime audio or MIDI support.  In general, STK should compile in this way using any generic C++ compiler.  C++ exception handling was added to STK with release 3.2.  I have had some difficulty testing this release under NeXTStep because our NeXTStep compilers at CCRMA are very old.  We tried a newer version of gcc-2.7.2.2 and that mostly worked, though it died trying to compile the BowedBar class.  Also, I was unable to locate the correct header for the random() function.

In summary, I _think_ STK will compile under NeXTStep with a fairly recent compiler, but you may have to do a little work to make it happen.  If you do succeed, please let us know.

Just for clarification, "realtime" support and the use of the __STK_REALTIME_ define statement includes audio and MIDI input/output routines, as well as socket and thread routines for realtime message acquisition (Controller) and internet audio streaming (StrmWvIn, StrmWvOut).

