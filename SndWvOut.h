/*******************************************/
/*  NeXT Sound File Output Class,          */
/*  by Perry R. Cook, 1995-96              */ 
/*  For now, This Object can open a raw    */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

/*******************************************/
/*  NeXT Soundfile Output Class, Improved  */
/*  by Perry R. Cook, 1996                 */ 
/*  This one opens a NeXT .snd file, and   */
/*  even knows how to byte-swap!           */
/*******************************************/

#include "Object.h"
#include "WvOut.h"

#if !defined(__SndWvOut_h)
#define __SndWvOut_h

#define SND_BUFFER_SIZE 1024

class SndWvOut : public WvOut
{
  protected:  
    FILE *fd;
    short data[SND_BUFFER_SIZE];
    long counter;
    long totalCount;
    int chans;
    MY_FLOAT pan;
  public:
    SndWvOut(char *fileName);
    SndWvOut(int channels, char *fileName);
    ~SndWvOut();
    long getCounter();
    MY_FLOAT getTime();
    void setMonoPan(MY_FLOAT aPan);
    void tick(MY_FLOAT sample);
    void tick(MY_FLOAT lsamp, MY_FLOAT rsamp);
};

#endif // defined(__SndWvOut_h)
