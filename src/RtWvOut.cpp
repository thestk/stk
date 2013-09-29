/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "RtWvOut.h"
#include <iostream>

RtWvOut :: RtWvOut( unsigned int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
{
  // We'll let RtAudio deal with channel and srate limitations.
  channels_ = nChannels;
  bufferSize_ = bufferFrames;
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  audio_ = 0;
  try {
    audio_ = new RtAudio();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  // Now open a stream and get the buffer pointer.
  try {
    audio_->openStream(device, (int)channels_, 0, 0, format,
                       (int)sampleRate, &bufferSize_, nBuffers);
    dataPtr_ = (StkFloat *) audio_->getStreamBuffer();
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
  return totalCount_;
}

StkFloat RtWvOut :: getTime( void ) const
{
  return (StkFloat) totalCount_ / Stk::sampleRate();
}

void RtWvOut :: tick( const StkFloat sample )
{
  if ( stopped_ )
    start();

  StkFloat input = sample;
  this->clipTest( input );
  for ( unsigned int j=0; j<channels_; j++ )
    dataPtr_[counter_*channels_+j] = input;

  counter_++;
  totalCount_++;

  if ( counter_ >= (unsigned int )bufferSize_ ) {
    try {
      audio_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
    counter_ = 0;
  }
}

void RtWvOut :: tick( const StkFloat *vector, unsigned int vectorSize )
{
  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void RtWvOut :: tick( const StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "RtWvOut::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( stopped_ )
    start();

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      tick( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      tick( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      tick( frames[iStart + i] );
  }
}

void RtWvOut :: tickFrame( const StkFloat *frameVector, unsigned int frames )
{
  if ( stopped_ )
    start();

  StkFloat sample;
  for ( unsigned int i=0; i<frames; i++ ) {
    for ( unsigned int j=0; j<channels_; j++ ) {
      sample = frameVector[i*channels_+j];
      this->clipTest( sample );
      dataPtr_[counter_*channels_+j] = sample;
    }
    counter_++;
    totalCount_++;

    if ( counter_ >= (unsigned int)bufferSize_ ) {
      try {
        audio_->tickStream();
      }
      catch (RtError &error) {
        handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
      }
      counter_ = 0;
    }
  }
}

void RtWvOut :: tickFrame( const StkFrames& frames )
{
  if ( channels_ != frames.channels() ) {
    errorString_ << "RtWvOut::tickFrame(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( stopped_ )
    start();

  unsigned int j;
  StkFloat sample;
  if ( channels_ == 1 || frames.interleaved() ) {
    unsigned long iFrames = 0, iData = counter_;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      for ( j=0; j<channels_; j++ ) {
        sample = frames[iFrames++];
        this->clipTest( sample );
        dataPtr_[iData++] = sample;
      }
      counter_++;
      totalCount_++;

      if ( counter_ >= (unsigned int)bufferSize_ ) {
        try {
          audio_->tickStream();
        }
        catch (RtError &error) {
          handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
        }
        counter_ = 0;
      }
    }
  }
  else {
    unsigned int hop = frames.frames();
    unsigned long iData = counter_;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      for ( j=0; j<channels_; j++ ) {
        sample = frames[i + j*hop];
        this->clipTest( sample );
        dataPtr_[iData++] = sample;
      }
      counter_++;
      totalCount_++;

      if ( counter_ >= (unsigned int)bufferSize_ ) {
        try {
          audio_->tickStream();
        }
        catch (RtError &error) {
          handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
        }
        counter_ = 0;
      }
    }
  }
}
