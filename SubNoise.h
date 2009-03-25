/*******************************************/
/*  SubSampled Noise Generator Class,      */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#if !defined(__SubNoise_h)
#define __SubNoise_h

#include "Noise.h"

class SubNoise : public Noise
{
  protected:  
     int counter;
     int howOften;
  public:
    SubNoise();
    ~SubNoise();
    SubNoise(int subSample);
    void setHowOften(int howOft);
    MY_FLOAT tick();
};

#endif
