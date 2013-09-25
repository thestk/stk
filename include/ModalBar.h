/*******************************************/
/*
  ModalBar SubClass of Modal4 Instrument
  by Perry R. Cook, 1999-2000

  Controls:    CONTROL1 = stickHardness
               CONTROL2 = strikePosition
               CONTROL3 = Mode Presets
*/
/*******************************************/

#if !defined(__ModalBar_h)
#define __ModalBar_h

#include "Modal4.h"

class ModalBar : public Modal4
{
private:
public:
  ModalBar();
  ~ModalBar();
  void setStickHardness(MY_FLOAT hardness);
  void setStrikePosition(MY_FLOAT position);
  void setModalPreset(int which);
  void setModulationDepth(MY_FLOAT mDepth);
  virtual void controlChange(int number, MY_FLOAT value);
};

#endif
