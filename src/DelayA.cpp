/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    a first-order allpass filter.  If the delay and maximum length are
    not specified during instantiation, a fixed maximum length of 4095
    and a delay of zero is set.

    An allpass filter has unity magnitude gain but variable phase
    delay properties, making it useful in achieving fractional delays
    without affecting a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay response, the
    minimum delay possible in this implementation is limited to a
    value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "DelayA.h"

namespace stk {

DelayA :: DelayA( StkFloat delay, unsigned long maxDelay )
{
  if ( delay < 0.5 || maxDelay < 1 ) {
    errorString_ << "DelayA::DelayA: delay must be >= 0.5, maxDelay must be > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( delay > (StkFloat) maxDelay ) {
    errorString_ << "DelayA::DelayA: maxDelay must be > than delay argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // Writing before reading allows delays from 0 to length-1. 
  if ( maxDelay + 1 > inputs_.size() )
    inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
  apInput_ = 0.0;
  doNextOut_ = true;
}

DelayA :: ~DelayA()
{
}

void DelayA :: clear()
{
  for ( unsigned int i=0; i<inputs_.size(); i++ )
    inputs_[i] = 0.0;
  lastFrame_[0] = 0.0;
  apInput_ = 0.0;
}

void DelayA :: setMaximumDelay( unsigned long delay )
{
  if ( delay < inputs_.size() ) return;

  if ( delay < 0 ) {
    errorString_ << "DelayA::setMaximumDelay: argument (" << delay << ") less than zero!\n";
    handleError( StkError::WARNING );
    return;
  }
  else if ( delay < delay_ ) {
    errorString_ << "DelayA::setMaximumDelay: argument (" << delay << ") less than current delay setting (" << delay_ << ")!\n";
    handleError( StkError::WARNING );
    return;
  }

  inputs_.resize( delay + 1 );
}

void DelayA :: setDelay( StkFloat delay )
{
  StkFloat outPointer;
  unsigned long length = inputs_.size();

  if ( delay + 1 > length ) { // The value is too big.
    errorString_ << "DelayA::setDelay: argument (" << delay << ") too big ... setting to maximum!";
    handleError( StkError::WARNING );

    // Force delay to maxLength
    outPointer = inPoint_ + 1.0;
    delay_ = length - 1;
  }
  else if ( delay < 0.5 ) {
    errorString_ << "DelayA::setDelay: argument (" << delay << ") less than 0.5 not possible!";
    handleError( StkError::WARNING );

    outPointer = inPoint_ + 0.4999999999;
    delay_ = 0.5;
  }
  else {
    outPointer = inPoint_ - delay + 1.0;     // outPoint chases inpoint
    delay_ = delay;
  }

  while ( outPointer < 0 )
    outPointer += length;  // modulo maximum length

  outPoint_ = (long) outPointer;         // integer part
  if ( outPoint_ == length ) outPoint_ = 0;
  alpha_ = 1.0 + outPoint_ - outPointer; // fractional part

  if ( alpha_ < 0.5 ) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint_ += 1;
    if (outPoint_ >= length) outPoint_ -= length;
    alpha_ += (StkFloat) 1.0;
  }

  coeff_ = ((StkFloat) 1.0 - alpha_) / 
    ((StkFloat) 1.0 + alpha_);         // coefficient for all pass
}

StkFloat DelayA :: contentsAt( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += inputs_.size();

  return inputs_[tap];
}

} // stk namespace
