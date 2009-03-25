/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This protected Filter subclass implements
    a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass
    filter with a given coefficient.  Another
    method is provided to create a DC blocking filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "PoleZero.h"

PoleZero :: PoleZero() : Filter()
{
  // Default setting for pass-through.
  MY_FLOAT B[2] = {1.0, 0.0};
  MY_FLOAT A[2] = {1.0, 0.0};
  Filter::setCoefficients( 2, B, 2, A );
}

PoleZero :: ~PoleZero()
{
}

void PoleZero :: clear(void)
{
  Filter::clear();
}

void PoleZero :: setB0(MY_FLOAT b0)
{
  b[0] = b0;
}

void PoleZero :: setB1(MY_FLOAT b1)
{
  b[1] = b1;
}

void PoleZero :: setA1(MY_FLOAT a1)
{
  a[1] = a1;
}

void PoleZero :: setAllpass(MY_FLOAT coefficient)
{
  b[0] = coefficient;
  b[1] = 1.0;
  a[0] = 1.0; // just in case
  a[1] = coefficient;
}

void PoleZero :: setBlockZero(MY_FLOAT thePole)
{
  b[0] = 1.0;
  b[1] = -1.0;
  a[0] = 1.0; // just in case
  a[1] = -thePole;
}

void PoleZero :: setGain(MY_FLOAT theGain)
{
  Filter::setGain(theGain);
}

MY_FLOAT PoleZero :: getGain(void) const
{
  return Filter::getGain();
}

MY_FLOAT PoleZero :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT PoleZero :: tick(MY_FLOAT sample)
{
  inputs[0] = gain * sample;
  outputs[0] = b[0] * inputs[0] + b[1] * inputs[1] - a[1] * outputs[1];
  inputs[1] = inputs[0];
  outputs[1] = outputs[0];

  return outputs[0];
}

MY_FLOAT *PoleZero :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

