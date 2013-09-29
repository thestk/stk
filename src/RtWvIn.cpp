/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio input class.

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "RtWvIn.h"

RtWvIn :: RtWvIn(int nChannels, MY_FLOAT sampleRate, int device, int bufferFrames, int nBuffers )
{
  channels = nChannels;
  int size = bufferFrames;
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
    audio_->openStream(0, 0, device, channels, format,
                      (int)sampleRate, &size, nBuffers);
    data = (MY_FLOAT *) audio_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  bufferSize = size;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  for (unsigned int i=0; i<channels; i++) lastOutput[i] = 0.0;
  counter_ = 0;
  stopped_ = true;
}

RtWvIn :: ~RtWvIn()
{
  if ( !stopped_ )
    audio_->stopStream();
  delete audio_;
  data = 0; // RtAudio deletes the buffer itself.
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

MY_FLOAT RtWvIn :: lastOut(void) const
{
  return WvIn::lastOut();
}

MY_FLOAT RtWvIn :: tick(void)
{
  tickFrame();
  return lastOut();
}

MY_FLOAT *RtWvIn :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick();

  return vector;
}

const MY_FLOAT *RtWvIn :: lastFrame() const
{
  return lastOutput;
}

const MY_FLOAT *RtWvIn :: tickFrame(void)
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

  long temp = counter_ * channels;
  for (unsigned int i=0; i<channels; i++)
    lastOutput[i] = data[temp++];

  counter_++;
  if (counter_ >= (long) bufferSize)
    counter_ = 0;

  return lastOutput;
}

MY_FLOAT *RtWvIn :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  return WvIn::tickFrame( frameVector, frames );
}
