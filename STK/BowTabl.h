/***********************************************/ 
/*  Simple Bow Table Object, after Smith       */
/*    by Perry R. Cook, 1995-96                */
/***********************************************/

#if !defined(__BowTabl_h)
#define __BowTabl_h

#include "Object.h"

class BowTabl : public Object
{
  protected:  
    MY_FLOAT offSet;
    MY_FLOAT slope;
    MY_FLOAT lastOutput;
  public:
    BowTabl();
    ~BowTabl();
    void setOffset(MY_FLOAT aValue);
    void setSlope(MY_FLOAT aValue);
    MY_FLOAT lookup(MY_FLOAT sample);
    MY_FLOAT lastOut();
};

#endif
