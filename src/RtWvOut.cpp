/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to RtAudio for realtime
    audio output.  It is a subclass of WvOut.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    into which data is written.  This class should not be used when
    low-latency is desired.

    RtWvOut supports multi-channel data in both interleaved and
    non-interleaved formats.  It is important to distinguish the
    tick() methods, which output single samples to all channels in a
    sample frame, from the tickFrame() method, which take a pointer or
    reference to multi-channel sample frame data.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "RtWvOut.h"

// Streaming status states.
enum { RUNNING, EMPTYING, FINISHED };

// This function is automatically called by RtAudio to get audio data for output.
int write( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double streamTime, RtAudioStreamStatus status, void *dataPointer )
{
  return ( (RtWvOut *) dataPointer )->readBuffer( outputBuffer, nBufferFrames );
}

// This function does not block.  If the user does not write output
// data to the buffer fast enough, previous data will be re-output
// (data underrun).
int RtWvOut :: readBuffer( void *buffer, unsigned int frameCount )
{
  unsigned int nSamples, nChannels = data_.channels();
  unsigned int nFrames = frameCount;
  StkFloat *input = (StkFloat *) &data_[ readIndex_ * nChannels ];
  StkFloat *output = (StkFloat *) buffer;
  long counter;

  while ( nFrames > 0 ) {

    // I'm assuming that both the RtAudio and StkFrames buffers
    // contain interleaved data.
    counter = nFrames;

    // Pre-increment read pointer and check bounds.
    readIndex_ += nFrames;
    if ( readIndex_ >= data_.frames() ) {
      counter -= readIndex_ - data_.frames();
      readIndex_ = 0;
    }

    // Copy data from the StkFrames container.
    if ( status_ == EMPTYING && framesFilled_ <= counter ) {
      nSamples = framesFilled_ * nChannels;
      unsigned int i;
      for ( i=0; i<nSamples; i++ ) *output++ = *input++;
      nSamples = (counter - framesFilled_) * nChannels;
      for ( i=0; i<nSamples; i++ ) *output++ = 0.0;
      status_ = FINISHED;
      return 1;
    }
    else {
      nSamples = counter * nChannels;
      for ( unsigned int i=0; i<nSamples; i++ )
        *output++ = *input++;
    }

    nFrames -= counter;
  }

  framesFilled_ -= frameCount;
  if ( framesFilled_ < 0 ) {
    framesFilled_ = 0;
    //    writeIndex_ = readIndex_;
    errorString_ << "RtWvOut: audio buffer underrun!";
    handleError( StkError::WARNING );
  }

  return 0;
}


RtWvOut :: RtWvOut( unsigned int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), readIndex_( 0 ), writeIndex_( 0 ), framesFilled_( 0 ), status_(0)
{
  // We'll let RtAudio deal with channel and sample rate limitations.
  RtAudio::StreamParameters parameters;
  parameters.deviceId = device;
  parameters.nChannels = nChannels;
  unsigned int size = bufferFrames;
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  // Open a stream and set the callback function.
  try {
    dac_.openStream( &parameters, NULL, format, (unsigned int)Stk::sampleRate(), &size, &write, (void *)this );
  }
  catch ( RtError &error ) {
    handleError( error.what(), StkError::AUDIO_SYSTEM );
  }

  data_.resize( size * nBuffers, nChannels );
  unsigned int offset = (unsigned int ) (data_.size() / 2.0);
  writeIndex_ = offset;    // start writing half-way into buffer
  framesFilled_ = offset;
}

RtWvOut :: ~RtWvOut()
{
  // Change status flag to signal callback to clear the buffer and close.
  status_ = EMPTYING;
  while ( status_ != FINISHED || dac_.isStreamRunning() == true ) Stk::sleep( 100 );
  dac_.closeStream();
}

void RtWvOut :: start()
{
  if ( stopped_ ) {
    dac_.startStream();
    stopped_ = false;
  }
}

void RtWvOut :: stop()
{
  if ( !stopped_ ) {
    dac_.stopStream();
    stopped_ = true;
  }
}

void RtWvOut :: computeSample( const StkFloat sample )
{
  if ( stopped_ ) this->start();

  // Block until we have room for at least one frame of output data.
  while ( framesFilled_ == (long) data_.frames() ) Stk::sleep( 1 );

  unsigned int nChannels = data_.channels();
  StkFloat input = sample;
  clipTest( input );
  unsigned long index = writeIndex_ * nChannels;
  for ( unsigned int j=0; j<nChannels; j++ )
    data_[index++] = input;

  framesFilled_++;
  frameCounter_++;
  writeIndex_++;
  if ( writeIndex_ == data_.frames() )
    writeIndex_ = 0;
}

void RtWvOut :: computeFrames( const StkFrames& frames )
{
  if ( data_.channels() != frames.channels() ) {
    errorString_ << "RtWvOut::computeFrames(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( stopped_ ) this->start();

  // Block until we have room for the frames of output data.
  while ( data_.frames() - framesFilled_ < frames.frames() ) Stk::sleep( 1 );

  unsigned int j, nChannels = data_.channels();
  if ( nChannels == 1 || frames.interleaved() ) {

    unsigned int index, iFrames = 0;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      index = writeIndex_ * nChannels;
      for ( j=0; j<nChannels; j++ )
        data_[index] = frames[iFrames++];
        clipTest( data_[index++] );

      framesFilled_++;
      frameCounter_++;
      writeIndex_++;
      if ( writeIndex_ == data_.frames() )
        writeIndex_ = 0;
    }
  }
  else { // non-interleaved frames

    unsigned long hop = frames.frames();
    unsigned int index, iFrame;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      iFrame = i;
      index = writeIndex_ * nChannels;
      for ( j=0; j<nChannels; j++ ) {
        data_[index] = frames[iFrame];
        clipTest( data_[index++] );
        iFrame += hop;
      }

      framesFilled_++;
      frameCounter_++;
      writeIndex_++;
      if ( writeIndex_ == data_.frames() )
        writeIndex_ = 0;
    }
  }
}



