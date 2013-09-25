/*********************************************/  
/* PitchShift Effect                         */
/*  by Perry Cook, 1996                      */
/*********************************************/

#if !defined(__PitShift_h)
#define __PitShift_h

#include "Object.h" 
#include "DLineL.h" 

class PitShift : public Object
{
 protected:  
  DLineL *delayLine[2];
  MY_FLOAT lastOut;
  MY_FLOAT delay[2];
  MY_FLOAT env[2];
  MY_FLOAT effectMix;
  MY_FLOAT rate;
 public:
  PitShift();
  ~PitShift();
  void clear();
  void setShift(MY_FLOAT shift);
  void setEffectMix(MY_FLOAT mix);
  virtual MY_FLOAT lastOutput();
  MY_FLOAT tick(MY_FLOAT input);
};

#endif

