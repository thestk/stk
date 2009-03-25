/*******************************************/
/*  Modulator Class, Perry R. Cook, 1995-96*/ 
/*  This Object combines random and        */
/*  periodic modulations to give a nice    */
/*  natural human modulation function.     */  
/*******************************************/

#if !defined(__Modulatr_h)
#define __Modulatr_h

#include "Object.h"
#include "RawWave.h"
#include "SubNoise.h"
#include "OnePole.h"

class Modulatr : public Object
{
  protected:  
    RawWave  *vibwave;
    SubNoise *noise;
    OnePole  *onepole;
    double vibAmt;
    double rndAmt;
    double lastOutput;
  public:
    Modulatr();
    ~Modulatr();
    void reset();
    void setVibFreq(double vibFreq);
    void setVibAmt(double vibAmount);
    void setRndAmt(double rndAmount);
    double tick();
    double lastOut();
};

#endif
