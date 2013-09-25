/******************************************/
/*  Singing Voice Synthesis Subclass      */
/*  of Algorithm 6 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1996                */ 
/******************************************/

#if !defined(__FMVoices_h)
#define __FMVoices_h

#include "FM4Alg6.h"

class FMVoices : public FM4Alg6
{
  protected:
    int currentVowel;
  public:
    FMVoices();
    virtual void setFreq(MY_FLOAT frequency);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
