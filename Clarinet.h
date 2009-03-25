/******************************************/
/*  Clarinet model ala Smith              */
/*  after McIntyre, Schumacher, Woodhouse */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = reedStiffns  */
/*                CONTROL2 = noiseGain    */
/*		  CONTROL3 = vibFreq      */
/*		  MOD_WHEEL= vibAmt       */
/******************************************/

#if !defined(__Clarinet_h)
#define __Clarinet_h

#include "Instrmnt.h"
#include "DLineL.h"
#include "ReedTabl.h"
#include "OneZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "RawLoop.h"

class Clarinet : public Instrmnt
{
  protected:  
    DLineL *delayLine;
    ReedTabl *reedTable;
    OneZero *filter;
    Envelope *envelope;
    Noise *noise;
    RawLoop *vibr;
    long length;
    MY_FLOAT outputGain;
    MY_FLOAT noiseGain;
    MY_FLOAT vibrGain;
  public:
    Clarinet(MY_FLOAT lowestFreq);
    ~Clarinet();
    void clear();
    virtual void setFreq(MY_FLOAT frequency);
    void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);
    void stopBlowing(MY_FLOAT rate);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amp);
    virtual MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
