/***************************************************/
/*! \class JetTabl
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "JetTabl.h"

JetTabl :: JetTabl()
{
  lastOutput = (MY_FLOAT) 0.0;
}

JetTabl :: ~JetTabl()
{
}

MY_FLOAT JetTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT JetTabl :: tick( MY_FLOAT input )
{
  // Perform "table lookup" using a polynomial
  // calculation (x^3 - x), which approximates
  // the jet sigmoid behavior.
  lastOutput = input * (input * input - (MY_FLOAT)  1.0);

  // Saturate at +/- 1.0.
  if (lastOutput > 1.0) 
    lastOutput = (MY_FLOAT) 1.0;
  if (lastOutput < -1.0)
    lastOutput = (MY_FLOAT) -1.0; 
  return lastOutput;
}

MY_FLOAT *JetTabl :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
