/********************************************/
/*  Data Input Class,                       */
/*  by Gary P. Scavone, 1999                */ 
/*                                          */
/*  This class can handle multi-channel     */
/*  data, which is assumed to be interwoven */
/*  in the data vector.  Actual data input  */
/*  occurs in the subclasses of WvIn.       */
/********************************************/

#include "Object.h"

#if !defined(__WvIn_h)
#define __WvIn_h

class WvIn : public Object
{
  protected:  
    long length;
    int channels;
    int looping;
    int finished;
    int interpolate;
    MY_FLOAT *data;
    MY_FLOAT time;
    MY_FLOAT rate;
    MY_FLOAT phaseOffset;
    MY_FLOAT *lastOutput;
  public:
    WvIn();
    ~WvIn();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setRate(MY_FLOAT aRate);
    void setFreq(MY_FLOAT aFreq);
    void addTime(MY_FLOAT aTime);
    void addPhase(MY_FLOAT anAngle);
    void addPhaseOffset(MY_FLOAT anAngle);
    void setInterpolate(int anInterpStatus);
    void setLooping(int aLoopStatus);
    long getLength();
    int isFinished();
    MY_FLOAT tick();
    MY_MULTI mtick();
    virtual int informTick();
    MY_FLOAT lastOut();
    MY_MULTI mlastOut();
};

#endif // defined(__WvIn_h)
