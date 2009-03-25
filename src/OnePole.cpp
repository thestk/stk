/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This protected Filter subclass implements
    a one-pole digital filter.  A method is
    provided for setting the pole position along
    the real axis of the z-plane while maintaining
    a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "OnePole.h"

OnePole :: OnePole() : Filter()
{
  MY_FLOAT B = 0.1;
  MY_FLOAT A[2] = {1.0, -0.9};
  Filter::setCoefficients( 1, &B, 2, A );
}

OnePole :: OnePole(MY_FLOAT thePole) : Filter()
{
  MY_FLOAT B;
  MY_FLOAT A[2] = {1.0, -0.9};

  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    B = (MY_FLOAT) (1.0 - thePole);
  else
    B = (MY_FLOAT) (1.0 + thePole);

  A[1] = -thePole;
  Filter::setCoefficients( 1, &B, 2,  A );
}

OnePole :: ~OnePole()    
{
}

void OnePole :: clear(void)
{
  Filter::clear();
}

void OnePole :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OnePole :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void OnePole :: setPole(MY_FLOAT thePole)
{
  // Normalize coefficients for peak unity gain.
  if (thePole > 0.0)
    b[0] = (MY_FLOAT) (1.0 - thePole);
  else
    b[0] = (MY_FLOAT) (1.0 + thePole);

  a[1] = -thePole;
}

void OnePole :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT OnePole :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT OnePole :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT OnePole :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] - a[1] * outputs[1];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *OnePole :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
