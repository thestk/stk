/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio output class.

    This class provides a simplified interface to
    RtAudio for realtime audio output.  It is a
    protected subclass of WvOut.

    RtWvOut supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which output
    single samples to all channels in a sample
    frame, from the tickFrame() method, which
    takes a pointer to multi-channel sample
    frame data.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "RtWvOut.h"
#include <iostream>

RtWvOut :: RtWvOut( unsigned int nChannels, MY_FLOAT sampleRate, int device, int bufferFrames, int nBuffers )
{
  // We'll let RtAudio deal with channel and srate limitations.
  channels = nChannels;
  bufferSize_ = bufferFrames;
  RtAudioFormat format = ( sizeof(MY_FLOAT) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  audio_ = 0;
  try {
    audio_ = new RtAudio();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  // Now open a stream and set the callback function.
  try {
    audio_->openStream(device, (int)channels, 0, 0, format,
                       (int)sampleRate, &bufferSize_, nBuffers);
    data = (MY_FLOAT *) audio_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  stopped_ = true;
}

RtWvOut :: ~RtWvOut()
{
  if ( !stopped_ )
    audio_->stopStream();
  delete audio_;
  data = 0;
}

void RtWvOut :: start()
{
  if ( stopped_ ) {
    audio_->startStream();
    stopped_ = false;
  }
}

void RtWvOut :: stop()
{
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
}

unsigned long RtWvOut :: getFrames( void ) const
{
  return totalCount;
}

MY_FLOAT RtWvOut :: getTime( void ) const
{
  return (MY_FLOAT) totalCount / Stk::sampleRate();
}

void RtWvOut :: tick(const MY_FLOAT sample)
{
  if ( stopped_ )
    start();

  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter >= (unsigned int )bufferSize_ ) {
    try {
      audio_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
    counter = 0;
  }
}

void RtWvOut :: tick(const MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void RtWvOut :: tickFrame(const MY_FLOAT *frameVector, unsigned int frames)
{
  if ( stopped_ )
    start();

  for ( unsigned int i=0; i<frames; i++ ) {
    for ( unsigned int j=0; j<channels; j++ ) {
      data[counter*channels+j] = frameVector[i*channels+j];
    }
    counter++;

    if ( counter >= (unsigned int)bufferSize_ ) {
      try {
        audio_->tickStream();
      }
      catch (RtError &error) {
        handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
      }
      counter = 0;
    }
  }
}

