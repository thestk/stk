/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This class implements a fractional-length digital delay-line using
    first-order linear interpolation.  If the delay and maximum length
    are not specified during instantiation, a fixed maximum length of
    4095 and a delay of zero is set.

    Linear interpolation is an efficient technique for achieving
    fractional delay lengths, though it does introduce high-frequency
    signal attenuation to varying degrees depending on the fractional
    delay setting.  The use of higher order Lagrange interpolators can
    typically improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "DelayL.h"

namespace stk {

DelayL :: DelayL( StkFloat delay, unsigned long maxDelay )
{
  if ( delay < 0.0 || maxDelay < 1 ) {
    errorString_ << "DelayL::DelayL: delay must be >= 0.0, maxDelay must be > 0!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( delay > (StkFloat) maxDelay ) {
    errorString_ << "DelayL::DelayL: maxDelay must be > than delay argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // Writing before reading allows delays from 0 to length-1. 
  if ( maxDelay + 1 > inputs_.size() )
    inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setDelay( delay );
  doNextOut_ = true;
}

DelayL :: ~DelayL()
{
}

void DelayL :: setMaximumDelay( unsigned long delay )
{
  if ( delay < inputs_.size() ) return;

  if ( delay < 0 ) {
    errorString_ << "DelayL::setMaximumDelay: argument (" << delay << ") less than zero!\n";
    handleError( StkError::WARNING );
    return;
  }
  else if ( delay < delay_ ) {
    errorString_ << "DelayL::setMaximumDelay: argument (" << delay << ") less than current delay setting (" << delay_ << ")!\n";
    handleError( StkError::WARNING );
    return;
  }

  inputs_.resize( delay + 1 );
}

void DelayL :: setDelay( StkFloat delay )
{
  StkFloat outPointer;

  if ( delay + 1 > inputs_.size() ) { // The value is too big.
    errorString_ << "DelayL::setDelay: argument (" << delay << ") too big ... setting to maximum!";
    handleError( StkError::WARNING );

    // Force delay to maxLength
    outPointer = inPoint_ + 1.0;
    delay_ = inputs_.size() - 1;
  }
  else if (delay < 0 ) {
    errorString_ << "DelayL::setDelay: argument (" << delay << ") less than zero ... setting to zero!";
    handleError( StkError::WARNING );

    outPointer = inPoint_;
    delay_ = 0;
  }
  else {
    outPointer = inPoint_ - delay;  // read chases write
    delay_ = delay;
  }

  while (outPointer < 0)
    outPointer += inputs_.size(); // modulo maximum length

  outPoint_ = (long) outPointer;   // integer part
  if ( outPoint_ == inputs_.size() ) outPoint_ = 0;
  alpha_ = outPointer - outPoint_; // fractional part
  omAlpha_ = (StkFloat) 1.0 - alpha_;
}

StkFloat DelayL :: contentsAt( unsigned long tapDelay )
{
  long tap = inPoint_ - tapDelay - 1;
  while ( tap < 0 ) // Check for wraparound.
    tap += inputs_.size();

  return inputs_[tap];
}

} // stk namespace
