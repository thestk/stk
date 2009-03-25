/***************************************************/
/*! \class BowTabl
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "BowTabl.h"
#include <math.h>

BowTabl :: BowTabl()
{
  offSet = (MY_FLOAT) 0.0;
  slope = (MY_FLOAT) 0.1;
}

BowTabl :: ~BowTabl()
{
}

void BowTabl :: setOffset(MY_FLOAT aValue)
{
  offSet = aValue;
}

void BowTabl :: setSlope(MY_FLOAT aValue)
{
  slope = aValue;
}

MY_FLOAT BowTabl :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT BowTabl :: tick(MY_FLOAT input)
{
  // The input represents differential string vs. bow velocity.
  MY_FLOAT sample;
  sample = input + offSet;  // add bias to input
  sample *= slope;          // then scale it
  lastOutput = (MY_FLOAT)  fabs((double) sample) + (MY_FLOAT) 0.75;
  lastOutput = (MY_FLOAT)  pow( lastOutput,(MY_FLOAT) -4.0 );

  // Set minimum friction to 0.0
  //if (lastOutput < 0.0 ) lastOutput = 0.0;
  // Set maximum friction to 1.0.
  if (lastOutput > 1.0 ) lastOutput = (MY_FLOAT) 1.0;

  return lastOutput;
}

MY_FLOAT *BowTabl :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
