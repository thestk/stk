/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Echo.h"
#include <iostream>

Echo :: Echo( unsigned long maximumDelay ) : Effect()
{
  this->setMaximumDelay( maximumDelay );
  delayLine_.setDelay( length_ >> 1 );
  effectMix_ = 0.5;
  this->clear();
}

Echo :: ~Echo()
{
}

void Echo :: clear()
{
  delayLine_.clear();
  lastOutput_[0] = 0.0;
  lastOutput_[1] = 0.0;
}

void Echo :: setMaximumDelay( unsigned long delay )
{
  length_ = delay;
  if ( delay == 0 ) {
    errorString_ << "Echo::setMaximumDelay: parameter cannot be zero ... setting to 10!";
    handleError( StkError::WARNING );
    length_ = 10;
  }

  delayLine_.setMaximumDelay( length_ );
}

void Echo :: setDelay( unsigned long delay )
{
  unsigned long size = delay;
  if ( delay > length_ ) {
    errorString_ << "Echo::setDelay: parameter is greater than maximum delay length ... setting to max!";
    handleError( StkError::WARNING );
    size = length_;
  }

  delayLine_.setDelay( size );
}

StkFloat Echo :: tick(StkFloat input)
{
  lastOutput_[0] = effectMix_ * delayLine_.tick(input);
  lastOutput_[0] += input * (1.0 - effectMix_);
  lastOutput_[1] = lastOutput_[0];
  return lastOutput_[0];
}

StkFloat *Echo :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Effect::tick( vector, vectorSize );
}

StkFrames& Echo :: tick( StkFrames& frames, unsigned int channel )
{
  return Effect::tick( frames, channel );
}
