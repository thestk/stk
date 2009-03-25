/*******************************************/
/*  Filter Class, by Perry R. Cook, 1995-96*/ 
/*  This is the base class for all filters.*/
/*  To me, most anything is a filter, but  */
/*  I'll be a little less general here, and*/
/*  define a filter as something which has */
/*  input(s), output(s), and gain.         */
/*******************************************/

#if !defined(__Filter_h)
#define __Filter_h

#include "Object.h"

class Filter : public Object
{
  protected:  
    MY_FLOAT gain;
    MY_FLOAT *outputs;
    MY_FLOAT *inputs;
    MY_FLOAT lastOutput;
  public:
    Filter();
    ~Filter();
    MY_FLOAT lastOut();
};

#endif
