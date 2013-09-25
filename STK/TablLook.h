/********************************************/
/*  Table Lookup Class,                     */
/*  by Gary P. Scavone, 1999                */ 
/*                                          */
/*  This class loads a table of floating    */
/*  point doubles, which are assumed to be  */
/*  in big-endian format.  Linear           */
/*  interpolation is performed for          */
/*  fractional lookup indexes.              */
/********************************************/

#include "Object.h"

#if !defined(__TablLook_h)
#define __TablLook_h

class TablLook : public Object
{
  protected:  
    long length;
    MY_FLOAT *data;
    MY_FLOAT lastOutput;
  public:
    TablLook(char *fileName);
    ~TablLook();
    long getLength();
    MY_FLOAT tick(MY_FLOAT index);
    MY_FLOAT lastOut();
};

#endif // defined(__TablLook_h)
