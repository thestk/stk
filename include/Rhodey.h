/******************************************/
/*  Fender Rhodes Electric Piano Subclass */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__Rhodey_h)
#define __Rhodey_h

#include "FM4Alg5.h"

class Rhodey : public FM4Alg5
{
  public:
    Rhodey();
    ~Rhodey();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
