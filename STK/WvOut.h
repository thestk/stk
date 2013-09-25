/********************************************/
/*  WvOut Abstract Class,                   */
/*  by Tim Stilson, 1996                    */
/*  revised by Gary P. Scavone, 1999        */
/*                                          */
/*  This class can handle multi-channel     */
/*  data via the mtick() method.            */
/********************************************/

#include "Object.h"

#if !defined(__WvOut_h)
#define __WvOut_h



class WvOut : public Object
{
  public:
    WvOut();
    virtual ~WvOut();
    virtual void tick(MY_FLOAT sample);
    virtual void mtick(MY_MULTI samples);
};


#endif // defined(__WvOut_h)
