/******************************************/  
/*  Chorus Effect                         */
/*  by Perry Cook, 1996                   */
/******************************************/

#if !defined(__Chorus_h)
#define __Chorus_h

#include "../STK/Object.h" 
#include "../STK/DLineL.h" 
#include "../STK/RawWvIn.h" 

class Chorus : public Object
{
 protected:  
  DLineL *delayLine[2];
  RawWvIn *mods[2];
  MY_FLOAT baseLength;
  MY_FLOAT modDepth;
  MY_FLOAT lastOutL;
  MY_FLOAT lastOutR;
  MY_FLOAT effectMix;
 public:
  Chorus(MY_FLOAT baseDelay);
  ~Chorus();
  void clear();
  void setModDepth(MY_FLOAT depth);
  void setModFreq(MY_FLOAT freq);
  void setEffectMix(MY_FLOAT mix);
  MY_FLOAT lastOutput();
  MY_FLOAT lastOutputL();
  MY_FLOAT lastOutputR();
  MY_FLOAT tick(MY_FLOAT input);
};

#endif

