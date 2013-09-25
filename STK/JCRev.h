/*******************************************/  
/*  JVRev Reverb Subclass                  */
/*  by Tim Stilson, 1998                   */
/*    based on CLM JCRev                   */
/*  Integrated into STK by Gary Scavone    */
/*                                         */
/*  This is based on some of the famous    */
/*  Stanford CCRMA reverbs (NRev, KipRev)  */
/*  all based on the Chowning/Moorer/      */
/*  Schroeder reverberators, which use     */
/*  networks of simple allpass and comb    */
/*  delay filters.  This particular        */
/*  arrangement consists of 3 allpass      */
/*  filters in series, followed by 4 comb  */
/*  filters in parallel, an optional       */
/*  lowpass filter, and two decorrelation  */
/*  delay lines in parallel at the output. */
/*******************************************/

#if !defined(__JCRev_h)
#define __JCRev_h

#include "Object.h" 
#include "Reverb.h"
#include "DLineN.h" 

class JCRev : public Reverb
{
  protected:  
    DLineN *APdelayLine[3];
    DLineN *CdelayLine[4];
    DLineN *outLdelayLine;
    DLineN *outRdelayLine;
    MY_FLOAT allPassCoeff;
    MY_FLOAT combCoeff[4];
	MY_FLOAT combsum,combsum1,combsum2;
    MY_FLOAT lastOutL;
    MY_FLOAT lastOutR;
    MY_FLOAT effectMix;
  public:
    JCRev(MY_FLOAT T60);
    ~JCRev();
    void clear();
    void setEffectMix(MY_FLOAT mix);
    MY_FLOAT lastOutput();
    MY_FLOAT lastOutputL();
    MY_FLOAT lastOutputR();
    MY_FLOAT tick(MY_FLOAT input);
};

#endif

