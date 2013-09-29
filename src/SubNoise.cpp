/***************************************************/
/*! \class SubNoise
    \brief STK sub-sampled noise generator.

    Generates a new random number every "rate" ticks
    using the C rand() function.  The quality of the
    rand() function varies from one OS to another.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "SubNoise.h"    

SubNoise :: SubNoise( int subRate ) : Noise()
{
  if ( subRate <= 0 ) {
    errorString_ << "SubNoise: subRate argument is negative ... setting to 16!";
    handleError( StkError::WARNING );
    rate_ = 16;
  }

  // Precompute an output.
  Noise::computeSample();
  counter_ = rate_;
}

SubNoise :: ~SubNoise()
{
}

void SubNoise :: setRate( int subRate )
{
  if ( subRate > 0 )
    rate_ = subRate;
}

StkFloat SubNoise :: computeSample()
{
  if ( counter_-- == 0 ) {
    Noise::computeSample();
    counter_ = rate_;
  }

  return lastOutput_;
}

