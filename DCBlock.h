/*******************************************/
/*  DC Blocking Filter                     */ 
/*  by Perry R. Cook, 1995-96              */ 
/*  This guy is very helpful in, uh,       */
/*  blocking DC.  Needed because a simple  */
/*  low-pass reflection filter allows DC   */
/*  to build up inside recursive           */ 
/*  structures.                            */
/*******************************************/

#if !defined(__DCBlock_h)
#define __DCBlock_h

#include "Filter.h"

class DCBlock : public Filter
{
  public:
    DCBlock();
    ~DCBlock();
    void clear();
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
