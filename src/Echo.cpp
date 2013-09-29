/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "Echo.h"
#include <iostream>

namespace stk {

Echo :: Echo( unsigned long maximumDelay ) : Effect()
{
  this->setMaximumDelay( maximumDelay );
  delayLine_.setDelay( length_ >> 1 );
  effectMix_ = 0.5;
  this->clear();
}

void Echo :: clear( void )
{
  delayLine_.clear();
  lastFrame_[0] = 0.0;
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

} // stk namespace
