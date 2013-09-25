/******************************************/
/*  Wurlitzer Electric Piano Subclass     */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__Wurley_h)
#define __Wurley_h

#include "FM4Alg5.h"

class Wurley : public FM4Alg5
{
  public:
    Wurley();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
