/*******************************************/
/*  Modulator Class, Perry R. Cook, 1995-96*/ 
/*  This Object combines random and        */
/*  periodic modulations to give a nice    */
/*  natural human modulation function.     */  
/*******************************************/

#if !defined(__Modulatr_h)
#define __Modulatr_h

#include "Object.h"
#include "RawWvIn.h"
#include "SubNoise.h"
#include "OnePole.h"

class Modulatr : public Object
{
 protected:  
  RawWvIn  *vibwave;
  SubNoise *noise;
  OnePole  *onepole;
  MY_FLOAT vibAmt;
  MY_FLOAT rndAmt;
  MY_FLOAT lastOutput;
 public:
  Modulatr();
  ~Modulatr();
  void reset();
  void setVibFreq(MY_FLOAT vibFreq);
  void setVibAmt(MY_FLOAT vibAmount);
  void setRndAmt(MY_FLOAT rndAmount);
  MY_FLOAT tick();
  MY_FLOAT lastOut();
};

#endif
