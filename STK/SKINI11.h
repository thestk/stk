/******************************************/
/*  3rd generation SKINI Text File Reader */
/*  Class, by Perry R. Cook, 1999         */ 
/*  This Object can open a SKINI File     */
/*  and parse it.  The file spec is mine  */
/*  and mine alone, but it's all text so  */
/*  that should help you figuring it out. */        
/*                                        */
/*  SKINI (Synthesis toolKit Instrument   */
/*  Network Interface) is like MIDI, but  */
/*  allows for floating point control     */
/*  changes, note numbers, etc. Example:  */
/*  noteOn 60.01 111.132 plays a sharp    */      
/*  middle C with a velocity of 111.132   */
/*  See SKINI.txt for more information    */
/*                                        */
/******************************************/

#if !defined(__SKINI11_h)
#define __SKINI11_h

#include "Object.h"

class SKINI11 : public Object
{
  protected:  
    FILE *myFile;
    int messageType;
    char msgTypeString[64];
    int channel;
    MY_FLOAT deltaTime;
    MY_FLOAT byteTwo;
    MY_FLOAT byteThree;
    long byteTwoInt;
    long byteThreeInt;
    char remainderString[1024];
  public:
    SKINI11(char *fileName);
    SKINI11();
    ~SKINI11();
    long parseThis(char* aString);
    long nextMessage();
    long getType();
    long getChannel();
    MY_FLOAT getDelta();
    MY_FLOAT getByteTwo();
    MY_FLOAT getByteThree();
    long     getByteTwoInt();
    long     getByteThreeInt();
    char*    getRemainderString();
    char*    getMessageTypeString();
    char*    whatsThisType(long type);
    char*    whatsThisController(long type);
};

#endif


