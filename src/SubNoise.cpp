/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "SubNoise.h"    

SubNoise :: SubNoise(int subRate) : Noise()
{    
  rate = subRate;
  counter = rate;
}
SubNoise :: ~SubNoise()
{
}

int SubNoise :: subRate(void) const
{
  return rate;
}

void SubNoise :: setRate(int subRate)
{
  if (subRate > 0)
    rate = subRate;
}

MY_FLOAT SubNoise :: tick()
{
  if ( ++counter > rate ) {
    Noise::tick();
    counter = 1;
  }

  return lastOutput;
}
