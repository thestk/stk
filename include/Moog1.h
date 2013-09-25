/******************************************/
/*  Moog1 Subclass of                     */                   
/*  Sampling Synthesizer Class            */
/*  by Perry R. Cook, 1995-96             */ 
/*                                        */
/*   Controls:    CONTROL1 = filterQ      */
/*                CONTROL2 = filterRate   */
/*                CONTROL3 = vibFreq      */
/*                MOD_WHEEL= vibAmt       */
/******************************************/

#if !defined(__Moog1_h)
#define __Moog1_h

#include "SamplFlt.h"

class Moog1 : public SamplFlt
{
 private:
  MY_FLOAT modDepth;
  MY_FLOAT filterQ;
  MY_FLOAT filterRate;
 public:
  Moog1();
  ~Moog1();
  virtual void setFreq(MY_FLOAT frequency);
  virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
  void setModulationSpeed(MY_FLOAT mSpeed);
  void setModulationDepth(MY_FLOAT mDepth);
  virtual void controlChange(int number, MY_FLOAT value);
  virtual MY_FLOAT tick();
};

#endif
