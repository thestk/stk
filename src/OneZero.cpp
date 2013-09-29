/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This protected Filter subclass implements
    a one-zero digital filter.  A method is
    provided for setting the zero position
    along the real axis of the z-plane while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "OneZero.h"

OneZero :: OneZero() : Filter()
{
  MY_FLOAT B[2] = {0.5, 0.5};
  MY_FLOAT A = 1.0;
  Filter::setCoefficients( 2, B, 1, &A );
}

OneZero :: OneZero(MY_FLOAT theZero) : Filter()
{
  MY_FLOAT B[2];
  MY_FLOAT A = 1.0;

  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    B[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    B[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  B[1] = -theZero * B[0];
  Filter::setCoefficients( 2, B, 1,  &A );
}

OneZero :: ~OneZero(void)
{
}

void OneZero :: clear(void)
{
  Filter::clear();
}

void OneZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void OneZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void OneZero :: setZero(MY_FLOAT theZero)
{
  // Normalize coefficients for unity gain.
  if (theZero > 0.0)
    b[0] = 1.0 / ((MY_FLOAT) 1.0 + theZero);
  else
    b[0] = 1.0 / ((MY_FLOAT) 1.0 - theZero);

  b[1] = -theZero * b[0];
}

void OneZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT OneZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT OneZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT OneZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[1] * inputs[1] + b[0] * inputs[0];
  inputs[1] = inputs[0];

  return outputs[0];
}

MY_FLOAT *OneZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
