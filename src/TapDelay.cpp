/***************************************************/
/*! \class TapDelay
    \brief STK non-interpolating tapped delay line class.

    This class implements a non-interpolating digital delay-line with
    an arbitrary number of output "taps".  If the maximum length and
    tap delays are not specified during instantiation, a fixed maximum
    length of 4095 and a single tap delay of zero is set.
    
    A non-interpolating delay line is typically used in fixed
    delay-length applications, such as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "TapDelay.h"

namespace stk {

TapDelay :: TapDelay( std::vector<unsigned long> taps, unsigned long maxDelay )
{
  // Writing before reading allows delays from 0 to length-1. 
  // If we want to allow a delay of maxDelay, we need a
  // delayline of length = maxDelay+1.
  if ( maxDelay < 1 ) {
    errorString_ << "TapDelay::TapDelay: maxDelay must be > 0!\n";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  for ( unsigned int i=0; i<taps.size(); i++ ) {
    if ( taps[i] > maxDelay ) {
      errorString_ << "TapDelay::TapDelay: maxDelay must be > than all tap delay values!\n";
      handleError( StkError::FUNCTION_ARGUMENT );
    }
  }

  if ( ( maxDelay + 1 ) > inputs_.size() )
    inputs_.resize( maxDelay + 1, 1, 0.0 );

  inPoint_ = 0;
  this->setTapDelays( taps );
}

TapDelay :: ~TapDelay()
{
}

void TapDelay :: setMaximumDelay( unsigned long delay )
{
  if ( delay < inputs_.size() ) return;

  if ( delay < 0 ) {
    errorString_ << "TapDelay::setMaximumDelay: argument (" << delay << ") less than zero!\n";
    handleError( StkError::WARNING );
    return;
  }

  for ( unsigned int i=0; i<delays_.size(); i++ ) {
    if ( delay < delays_[i] ) {
      errorString_ << "TapDelay::setMaximumDelay: argument (" << delay << ") less than a current tap delay setting (" << delays_[i] << ")!\n";
      handleError( StkError::WARNING );
      return;
    }
  }

  inputs_.resize( delay + 1 );
}

void TapDelay :: setTapDelays( std::vector<unsigned long> taps )
{
  if ( taps.size() != outPoint_.size() ) {
    outPoint_.resize( taps.size() );
    delays_.resize( taps.size() );
    lastFrame_.resize( 1, taps.size(), 0.0 );
  }

  for ( unsigned int i=0; i<taps.size(); i++ ) {
    if ( taps[i] > inputs_.size() - 1 ) { // The value is too big.
      errorString_ << "TapDelay::setTapDelay: argument (" << taps[i] << ") too big ... setting to maximum!\n";
      handleError( StkError::WARNING );

      // Force delay to maximum length.
      outPoint_[i] = inPoint_ + 1;
      if ( outPoint_[i] == inputs_.size() ) outPoint_[i] = 0;
      delays_[i] = inputs_.size() - 1;
    }
    else if ( taps[i] < 0 ) {
      errorString_ << "TapDelay::setDelay: argument (" << taps[i] << ") less than zero ... setting to zero!\n";
      handleError( StkError::WARNING );

      outPoint_[i] = inPoint_;
      delays_[i] = 0;
    }
    else { // read chases write
      if ( inPoint_ >= taps[i] ) outPoint_[i] = inPoint_ - taps[i];
      else outPoint_[i] = inputs_.size() + inPoint_ - taps[i];
      delays_[i] = taps[i];
    }
  }
}

} // stk namespace

