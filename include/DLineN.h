/*******************************************/
/*
   Non-Interpolating Delay Line
   Object by Perry R. Cook 1995-96.
   Revised by Gary Scavone, 1999.
   Added methods by Julius Smith, 2000.

   This one uses either a delay line of
   maximum length specified on creation
   or a default length of 2047 samples.
   A non-interpolating delay line is
   typically used in non-time varying
   (reverb) applications.
 */
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
  MY_FLOAT currentDelay;
public:
  DLineN();
  DLineN(long max_length);
  ~DLineN();  
  void clear();
  void setDelay(MY_FLOAT length);
  MY_FLOAT energy(void);
  long currentInPoint(void);
  long currentOutPoint(void);
  MY_FLOAT contentsAt(int n);
  MY_FLOAT contentsAtNowMinus(int n);
  MY_FLOAT delay(void);
  MY_FLOAT tick(MY_FLOAT sample);
};

#endif

