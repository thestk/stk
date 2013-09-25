/********************************************/  
/*
   General Finite-Impulse-Response (FIR)
   Digital Filter Class
   by Julius Smith, 1997
*/
/********************************************/  

#if !defined(__FIR_h)
#define __FIR_h

#include "Object.h"

class FIR : public Object
{
protected:  
  int length;
  MY_FLOAT *coeffs;
  MY_FLOAT *pastInputs;
  int piOffset;
  MY_FLOAT delay;
public:
  FIR(int length);
  FIR(const char *filterFile);
  ~FIR();
  void clear(void);
  void setCoeffs(MY_FLOAT *theCoeffs);
  MY_FLOAT tick(MY_FLOAT input);
  MY_FLOAT lastOutput;
  MY_FLOAT getDelay(MY_FLOAT freq);
  int getLength(void);
};

#endif
