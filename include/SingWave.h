/*******************************************/
/*  "Singing" Looped Soundfile Class,      */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object contains all that's needed */
/*  to make a pitched musical sound, like  */
/*  a simple voice or violin.  In general, */  
/*  it will not be used alone (because of  */
/*  of munchinification effects from pitch */
/*  shifting.  It will be used as an       */
/*  excitation source for other instruments*/
/*******************************************/

#if !defined(__SingWave_h)
#define __SingWave_h

#include "Object.h"
#include "Modulatr.h"
#include "Envelope.h"
#include "StkError.h"

class SingWave : public Object
{
  protected:  
    Modulatr *modulator;
    Envelope *envelope;
    Envelope *pitchEnvelope;
    long length;
    MY_FLOAT *data;
    MY_FLOAT rate;
    MY_FLOAT sweepRate;
    MY_FLOAT mytime;
    MY_FLOAT lastOutput;
  public:
    SingWave(char *fileName);
    ~SingWave();
    void reset();
    void normalize();
    void normalize(MY_FLOAT newPeak);
    void setFreq(MY_FLOAT aFreq);    
    void setVibFreq(MY_FLOAT vibFreq);
    void setVibAmt(MY_FLOAT vibAmt);
    void setRndAmt(MY_FLOAT rndVib);
    void setSweepRate(MY_FLOAT swpRate);
    void setGainRate(MY_FLOAT gainRate);    
    void setGainTarget(MY_FLOAT aTarget);
    void noteOn();
    void noteOff();
    MY_FLOAT tick();
    MY_FLOAT lastOut();
};

#endif
