/******************************************/
/*  Percussive Flute Subclass             */
/*  of Algorithm 4 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__PercFlut_h)
#define __PercFlut_h

#include "FM4Alg4.h"

class PercFlut : public FM4Alg4
{
  public:
    PercFlut();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
