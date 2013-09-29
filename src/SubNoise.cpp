/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "SubNoise.h"    

SubNoise :: SubNoise(int subRate) : Noise()
{    
  rate_ = subRate;
  counter_ = rate_;
}
SubNoise :: ~SubNoise()
{
}

int SubNoise :: subRate(void) const
{
  return rate_;
}

void SubNoise :: setRate(int subRate)
{
  if (subRate > 0)
    rate_ = subRate;
}

StkFloat SubNoise :: tick()
{
  if ( ++counter_ > rate_ ) {
    Noise::tick();
    counter_ = 1;
  }

  return lastOutput_;
}

StkFloat *SubNoise :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Generator::tick( vector, vectorSize );
}

StkFrames& SubNoise :: tick( StkFrames& frames, unsigned int channel )
{
  return Generator::tick( frames, channel );
}
