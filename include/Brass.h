/******************************************/
/*  Simple Brass Instrument Model ala     */
/*  Cook (TBone, HosePlayer)              */
/*  by Perry R. Cook, 1995-96             */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = lipTension   */
/*                CONTROL2 = slideLength  */
/*                CONTROL3 = vibFreq      */
/*                MOD_WHEEL= vibAmt       */
/******************************************/

#if !defined(__Brass_h)
#define __Brass_h

#include "Instrmnt.h"
#include "DLineA.h"
#include "LipFilt.h"
#include "DCBlock.h"
#include "ADSR.h"
#include "RawWvIn.h"

class Brass: public Instrmnt
{
  protected:  
    DLineA *delayLine;
    LipFilt *lipFilter;
    DCBlock *dcBlock;
    ADSR *adsr;
    RawWvIn *vibr;
    long length;
    MY_FLOAT lipTarget;
    MY_FLOAT slideTarget;
    MY_FLOAT vibrGain;
    MY_FLOAT maxPressure;
  public:
    Brass(MY_FLOAT lowestFreq);
    ~Brass();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void setLip(MY_FLOAT frequency);
    void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);
    void stopBlowing(MY_FLOAT rate);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amp);
    virtual void controlChange(int number, MY_FLOAT value);
    virtual MY_FLOAT tick();
};

#endif
