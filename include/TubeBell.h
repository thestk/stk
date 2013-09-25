/******************************************/
/*  Tubular Bell (Orch. Chime) Subclass   */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#if !defined(__TubeBell_h)
#define __TubeBell_h

#include "FM4Alg5.h"

class TubeBell : public FM4Alg5
{
  public:
    TubeBell();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
};

#endif
