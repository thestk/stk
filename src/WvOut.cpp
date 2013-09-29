/***************************************************/
/*! \class WvOut
    \brief STK audio output abstract base class.

    This class provides common functionality for a variety of audio
    data output subclasses.

    WvOut supports multi-channel data.  It is important to distinguish
    the tick() methods, which output single samples to all channels in
    a sample frame, from the tickFrame() methods, which take a pointer
    or reference to multi-channel sample frame data.

    Both interleaved and non-interleaved data is supported via the use
    of StkFrames objects.

    Currently, WvOut is non-interpolating and the output rate is
    always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "WvOut.h"

WvOut :: WvOut()
  : frameCounter_(0), clipping_(false)
{
}

WvOut :: ~WvOut()
{
}

unsigned long WvOut :: getFrameCount( void ) const
{
  return frameCounter_;
}

StkFloat WvOut :: getTime( void ) const
{
  return (StkFloat) frameCounter_ / Stk::sampleRate();
}

StkFloat& WvOut :: clipTest( StkFloat& sample )
{
  bool clip = false;
  if ( sample > 1.0 ) {
    sample = 1.0;
    clip = true;
  }
  else if ( sample < -1.0 ) {
    sample = -1.0;
    clip = true;
  }

  if ( clip == true && clipping_ == false ) {
    // First occurrence of clipping since instantiation or reset.
    clipping_ = true;
    errorString_ << "WvOut: data value(s) outside +-1.0 detected ... clamping at outer bound!";
    handleError( StkError::WARNING );
  }

  return sample;
}

void WvOut :: tick( const StkFloat sample )
{
  this->computeSample( sample );
}

void WvOut :: tick( const StkFrames& frames, unsigned int channel )
{
  if ( channel >= frames.channels() ) {
    errorString_ << "WvOut::tick(): channel argument (" << channel << ") is incompatible with StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      computeSample( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      computeSample( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = channel * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      computeSample( frames[iStart++] );
  }
}

void WvOut :: tickFrame( const StkFrames& frames )
{
  this->computeFrames( frames );
}
