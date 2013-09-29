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
#include <stdio.h>

RtWvOut :: RtWvOut(unsigned int nChannels, MY_FLOAT sampleRate, int device, int bufferFrames, int nBuffers )
{
  // We'll let RtAudio deal with channel and srate limitations.
  channels = nChannels;
  bufferSize = bufferFrames;
  RtAudio::RTAUDIO_FORMAT format = ( sizeof(MY_FLOAT) == 8 ) ? RtAudio::RTAUDIO_FLOAT64 : RtAudio::RTAUDIO_FLOAT32;
  try {
    audio = new RtAudio(&stream, device, (int)channels, 0, 0, format,
                        (int)sampleRate, &bufferSize, nBuffers);
    data = (MY_FLOAT *) audio->getStreamBuffer(stream);
  }
  catch (RtError &error) {
    handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
  }
  stopped = true;
}

RtWvOut :: ~RtWvOut()
{
  if ( !stopped )
    audio->stopStream(stream);
  delete audio;
  data = 0; // RtAudio deletes the buffer itself.
}

void RtWvOut :: start()
{
  if ( stopped ) {
    audio->startStream(stream);
    stopped = false;
  }
}

void RtWvOut :: stop()
{
  if ( !stopped ) {
    audio->stopStream(stream);
    stopped = true;
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
  if ( stopped )
    start();
  
  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter >= (unsigned int )bufferSize ) {
    try {
      audio->tickStream(stream);
    }
    catch (RtError &error) {
      handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
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
  if ( stopped )
    start();

  for ( unsigned int i=0; i<frames; i++ ) {
    for ( unsigned int j=0; j<channels; j++ ) {
      data[counter*channels+j] = frameVector[i*channels+j];
    }
    counter++;

    if ( counter >= (unsigned int)bufferSize ) {
      try {
        audio->tickStream(stream);
      }
      catch (RtError &error) {
        handleError( error.getMessage(), StkError::AUDIO_SYSTEM );
      }
      counter = 0;
    }
  }
}

