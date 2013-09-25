/******************************************/  
/*  NRev Reverb Subclass                  */
/*  by Tim Stilson, 1998                  */
/*    based on CLM NRev                   */
/*  Integrated into STK by Gary Scavone   */
/*                                        */
/*  This is based on some of the famous   */
/*  Stanford CCRMA reverbs (NRev, KipRev) */
/*  all based on the the Chowning/Moorer/ */
/*  Schroeder reverberators, which use    */
/*  networks of simple allpass and comb   */
/*  delay filters.  This particular       */
/*  arrangement consists of 6 comb        */
/*  filters in parallel, followed by 3    */
/*  allpass filters, a lowpass filter,    */
/*  and another allpass in series,        */
/*  followed by two allpass filters in    */
/*  parallel with corresponding right and */
/*  left outputs.                         */
/******************************************/

#if !defined(__NRev_h)
#define __NRev_h

#include "Object.h" 
#include "Reverb.h" 
#include "DLineN.h" 

class NRev : public Reverb
{
  protected:  
    DLineN *APdelayLine[8];
    DLineN *CdelayLine[6];
    MY_FLOAT allPassCoeff;
    MY_FLOAT combCoef[6];
	MY_FLOAT lpLastout;
    MY_FLOAT lastOutL;
    MY_FLOAT lastOutR;
    MY_FLOAT effectMix;
  public:
    NRev(MY_FLOAT T60);
    ~NRev();
    void clear();
    void setEffectMix(MY_FLOAT mix);
    MY_FLOAT lastOutput();
    MY_FLOAT lastOutputL();
    MY_FLOAT lastOutputR();
    MY_FLOAT tick(MY_FLOAT input);
};

#endif

