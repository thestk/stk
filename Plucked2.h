/******************************************/  
/*  Enhanced (Jaffe-Smith, Smith, others) */
/*  Karplus-Strong plucked model          */
/*  by Perry Cook, 1995-96                */
/*  This is the super-class, with no      */
/*  excitation specified.  So this one by */
/*  itself doesn't make any sound.        */
/******************************************/

#if !defined(__Plucked2_h)
#define __Plucked2_h

#include "Instrmnt.h"
#include "DLineL.h"
#include "DLineA.h"
#include "OneZero.h"

class Plucked2 : public Instrmnt
{
  protected:  
    DLineA *delayLine;
    DLineA *delayLine2;
    DLineL *combDelay;
    OneZero *filter;
    OneZero *filter2;
    long length;
    MY_FLOAT loopGain;
    MY_FLOAT baseLoopGain;
    MY_FLOAT lastFreq;
    MY_FLOAT lastLength;
    MY_FLOAT detuning;
    MY_FLOAT pluckAmp;
    MY_FLOAT pluckPos;
  public:
    Plucked2(MY_FLOAT lowestFreq);
    ~Plucked2();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void setDetune(MY_FLOAT detune);
    void setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune);
    void setPluckPos(MY_FLOAT position);
    void setBaseLoopGain(MY_FLOAT aGain);
    virtual void noteOff(MY_FLOAT amp);
};

#endif
