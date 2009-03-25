/******************************************/
/*  Heavy Metal Synth Subclass            */
/*  of Algorithm 3 (TX81Z) Subclass of    */ 
/*  3 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__HeavyMtl_h)
#define __HeavyMtl_h

#include "FM4Alg3.h"

class HeavyMtl : public FM4Alg3
{
  public:
    HeavyMtl();
    ~HeavyMtl();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
