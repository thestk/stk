/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Instrmnt.h"

Instrmnt :: Instrmnt()
{
}

Instrmnt :: ~Instrmnt()
{
}

void Instrmnt :: setFrequency(MY_FLOAT frequency)
{
  cerr << "Instrmnt: virtual setFrequency function call!" << endl;
}

MY_FLOAT Instrmnt :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT *Instrmnt :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

void Instrmnt :: controlChange(int number, MY_FLOAT value)
{
}
