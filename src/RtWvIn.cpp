/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to
    RtAudio for realtime audio input.  It is a
    protected subclass of WvIn.

    RtWvIn supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "RtWvIn.h"

RtWvIn :: RtWvIn(int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
{
  channels_ = nChannels;
  int size = bufferFrames;
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
    audio_->openStream(0, 0, device, channels_, format,
                      (int)sampleRate, &size, nBuffers);
    data_ = (StkFloat *) audio_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  bufferSize_ = size;
  lastOutputs_ = (StkFloat *) new StkFloat[channels_];
  for (unsigned int i=0; i<channels_; i++) lastOutputs_[i] = 0.0;
  counter_ = 0;
  stopped_ = true;
}

RtWvIn :: ~RtWvIn()
{
  if ( !stopped_ )
    audio_->stopStream();
  delete audio_;
  data_ = 0; // RtAudio deletes the buffer itself.
}

void RtWvIn :: start()
{
  if ( stopped_ ) {
    audio_->startStream();
    stopped_ = false;
  }
}

void RtWvIn :: stop()
{
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
}

StkFloat RtWvIn :: lastOut(void) const
{
  return WvIn::lastOut();
}

StkFloat RtWvIn :: tick(void)
{
  this->tickFrame();
  return lastOut();
}

StkFloat *RtWvIn :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return WvIn::tick( vector, vectorSize );
}

StkFrames& RtWvIn :: tick( StkFrames& frames, unsigned int channel )
{
  return WvIn::tick( frames, channel );
}

const StkFloat *RtWvIn :: lastFrame() const
{
  return lastOutputs_;
}

const StkFloat *RtWvIn :: tickFrame(void)
{
  if ( stopped_ )
    this->start();

  if (counter_ == 0) {
    try {
      audio_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
  }

  long temp = counter_ * channels_;
  for (unsigned int i=0; i<channels_; i++)
    lastOutputs_[i] = data_[temp++];

  counter_++;
  if (counter_ >= (long) bufferSize_)
    counter_ = 0;

  return lastOutputs_;
}

StkFloat *RtWvIn :: tickFrame(StkFloat *frameVector, unsigned int frames)
{
  return WvIn::tickFrame( frameVector, frames );
}

StkFrames& RtWvIn :: tickFrame( StkFrames& frames )
{
  return WvIn::tickFrame( frames );
}
