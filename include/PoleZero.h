/*******************************************/
/*  PoleZero (1-pole, 1-zero) Filter Class */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#if !defined(__PoleZero_h)
#define __PoleZero_h

#include "Filter.h"

class PoleZero : public Filter
{
 protected:  
  MY_FLOAT a1Coeff;
  MY_FLOAT b0Coeff;
  MY_FLOAT b1Coeff;
 public:
  PoleZero();
  ~PoleZero();
  void clear();
  void setA1(MY_FLOAT coeff);
  void setB0(MY_FLOAT coeff);
  void setB1(MY_FLOAT coeff);
  void setGain(MY_FLOAT aValue);
  MY_FLOAT tick(MY_FLOAT sample);
};

#endif
