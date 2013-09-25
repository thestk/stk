/*******************************************/
/*  Linearly Interpolating Delay Line      */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */ 
/*  linearly interpolates fractional       */
/*  length.  It is designed to be more     */
/*  efficient if the delay length is not   */
/*  changed very often.                    */
/*******************************************/

#if !defined(__DLineL_h)
#define __DLineL_h

#include "Filter.h"

class DLineL : public Filter
{
 protected:  
  long inPoint;
  long outPoint;
  long length;
  MY_FLOAT alpha;
  MY_FLOAT omAlpha;
 public:
  DLineL();
  DLineL(long max_length);
  ~DLineL();
  void clear();
  void setDelay(MY_FLOAT length);
  MY_FLOAT tick(MY_FLOAT sample);
};

#endif
