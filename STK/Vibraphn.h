/*******************************************/
/*  Vibraphone SubClass of Modal4          */
/*  Instrument, by Perry R. Cook, 1995-96  */ 
/*                                         */
/*   Controls:    CONTROL1 = stickHardness */
/*                CONTROL2 = strikePosition*/
/*		  CONTROL3 = vibFreq       */
/*		  MOD_WHEEL= vibAmt        */
/*******************************************/

#if !defined(__Vibraphn_h)
#define __Vibraphn_h

#include "Modal4.h"

class Vibraphn : public Modal4
{
  protected:
  public:
    Vibraphn();
    ~Vibraphn();
    void setStickHardness(MY_FLOAT hardness);
    void setStrikePosition(MY_FLOAT position);
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
