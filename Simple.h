/*******************************************/
/*  Master Class for Simple Instrument     */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains 1 looped      */
/*  wave, 1 noise source, 1 biquad filter  */
/*  1 one-pole filter, and 1 ADSR envelope */
/*******************************************/

#if !defined(__Simple_h)
#define __Simple_h

#include "Instrmnt.h"
#include "ADSR.h"
#include "RawLoop.h"
#include "OnePole.h"
#include "TwoPole.h"
#include "TwoZero.h"
#include "Noise.h"

class Simple : public Instrmnt
{
  protected:  
    ADSR     *adsr; 
    RawLoop  *loop;
    OnePole  *filter;
    TwoPole  *bqpoles;
    TwoZero  *bqzeroes;
    Noise    *noise;
    MY_FLOAT baseFreq;
    MY_FLOAT loopGain;
  public:
    Simple();
    ~Simple();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void keyOn();
    void keyOff();
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amplitude);
    virtual MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
