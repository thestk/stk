/*******************************************/
/*  Marimba SubClass of Modal4 Instrument, */
/*  by Perry R. Cook, 1995-96              */ 
/*                                         */
/*   Controls:    CONTROL1 = stickHardness */
/*                CONTROL2 = strikePosition*/
/*		  CONTROL3 = vibFreq       */
/*		  MOD_WHEEL= vibAmt        */
/*******************************************/

#if !defined(__Marimba_h)
#define __Marimba_h

#include "Modal4.h"

class Marimba : public Modal4
{
  private:
      int multiStrike;  
  public:
    Marimba();
    ~Marimba();
    void setStickHardness(MY_FLOAT hardness);
    void setStrikePosition(MY_FLOAT position);
    void setModulationSpeed(MY_FLOAT mSpeed);
    void setModulationDepth(MY_FLOAT mDepth);
    virtual void strike(MY_FLOAT amplitude);
    virtual void controlChange(int number, MY_FLOAT value);
    virtual MY_FLOAT tick();
};

#endif
