/******************************************/  
/*  Reverb Effect Applied to Soundfile    */
/*  by Perry Cook, 1996                   */
/*					  */
/*  This is based on some of the famous   */
/*  Stanford CCRMA reverbs (NRev, KipRev) */
/*  all based on the the Chowning/Moorer/ */
/*  Schroeder reverberators, which use    */
/*  networks of simple allpass and comb   */
/*  delay filters.                        */
/******************************************/

#if !defined(__Reverb_h)
#define __Reverb_h

#include "Object.h" 
#include "DLineN.h" 

class Reverb : public Object
{
  protected:  
    DLineN *delayLine[4];
    MY_FLOAT allPassCoeff;
    MY_FLOAT combCoeff1;
    MY_FLOAT combCoeff2;
    MY_FLOAT lastOutL;
    MY_FLOAT lastOutR;
    MY_FLOAT effectMix;
  public:
    Reverb(MY_FLOAT longestDelay);
    ~Reverb();
    void clear();
    void setEffectMix(MY_FLOAT mix);
    MY_FLOAT lastOutput();
    MY_FLOAT lastOutputL();
    MY_FLOAT lastOutputR();
    MY_FLOAT tick(MY_FLOAT input);
};

#endif

