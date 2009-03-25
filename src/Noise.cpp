/***************************************************/
/*! \class Noise
    \brief STK noise generator.

    Generic random number generation using the
    C rand() function.  The quality of the rand()
    function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Noise.h"
#include <stdlib.h>
#include <time.h>

Noise :: Noise() : Generator()
{
  // Seed the random number generator with system time.
  this->setSeed( 0 );
  lastOutput_ = (StkFloat) 0.0;
}

Noise :: Noise( unsigned int seed ) : Generator()
{
  // Seed the random number generator
  this->setSeed( seed );
  lastOutput_ = (StkFloat) 0.0;
}

Noise :: ~Noise()
{
}

void Noise :: setSeed( unsigned int seed )
{
  if ( seed == 0 )
    srand( (unsigned int) time(NULL) );
  else
    srand( seed );
}

StkFloat Noise :: tick()
{
  lastOutput_ = (StkFloat) (2.0 * rand() / (RAND_MAX + 1.0) );
  lastOutput_ -= 1.0;
  return lastOutput_;
}

StkFloat *Noise :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Generator::tick( vector, vectorSize );
}

StkFrames& Noise :: tick( StkFrames& frames, unsigned int channel )
{
  return Generator::tick( frames, channel );
}

