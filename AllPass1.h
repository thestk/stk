/*******************************************/
/*  1st order allpass filter.              */ 
/*  by Perry R. Cook, 1995-96              */ 
/*  A special case of the one pole         */
/*  one zero filter.                       */
/*******************************************/

#include "Filter.h"

#if !defined(__AllPass1_h)
#define __AllPass1_h

#include "Filter.h"

class AllPass1 : Filter
{
  public:
    AllPass1();
    ~AllPass1();
    void clear();
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
