/******************************************/
/*  Instrument SuperClass for Toolkit96   */
/*  Perry R. Cook, Princeton University   */
/******************************************/

#if !defined(__Instrmnt_h)
#define __Instrmnt_h

#include "Object.h"

class Instrmnt : public Object
{
  protected:
    MY_FLOAT lastOutput;
  public:
    Instrmnt();
    virtual ~Instrmnt();
    MY_FLOAT lastOut();
    virtual void noteOn(MY_FLOAT freq, MY_FLOAT amp);
    virtual void noteOff(MY_FLOAT amp);
    virtual void setFreq(MY_FLOAT frequency);
    virtual MY_FLOAT tick();
    virtual void controlChange(int number, MY_FLOAT value);
};

#endif
