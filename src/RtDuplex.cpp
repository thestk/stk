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
  channels = nChannels;
  bufferSize = bufferFrames;
  RtAudio::RTAUDIO_FORMAT format = ( sizeof(MY_FLOAT) == 8 ) ? RtAudio::RTAUDIO_FLOAT64 : RtAudio::RTAUDIO_FLOAT32;
  try {
    audio = new RtAudio(&stream, device, channels, device, channels, format,
                        (int)sampleRate, &bufferSize, nBuffers);
    data = (MY_FLOAT *) audio->getStreamBuffer(stream);
  }
  catch (RtError &error) {
    handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
  }

  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  for (unsigned int i=0; i<channels; i++) lastOutput[i] = 0.0;
  counter = 0;
  stopped = true;
}

RtDuplex :: ~RtDuplex()
{
  if ( !stopped )
    audio->stopStream(stream);
  delete audio;
  delete [] lastOutput;
  data = 0; // RtAudio deletes the buffer itself.
}

void RtDuplex :: start()
{
  if ( stopped ) {
    audio->startStream(stream);
    stopped = false;
  }
}

void RtDuplex :: stop()
{
  if ( !stopped ) {
    audio->stopStream(stream);
    stopped = true;
  }
}

MY_FLOAT RtDuplex :: lastOut(void) const
{
  if ( channels == 1 )
    return *lastOutput;

  MY_FLOAT output = 0.0;
  for (unsigned int i=0; i<channels; i++ ) {
    output += lastOutput[i];
  }
  return output / channels;
}

MY_FLOAT RtDuplex :: tick(const MY_FLOAT sample)
{
  if ( stopped )
    start();

  if (counter == 0) {
    try {
      audio->tickStream(stream);
    }
    catch (RtError &error) {
      handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
    }
  }

  unsigned long temp = counter * channels;
  for (unsigned int i=0; i<channels; i++) {
    lastOutput[i] = data[temp];
    data[temp++] = sample;
  }

  counter++;
  if (counter >= (long) bufferSize)
    counter = 0;

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
  return lastOutput;
}

MY_FLOAT *RtDuplex :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  if ( stopped )
    start();

  unsigned int i;
  unsigned long temp;
  for (unsigned int j=0; j<frames; j++ ) {
    if (counter == 0) {
      try {
        audio->tickStream(stream);
      }
      catch (RtError &error) {
        handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
      }
    }

    temp = counter * channels;
    for (i=0; i<channels; i++) {
      lastOutput[i] = data[temp];
      data[temp++] = frameVector[j*channels+i];
      frameVector[j*channels+i] = lastOutput[i];
    }

    counter++;
    if (counter >= (long) bufferSize)
      counter = 0;
  }

  return frameVector;
}
