/*******************************************/
/*  Raw Looped Soundfile Class,            */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data, looping only, with linear    */
/*  interpolation on playback.             */
/*******************************************/

#if !defined(__RawLoop_h)
#define __RawLoop_h

#include "Object.h"

class RawLoop : public Object
{
  protected:  
    long length;
    MY_FLOAT *data;
    MY_FLOAT rate;
    MY_FLOAT time;
    MY_FLOAT phaseOffset;
    MY_FLOAT lastOutput;
  public:
    RawLoop(char *fileName);
    ~RawLoop();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    void setFreq(MY_FLOAT aFreq);    
    void addTime(MY_FLOAT aTime);
    void addPhase(MY_FLOAT anAngle);
    void addPhaseOffset(MY_FLOAT anAngle);
    MY_FLOAT tick();
    MY_FLOAT lastOut();
};

#endif
