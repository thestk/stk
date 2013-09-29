/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to RtAudio for realtime
    audio input.  It is a subclass of WvIn.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    from which data is read.  This class should not be used when
    low-latency is desired.

    RtWvIn supports multi-channel data in both interleaved and
    non-interleaved formats.  It is important to distinguish the
    tick() methods, which return samples produced by averaging across
    sample frames, from the tickFrame() methods, which return
    references or pointers to multi-channel sample frames.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "RtWvIn.h"

// This function is automatically called by RtAudio to supply input audio data.
int read( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
          double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
  ( (RtWvIn *) dataPointer )->fillBuffer( inputBuffer, nBufferFrames );
  return 0;
}

// This function does not block.  If the user does not read the buffer
// data fast enough, unread data will be overwritten (data overrun).
void RtWvIn :: fillBuffer( void *buffer, unsigned int nFrames )
{
  StkFloat *samples = (StkFloat *) buffer;
  unsigned int counter, iStart, nSamples = nFrames * data_.channels();

  while ( nSamples > 0 ) {

    // I'm assuming that both the RtAudio and StkFrames buffers
    // contain interleaved data.
    iStart = writeIndex_ * data_.channels();
    counter = nSamples;

    // Pre-increment write pointer and check bounds.
    writeIndex_ += nSamples / data_.channels();
    if ( writeIndex_ >= data_.frames() ) {
      writeIndex_ = 0;
      counter = data_.size() - iStart;
    }

    // Copy data to the StkFrames container.
    for ( unsigned int i=0; i<counter; i++ )
      data_[iStart++] = *samples++;

    nSamples -= counter;
  }

  framesFilled_ += nFrames;
  if ( framesFilled_ > data_.frames() ) {
    framesFilled_ = data_.frames();
    errorString_ << "RtWvIn: audio buffer overrun!";
    handleError( StkError::WARNING );
  }
}

RtWvIn :: RtWvIn( unsigned int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), readIndex_( 0 ), writeIndex_( 0 ), framesFilled_( 0 )
{
  // We'll let RtAudio deal with channel and sample rate limitations.
  RtAudio::StreamParameters parameters;
  parameters.deviceId = device;
  parameters.nChannels = nChannels;
  unsigned int size = bufferFrames;
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  try {
    adc_.openStream( NULL, &parameters, format, (unsigned int)Stk::sampleRate(), &size, &read, (void *)this );
  }
  catch ( RtError &error ) {
    handleError( error.what(), StkError::AUDIO_SYSTEM );
  }

  data_.resize( size * nBuffers, nChannels );
  lastOutputs_.resize( 1, nChannels );
}

RtWvIn :: ~RtWvIn()
{
  if ( !stopped_ ) adc_.stopStream();
  adc_.closeStream();
}

void RtWvIn :: start()
{
  if ( stopped_ ) {
    adc_.startStream();
    stopped_ = false;
  }
}

void RtWvIn :: stop()
{
  if ( !stopped_ ) {
    adc_.stopStream();
    stopped_ = true;
  }
}

void RtWvIn :: computeFrame( void )
{
  if ( stopped_ ) this->start();

  // Block until at least one frame is available.
  while ( framesFilled_ == 0 )
    Stk::sleep( 1 );

  for ( unsigned int i=0; i<lastOutputs_.size(); i++ )
    lastOutputs_[i] = data_( readIndex_, i );

  framesFilled_--;
  readIndex_++;
  if ( readIndex_ >= data_.frames() )
    readIndex_ = 0;
}
