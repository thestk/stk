/***************************************************/
/*! \class BiQuad
    \brief STK biquad (two-pole, two-zero) filter class.

    This protected Filter subclass implements a
    two-pole, two-zero digital filter.  A method
    is provided for creating a resonance in the
    frequency response while maintaining a constant
    filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "BiQuad.h"
#include <math.h>

BiQuad :: BiQuad() : Filter()
{
  MY_FLOAT B[3] = {1.0, 0.0, 0.0};
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  Filter::setCoefficients( 3, B, 3, A );
}

BiQuad :: ~BiQuad()
{
}

void BiQuad :: clear(void)
{
  Filter::clear();
}

void BiQuad :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void BiQuad :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void BiQuad :: setB2(MY_FLOAT b2)
{
  b[2] = b2;
}

void BiQuad :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void BiQuad :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void BiQuad :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Use zeros at +- 1 and normalize the filter peak gain.
    b[0] = 0.5 - 0.5 * a[2];
    b[1] = 0.0;
    b[2] = -b[0];
  }
}

void BiQuad :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  // This method does not attempt to normalize the filter gain.
  b[2] = radius * radius;
  b[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());
}

void BiQuad :: setEqualGainZeroes()
{
  b[0] = 1.0;
  b[1] = 0.0;
  b[2] = -1.0;
}

void BiQuad :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT BiQuad :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT BiQuad :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT BiQuad :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] + b[2] * inputs[2];
  outputs[0] -= a[2] * outputs[2] + a[1] * outputs[1];
  inputs[2] = inputs[1];
  inputs[1] = inputs[0];
  outputs[2] = outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *BiQuad :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
