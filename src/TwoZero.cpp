/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "TwoZero.h"
#include <math.h>

TwoZero :: TwoZero() : Filter()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A = 1.0;
  Filter::setCoefficients( 3, B, 1, &A );
}

TwoZero :: ~TwoZero()
{
}

void TwoZero :: clear(void)
{
  Filter::clear();
}

void TwoZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void TwoZero :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void TwoZero :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());

  // Normalize the filter gain.
  if (b[1] > 0.0) // Maximum at z = 0.
    b[0] = 1.0 / (1.0+b[1]+b[2]);
  else            // Maximum at z = -1.
    b[0] = 1.0 / (1.0-b[1]+b[2]);
  b[1] *= b[0];
  b[2] *= b[0];
}

void TwoZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT TwoZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT TwoZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT TwoZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[2] * inputs[2] + b[1] * inputs[1] + b[0] * inputs[0];
  inputs[2] = inputs[1];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *TwoZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
