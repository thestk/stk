/******************************************/
/*  Bowed String model ala Smith          */
/* after McIntyre, Schumacher, Woodhouse  */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = bowPressure  */
/*                CONTROL2 = bowPosition  */
/*		  CONTROL3 = vibrFreq     */
/*		  MOD_WHEEL= vibrGain     */
/*					  */
/******************************************/

#if !defined(__Bowed_h)
#define __Bowed_h

#include "Instrmnt.h"
#include "DLineL.h"
#include "BowTabl.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "RawLoop.h"
#include "ADSR.h"

class Bowed : public Instrmnt
{
  protected:  
    DLineL *neckDelay;
    DLineL *bridgeDelay;
    BowTabl *bowTabl;
    OnePole *reflFilt;
    BiQuad *bodyFilt;
    RawLoop *vibr;
    ADSR *adsr;
    MY_FLOAT maxVelocity;
    MY_FLOAT baseDelay;
    MY_FLOAT vibrGain;
    MY_FLOAT betaRatio;
  public:
    Bowed(MY_FLOAT lowestFreq);
    ~Bowed();
    void clear();
    void startBowing(MY_FLOAT amplitude,MY_FLOAT rate);
    void stopBowing(MY_FLOAT rate);
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amp);
    virtual void setFreq(MY_FLOAT frequency);
    void setVibrato(MY_FLOAT amount);
    virtual void controlChange(int number, MY_FLOAT value);
    virtual MY_FLOAT tick();
};

#endif
