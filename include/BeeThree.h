/******************************************/
/*  HammondOid Organ Subclass             */
/*  of Algorithm 8 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__BeeThree_h)
#define __BeeThree_h

#include "FM4Alg8.h"

class BeeThree : public FM4Alg8
{
  public:
    BeeThree();
    ~BeeThree();
    virtual void setFreq(MY_FLOAT frequency);
    MY_FLOAT tick();
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
