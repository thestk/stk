/******************************************/
/*  Simple MIDI Text File Reader Class,   */
/*  by Perry R. Cook, 1996                */ 
/*  This Object can open a MIDI Text File */
/*  and parse it.  The file spec is mine  */
/*  and mine alone, but it's all text so  */
/*  you should be able to figure it out.  */        
/*					  */
/*  TSIDI (ToolKit Synthesis Instrument   */
/*  Digital Interfaceis like MIDI, but    */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn(1,60.01,111.132) plays a sharp */      
/*  middle C with a velocity of 111.132   */
/*                                        */
/*  Warning:  Obey column spacing in the  */
/*  text file if you try to edit it or    */
/*  create your own files.                */
/******************************************/

#if !defined(__MIDIInpt_h)
#define __MIDIInpt_h

#include "Object.h"
#include "dmedia/midi.h" 
#include <sys/types.h>
#include <sys/prctl.h>

class MIDIInpt : public Object
{
  protected:  
    int midi_pid;
    int messageType;
    int channel;
    float byteTwo;
    float byteThree;
  public:
    MIDIInpt();
    ~MIDIInpt();
    void printMessage();
    int nextMessage();
    int getType();
    int getChannel();
    MY_FLOAT getByteTwo();
    MY_FLOAT getByteThree();
};

#endif
