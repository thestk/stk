/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "TwoPole.h"
#include <math.h>

TwoPole :: TwoPole() : Filter()
{
  MY_FLOAT B = 1.0;
  MY_FLOAT A[3] = {1.0, 0.0, 0.0};
  Filter::setCoefficients( 1, &B, 3, A );
}

TwoPole :: ~TwoPole()
{
}

void TwoPole :: clear(void)
{
  Filter::clear();
}

void TwoPole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void TwoPole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void TwoPole :: setA2(MY_FLOAT a2)
{
  a[2] = a2;
}

void TwoPole :: setResonance(MY_FLOAT frequency, MY_FLOAT radius, bool normalize)
{
  a[2] = radius * radius;
  a[1] = (MY_FLOAT) -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Normalize the filter gain ... not terribly efficient.
    MY_FLOAT real = 1 - radius + (a[2] - radius) * cos(TWO_PI * 2 * frequency / Stk::sampleRate());
    MY_FLOAT imag = (a[2] - radius) * sin(TWO_PI * 2 * frequency / Stk::sampleRate());
    b[0] = sqrt( pow(real, 2) + pow(imag, 2) );
  }
}

void TwoPole :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT TwoPole :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT TwoPole :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT TwoPole :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] - a[2] * outputs[2] - a[1] * outputs[1];
  outputs[2] = outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *TwoPole :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
