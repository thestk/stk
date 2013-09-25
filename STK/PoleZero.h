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
  MY_FLOAT poleCoeff;
  MY_FLOAT zeroCoeff;
 public:
  PoleZero();
  ~PoleZero();
  void clear();
  void setPoleCoeff(MY_FLOAT coeff);
  void setZeroCoeff(MY_FLOAT coeff);
  void setGain(MY_FLOAT aValue);
  MY_FLOAT tick(MY_FLOAT sample);
};

#endif
