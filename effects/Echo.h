/******************************************/  
/*  Echo Effect Applied to Soundfile      */
/*  by Perry Cook, 1996                   */
/******************************************/

#if !defined(__Echo_h)
#define __Echo_h

#include "../STK/Object.h" 
#include "../STK/DLineN.h" 

class Echo : public Object
{
 protected:  
  DLineN *delayLine;
  long length;
  MY_FLOAT lastOut;
  MY_FLOAT effectMix;
 public:
  Echo(MY_FLOAT longestDelay);
  ~Echo();
  void clear();
  void setDelay(MY_FLOAT delay);
  void setEffectMix(MY_FLOAT mix);
  MY_FLOAT lastOutput();
  MY_FLOAT tick(MY_FLOAT input);
};

#endif

