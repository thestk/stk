/*******************************************/
/*  Master Class for Sampling Synthesizer  */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains up to 5       */
/*  attack waves, 5 looped waves, and      */
/*  an ADSR envelope.                      */
/*******************************************/

#if !defined(__Sampler_h)
#define __Sampler_h

#include "Instrmnt.h"
#include "ADSR.h"
#include "RawWave.h"
#include "RawLoop.h"
#include "OnePole.h"

class Sampler : public Instrmnt
{
  protected:  
    ADSR     *adsr; 
    RawWave  *attacks[5];
    RawLoop  *loops[5];
    OnePole  *filter;
    MY_FLOAT baseFreq;
    MY_FLOAT attackRatios[5];
    MY_FLOAT loopRatios[5];
    MY_FLOAT attackGain;
    MY_FLOAT loopGain;
    int whichOne;
  public:
    Sampler();
    virtual ~Sampler();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void keyOn();
    void keyOff();
    virtual void noteOff(MY_FLOAT amplitude);
    virtual MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
