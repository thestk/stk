/*******************************************/
/*  Raw Soundfile Class,                   */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once or looping, with linear  */
/*  interpolation on playback.             */
/*******************************************/

#if !defined(__RawWave_h)
#define __RawWave_h

#include "Object.h"

class RawWave : public Object
{
  protected:  
    int looping;
    int myData;
    int allDone;
    long length;
    MY_FLOAT *data;
    MY_FLOAT rate;
    MY_FLOAT time;
    MY_FLOAT phaseOffset;
    MY_FLOAT lastOutput;
  public:
    RawWave(char *fileName);
    RawWave(MY_FLOAT *someData,long aLength);   
    ~RawWave();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    void setFreq(MY_FLOAT aFreq);    
    void addTime(MY_FLOAT aTime);
    void addPhase(MY_FLOAT anAngle);
    void addPhaseOffset(MY_FLOAT anAngle);
    void setLooping(int aLoopStatus);
    int isAllDone();
    long getLength();
    MY_FLOAT* getData();
    MY_FLOAT tick();
    int informTick();
    MY_FLOAT lastOut();
};

#endif
