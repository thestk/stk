/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Noise.h"
#include <stdlib.h>

Noise :: Noise() : Stk()
{    
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: ~Noise()
{
}

MY_FLOAT Noise :: tick()
{
  lastOutput = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  lastOutput -= 1.0;
  return lastOutput;
}

MY_FLOAT *Noise :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Noise :: lastOut() const
{
  return lastOutput;
}

