/***************************************************/
/*! \class RtDuplex
    \brief STK realtime audio (blocking) input/output class.

    This class provides a simplified interface to
    RtAudio for realtime audio input/output.  It
    may also be possible to achieve duplex
    operation using separate RtWvIn and RtWvOut
    classes, but this class ensures better
    input/output synchronization.

    Because this class makes use of RtAudio's
    blocking input/output routines, its
    performance is less robust on systems where
    the audio API is callback-based (Macintosh
    CoreAudio and Windows ASIO).

    RtDuplex supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which output
    single samples to all channels in a sample
    frame and return samples produced by averaging
    across sample frames, from the tickFrame()
    methods, which take/return pointers to
    multi-channel sample frames.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "RtDuplex.h"

RtDuplex :: RtDuplex(int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
{
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
    audio_->openStream(device, channels_, device, channels_, format,
                      (int)sampleRate, &bufferSize_, nBuffers);
    data_ = (StkFloat *) audio_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  lastOutput_ = (StkFloat *) new StkFloat[channels_];
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

StkFloat RtDuplex :: lastOut(void) const
{
  if ( channels_ == 1 )
    return *lastOutput_;

  StkFloat output = 0.0;
  for (unsigned int i=0; i<channels_; i++ ) {
    output += lastOutput_[i];
  }
  return output / channels_;
}

StkFloat RtDuplex :: tick(const StkFloat sample)
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

StkFloat *RtDuplex :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick(vector[i]);

  return vector;
}

StkFrames& RtDuplex :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "RtDuplex::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick( frames[iStart + i] );
  }

  return frames;
}

const StkFloat *RtDuplex :: lastFrame() const
{
  return lastOutput_;
}

StkFloat *RtDuplex :: tickFrame(StkFloat *frameVector, unsigned int frames)
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

StkFrames& RtDuplex :: tickFrame( StkFrames& frames )
{
  if ( channels_ != frames.channels() ) {
    errorString_ << "RtDuplex::tickFrame(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( stopped_ )
    start();

  unsigned long j, index = 0;
  unsigned long k, hop = frames.frames();
  for (unsigned int i=0; i<frames.frames(); i++ ) {
    if (counter_ == 0) {
      try {
        audio_->tickStream();
      }
      catch (RtError &error) {
        handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
      }
    }

    if ( channels_ > 1 && frames.interleaved() == false ) {
      k = i;
      for (j=0; j<channels_; j++) {
        lastOutput_[j] = data_[index];
        data_[index++] = frames[k];
        frames[k] = lastOutput_[j];
        k += hop;
      }
    }
    else {
      for (j=0; j<channels_; j++) {
        lastOutput_[j] = data_[index];
        data_[index] = frames[index];
        frames[index++] = lastOutput_[j];
      }
    }

    counter_++;
    if (counter_ >= (long) bufferSize_)
      counter_ = 0;
  }

  return frames;
}
