/******************************************/
/*
  RtMidi.cpp
  Realtime MIDI I/O Object for STK,
  by Gary P. Scavone, 1998-2000.
  Based in part on code by Perry
  Cook (SGI), Paul Leonard (Linux),
  the RoseGarden team (Linux), and
  Bill Putnam (Win95/NT).

  At the moment, this object only
  handles MIDI input, though MIDI
  output code can go here when someone
  decides they need it (and writes it).

  This object opens a MIDI input device
  and parses MIDI messages into a MIDI
  buffer.  Time stamp info is converted
  to deltaTime. MIDI data is stored as
  MY_FLOAT to conform with SKINI.

  An optional argument to the constructor
  can be used to specify a device or card.
  When no argument is given, a default
  device is opened or a list of available
  devices is printed to allow selection
  by the user.
*/
/******************************************/

#if !defined(__RtMidi_h)
#define __RtMidi_h

#include "Object.h"
#include "StkError.h"

class RtMidi : public Object
{
 protected:  
  int messageType;
  int channel;
  float byteTwo;
  float byteThree;
	MY_FLOAT deltaTime;
 public:
  RtMidi(int device = -1);
  ~RtMidi();
  void printMessage();
  int nextMessage();
  int getType();
  int getChannel();
  MY_FLOAT getByteTwo();
  MY_FLOAT getByteThree();
	MY_FLOAT getDeltaTime();
};

#if defined(__OS_Win_)

#include <windows.h>
#include <mmsystem.h>

static void CALLBACK midiInputCallback( HMIDIOUT hmin, UINT inputStatus, 
			DWORD instancePtr, DWORD midiMessage, DWORD timestamp);

#endif
#endif
