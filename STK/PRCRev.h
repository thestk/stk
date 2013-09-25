/*******************************************/  
/* PRCRev, a simple reverb unit            */
/* by Perry Cook, 1996.                    */
/* Incorporated into the Reverb superclass */
/* by Gary Scavone, 1998.                  */
/*                                         */
/* This is based on some of the famous     */
/* Stanford CCRMA reverbs (NRev, KipRev)   */
/* all based on the the Chowning/Moorer/   */
/* Schroeder reverberators, which use      */
/* networks of simple allpass and comb     */
/* delay filters.                          */
/*******************************************/

#if !defined(__PRCRev_h)
#define __PRCRev_h

#include "Reverb.h" 
#include "DLineN.h" 

class PRCRev : public Reverb
{
  protected:  
    DLineN *APdelayLine[2];
    DLineN *CdelayLine[2];
    MY_FLOAT allPassCoeff;
    MY_FLOAT combCoeff[2];
    MY_FLOAT lastOutL;
    MY_FLOAT lastOutR;
    MY_FLOAT effectMix;
  public:
    PRCRev(MY_FLOAT T60);
    ~PRCRev();
    void clear();
    void setEffectMix(MY_FLOAT mix);
    MY_FLOAT lastOutput();
    MY_FLOAT lastOutputL();
    MY_FLOAT lastOutputR();
    MY_FLOAT tick(MY_FLOAT input);
};

#endif

