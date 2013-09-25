/*******************************************/
/*  Noise Generator Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#if !defined(__Noise_h)
#define __Noise_h

#include "Object.h"

class Noise : public Object
{
  protected:  
     MY_FLOAT lastOutput;
  public:
    Noise();
    virtual ~Noise();
    MY_FLOAT tick();
    MY_FLOAT lastOut();
};

#endif
