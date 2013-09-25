/********************************************/
/*  RawWave Abstract Class,                 */
/*  by Gary P. Scavone, 1998                */ 
/********************************************/

#include "Object.h"

#if !defined(__RawWave_h)
#define __RawWave_h

class RawWave : public Object
{
  public:
    RawWave();
    virtual ~RawWave();
    virtual void reset();
    virtual void normalize(MY_FLOAT newPeak);
    virtual void setRate(MY_FLOAT aRate);
    virtual int informTick();
    virtual MY_FLOAT tick();
    virtual MY_FLOAT lastOut();
};


#endif // defined(__RawWave_h)
