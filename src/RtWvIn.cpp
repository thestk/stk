/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to RtAudio for realtime
    audio input.  It is a protected subclass of WvIn.  Because this
    class makes use of RtAudio's blocking output routines, its
    performance is less robust on systems where the audio API is
    callback-based (Macintosh CoreAudio and Windows ASIO).

    RtWvIn supports multi-channel data in interleaved format.  It is
    important to distinguish the tick() methods, which return samples
    produced by averaging across sample frames, from the tickFrame()
    methods, which return references or pointers to multi-channel
    sample frames.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "RtWvIn.h"

RtWvIn :: RtWvIn( unsigned int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), bufferIndex_( 0 )
{
  int size = bufferFrames;
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  adc_ = 0;
  try {
    adc_ = new RtAudio();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  // Allocate the adc and get the buffer pointer.
  try {
    adc_->openStream( 0, 0, device, (int)nChannels, format,
                        (int)sampleRate, &size, nBuffers );
    buffer_ = (StkFloat *) adc_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  bufferFrames_ = size;
  lastOutputs_.resize( 1, nChannels );
}

RtWvIn :: ~RtWvIn()
{
  if ( !stopped_ ) adc_->stopStream();
  adc_->closeStream();
  delete adc_;
}

void RtWvIn :: start()
{
  if ( stopped_ ) {
    adc_->startStream();
    stopped_ = false;
  }
}

void RtWvIn :: stop()
{
  if ( !stopped_ ) {
    adc_->stopStream();
    stopped_ = true;
  }
}

void RtWvIn :: computeFrame( void )
{
  if ( stopped_ ) this->start();

  if ( bufferIndex_ == 0) {
    try {
      adc_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
  }

  long iBuffer = bufferIndex_ * lastOutputs_.size();
  for (unsigned int i=0; i<lastOutputs_.size(); i++)
    lastOutputs_[i] = buffer_[iBuffer++];

  bufferIndex_++;
  if ( bufferIndex_ >= bufferFrames_ )
    bufferIndex_ = 0;
}
