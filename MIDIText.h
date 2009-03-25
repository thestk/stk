/******************************************/
/*  Simple MIDI Text File Reader Class,   */
/*  by Perry R. Cook, 1996                */ 
/*  This Object can open a MIDI Text File */
/*  and parse it.  The file spec is mine  */
/*  and mine alone, but it's all text so  */
/*  you should be able to figure it out.  */        
/*					  */
/*  SKINI (Synthesis toolKit Instrument   */
/*  Network Interface) is like MIDI, but   */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn 60.01 111.132 plays a sharp    */      
/*  middle C with a velocity of 111.132   */
/*  See SKINI.txt for more information    */
/*  Warning:  Obey columns in the text    */
/*  file if you try to edit it or create  */
/*  your own files.                       */
/******************************************/

#if !defined(__MIDIText_h)
#define __MIDIText_h

#include "Object.h"

class MIDIText : public Object
{
  protected:  
    FILE *myFile;
    int messageType;
    int channel;
    MY_FLOAT deltaTime;
    float byteTwo;
    float byteThree;
  public:
    MIDIText(char *fileName);
    ~MIDIText();
    int nextMessage();
    int getType();
    int getChannel();
    MY_FLOAT getDelta();
    MY_FLOAT getByteTwo();
    MY_FLOAT getByteThree();
};

#endif
