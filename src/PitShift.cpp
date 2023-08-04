/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using a delay line.

    by Perry R. Cook and Gary P. Scavone, 1995--2021.
*/
/***************************************************/

#include "PitShift.h"
#include <cmath>

namespace stk {

PitShift :: PitShift( void )
{
  delayLength_ = maxDelay - 24;
  halfLength_ = delayLength_ / 2;
  delay_[0] = 12;
  delay_[1] = maxDelay / 2;

  delayLine_.setMaximumDelay( maxDelay );
  delayLine_.setDelay( delay_[0] );
  effectMix_ = 0.5;
  rate_ = 1.0;
}

void PitShift :: clear()
{
  delayLine_.clear();
  lastFrame_[0] = 0.0;
}

void PitShift :: setShift( StkFloat shift )
{
  if ( shift < 1.0 ) {
    rate_ = 1.0 - shift; 
  }
  else if ( shift > 1.0 ) {
    rate_ = 1.0 - shift;
  }
  else {
    rate_ = 0.0;
    delay_[0] = halfLength_ + 12;
  }
}

StkFrames& PitShift :: tick( StkFrames& frames, unsigned int channel )
{
#if defined(_STK_DEBUG_)
  if ( channel >= frames.channels() ) {
    oStream_ << "PitShift::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *samples = &frames[channel];
  unsigned int hop = frames.channels();
  for ( unsigned int i=0; i<frames.frames(); i++, samples += hop )
    *samples = tick( *samples );

  return frames;
}

StkFrames& PitShift :: tick( StkFrames& iFrames, StkFrames& oFrames, unsigned int iChannel, unsigned int oChannel )
{
#if defined(_STK_DEBUG_)
  if ( iChannel >= iFrames.channels() || oChannel >= oFrames.channels() ) {
    oStream_ << "PitShift::tick(): channel and StkFrames arguments are incompatible!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }
#endif

  StkFloat *iSamples = &iFrames[iChannel];
  StkFloat *oSamples = &oFrames[oChannel];
  unsigned int iHop = iFrames.channels(), oHop = oFrames.channels();
  for ( unsigned int i=0; i<iFrames.frames(); i++, iSamples += iHop, oSamples += oHop )
    *oSamples = tick( *iSamples );

  return iFrames;
}

} // stk namespace
