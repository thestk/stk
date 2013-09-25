/********************************************/
/*  Output Abstract Class,                  */
/*  by Tim Stilson, 1996                    */ 
/*  based on code by Perry R. Cook, 1995-96 */ 
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
};


#endif // defined(__WvOut_h)
