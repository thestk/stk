/***************************************************/
/*! \class RtDuplex
    \brief STK realtime audio input/output class.

    This class provides a simplified interface to
    RtAudio for realtime audio input/output.  It
    is also possible to achieve duplex operation
    using separate RtWvIn and RtWvOut classes, but
    this class ensures better input/output
    syncronization.

    RtDuplex supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which output
    single samples to all channels in a sample frame
    and return samples produced by averaging across
    sample frames, from the tickFrame() methods, which
    take/return pointers to multi-channel sample frames.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "RtDuplex.h"

RtDuplex :: RtDuplex(int nChannels, MY_FLOAT sampleRate, int device, int bufferFrames, int nBuffers )
{
  channels_ = nChannels;
  bufferSize_ = bufferFrames;
  RtAudioFormat format = ( sizeof(MY_FLOAT) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  audio_ = 0;
  try {
    audio_ = new RtAudio();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  // Now open a stream and get the buffer pointer.
  try {
    audio_->openStream(device, channels_, device, channels_, format,
                      (int)sampleRate, &bufferSize_, nBuffers);
    data_ = (MY_FLOAT *) audio_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  lastOutput_ = (MY_FLOAT *) new MY_FLOAT[channels_];
  for (unsigned int i=0; i<channels_; i++) lastOutput_[i] = 0.0;
  counter_ = 0;
  stopped_ = true;
}

RtDuplex :: ~RtDuplex()
{
  if ( !stopped_ )
    audio_->stopStream();
  delete audio_;
  delete [] lastOutput_;
  data_ = 0; // RtAudio deletes the buffer itself.
}

void RtDuplex :: start()
{
  if ( stopped_ ) {
    audio_->startStream();
    stopped_ = false;
  }
}

void RtDuplex :: stop()
{
  if ( !stopped_ ) {
    audio_->stopStream();
    stopped_ = true;
  }
}

MY_FLOAT RtDuplex :: lastOut(void) const
{
  if ( channels_ == 1 )
    return *lastOutput_;

  MY_FLOAT output = 0.0;
  for (unsigned int i=0; i<channels_; i++ ) {
    output += lastOutput_[i];
  }
  return output / channels_;
}

MY_FLOAT RtDuplex :: tick(const MY_FLOAT sample)
{
  if ( stopped_ )
    start();

  if (counter_ == 0) {
    try {
      audio_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
  }

  unsigned long temp = counter_ * channels_;
  for (unsigned int i=0; i<channels_; i++) {
    lastOutput_[i] = data_[temp];
    data_[temp++] = sample;
  }

  counter_++;
  if (counter_ >= (long) bufferSize_)
    counter_ = 0;

  return lastOut();
}

MY_FLOAT *RtDuplex :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick(vector[i]);

  return vector;
}

const MY_FLOAT *RtDuplex :: lastFrame() const
{
  return lastOutput_;
}

MY_FLOAT *RtDuplex :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  if ( stopped_ )
    start();

  unsigned int i;
  unsigned long temp;
  for (unsigned int j=0; j<frames; j++ ) {
    if (counter_ == 0) {
      try {
        audio_->tickStream();
      }
      catch (RtError &error) {
        handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
      }
    }

    temp = counter_ * channels_;
    for (i=0; i<channels_; i++) {
      lastOutput_[i] = data_[temp];
      data_[temp++] = frameVector[j*channels_+i];
      frameVector[j*channels_+i] = lastOutput_[i];
    }

    counter_++;
    if (counter_ >= (long) bufferSize_)
      counter_ = 0;
  }

  return frameVector;
}
