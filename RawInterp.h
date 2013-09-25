/*******************************************/
/*  Interpolating RawWave Class,           */
/*  by Perry R. Cook, 1995-96              */ 
/*  This object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once or looping, with linear  */
/*  interpolation on playback.             */
/*                                         */
/*  Made inherited from RawWave            */
/*  by Gary P. Scavone (11/11/98)          */
/*******************************************/

#if !defined(__RawInterp_h)
#define __RawInterp_h

#include "Object.h"
#include "RawWave.h"

class RawInterp : public RawWave
{
  protected:  
    int looping;
    int myData;
    int finished;
    long length;
    MY_FLOAT *data;
    MY_FLOAT rate;
    MY_FLOAT time;
    MY_FLOAT phaseOffset;
    MY_FLOAT lastOutput;
  public:
    RawInterp(char *fileName);
    RawInterp(MY_FLOAT *someData,long aLength);   
    ~RawInterp();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    void setFreq(MY_FLOAT aFreq);    
    void addTime(MY_FLOAT aTime);
    void addPhase(MY_FLOAT anAngle);
    void addPhaseOffset(MY_FLOAT anAngle);
    void setLooping(int aLoopStatus);
    int isFinished();
    long getLength();
    MY_FLOAT* getData();
    MY_FLOAT tick();
    int informTick();
    MY_FLOAT lastOut();
};

#endif
