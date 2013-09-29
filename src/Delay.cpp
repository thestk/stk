/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This class implements a non-interpolating digital delay-line.  If
    the delay and maximum length are not specified during
    instantiation, a fixed maximum length of 4095 and a delay of zero
    is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "Delay.h"

namespace stk {

Delay :: Delay( unsigned long delay, unsigned long maxDelay )
{
  // Writing before reading allows delays from 0 to length-1. 
  // If we want to allow a delay of maxDelay, we need a
  // delay-line of length = maxDelay+1.
  if ( maxDelay < 1 ) {
    errorString_ << "Delay::Delay: maxDelay must be > 0!\n";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( delay > maxDelay ) {
    errorString_ << "Delay::Delay: maxDelay must be > than delay argument!\n";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( ( maxDelay + 1 ) > inputs_.size() )
    inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
}

Delay :: ~Delay()
{
}

void Delay :: setMaximumDelay( unsigned long delay )
{
  if ( delay < inputs_.size() ) return;

  if ( delay < 0 ) {
    errorString_ << "Delay::setMaximumDelay: argument (" << delay << ") less than zero!\n";
    handleError( StkError::WARNING );
    return;
  }
  else if ( delay < delay_ ) {
    errorString_ << "Delay::setMaximumDelay: argument (" << delay << ") less than current delay setting (" << delay_ << ")!\n";
    handleError( StkError::WARNING );
    return;
  }

  inputs_.resize( delay + 1 );
}

void Delay :: setDelay( unsigned long delay )
{
  if ( delay > inputs_.size() - 1 ) { // The value is too big.
    errorString_ << "Delay::setDelay: argument (" << delay << ") too big ... setting to maximum!\n";
    handleError( StkError::WARNING );

    // Force delay to maximum length.
    outPoint_ = inPoint_ + 1;
    if ( outPoint_ == inputs_.size() ) outPoint_ = 0;
    delay_ = inputs_.size() - 1;
  }
  else if ( delay < 0 ) {
    errorString_ << "Delay::setDelay: argument (" << delay << ") less than zero ... setting to zero!\n";
    handleError( StkError::WARNING );

    outPoint_ = inPoint_;
    delay_ = 0;
  }
  else { // read chases write
    if ( inPoint_ >= delay ) outPoint_ = inPoint_ - delay;
    else outPoint_ = inputs_.size() + inPoint_ - delay;
    delay_ = delay;
  }
}

StkFloat Delay :: energy( void ) const
{
  unsigned long i;
  register StkFloat e = 0;
  if ( inPoint_ >= outPoint_ ) {
    for ( i=outPoint_; i<inPoint_; i++ ) {
      register StkFloat t = inputs_[i];
      e += t*t;
    }
  } else {
    for ( i=outPoint_; i<inputs_.size(); i++ ) {
      register StkFloat t = inputs_[i];
      e += t*t;
    }
    for ( i=0; i<inPoint_; i++ ) {
      register StkFloat t = inputs_[i];
      e += t*t;
    }
  }
  return e;
}

StkFloat Delay :: contentsAt( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += inputs_.size();

  return inputs_[tap];
}

StkFloat Delay :: addTo( unsigned long tapDelay, StkFloat value )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += inputs_.size();

  return inputs_[tap]+= value;
}

} // stk namespace
