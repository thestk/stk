/*******************************************/
/*  Non-Interpolating Delay Line           */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation.  A non-  */
/*  interpolating delay line should be     */
/*  used in non-time varying (reverb) or   */
/*  non-critical (????) applications.      */
/*******************************************/

#if !defined(__DLineN_h)
#define __DLineN_h

#include "Filter.h"

class DLineN : public Filter
{
  protected:  
    long inPoint;
    long outPoint;
    long length;
  public:
    DLineN(long max_length);  
    ~DLineN();  
    void clear();
    void setDelay(MY_FLOAT length);
    MY_FLOAT tick(MY_FLOAT sample);
};

#endif
