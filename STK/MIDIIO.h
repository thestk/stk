/******************************************/
/*  MIDIIO.h                              */
/*  Realtime MIDI I/O Object for STK,     */
/*  by Gary P. Scavone, 1998.             */
/*  Based in part on code by Perry        */
/*  Cook (SGI), Paul Leonard (Linux),     */
/*  the RoseGarden team (Linux), and      */
/*  Bill Putnam (Win95/NT).               */
/*                                        */
/*  At the moment, this object only       */
/*  handles MIDI Input, though MIDI       */
/*  Output code can go here when someone  */
/*  decides they need it (and writes it). */
/*                                        */
/*  This object opens a MIDI Input device */
/*  and parses MIDI data.  Time stamp     */
/*  info is converted to deltaTime.       */
/*  MIDI data is stored as MY_FLOAT to    */
/*  conform with SKINI.                   */
/******************************************/

#if !defined(__MIDIIO_h)
#define __MIDIIO_h

#include "Object.h"

class MIDIIO : public Object
{
  protected:  
    int messageType;
    int channel;
    float byteTwo;
    float byteThree;
	MY_FLOAT deltaTime;
  public:
    MIDIIO();
    ~MIDIIO();
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
