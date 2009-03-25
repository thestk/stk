/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to RtAudio for realtime
    audio output.  It is a subclass of WvOut.  Because this class
    makes use of RtAudio's blocking output routines, its performance
    is less robust on systems where the audio API is callback-based
    (Macintosh CoreAudio and Windows ASIO).

    RtWvOut supports multi-channel data in interleaved format.  It is
    important to distinguish the tick() methods, which output single
    samples to all channels in a sample frame, from the tickFrame()
    method, which take a pointer or reference to multi-channel sample
    frame data.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "RtWvOut.h"

RtWvOut :: RtWvOut( unsigned int nChannels, StkFloat sampleRate, int device, int bufferFrames, int nBuffers )
  : stopped_( true ), nChannels_(nChannels), bufferIndex_( 0 ), iBuffer_( 0 )
{
  // We'll let RtAudio deal with channel and sample rate limitations.
  int size = bufferFrames;
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;

  dac_ = 0;
  try {
    dac_ = new RtAudio();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  // Now open a stream and get the buffer pointer.
  try {
    dac_->openStream( device, (int)nChannels, 0, 0, format,
                      (int)sampleRate, &size, nBuffers );
    buffer_ = (StkFloat *) dac_->getStreamBuffer();
  }
  catch (RtError &error) {
    handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
  }

  bufferFrames_ = size;
}

RtWvOut :: ~RtWvOut()
{
  if ( !stopped_ ) dac_->stopStream();
  dac_->closeStream();
  delete dac_;
}

void RtWvOut :: start()
{
  if ( stopped_ ) {
    dac_->startStream();
    stopped_ = false;
  }
}

void RtWvOut :: stop()
{
  if ( !stopped_ ) {
    dac_->stopStream();
    stopped_ = true;
  }
}

void RtWvOut :: incrementFrame( void )
{
  frameCounter_++;
  bufferIndex_++;

  if ( bufferIndex_ == bufferFrames_ ) {
    try {
      dac_->tickStream();
    }
    catch (RtError &error) {
      handleError( error.getMessageString(), StkError::AUDIO_SYSTEM );
    }
    bufferIndex_ = 0;
    iBuffer_ = 0;
  }
}

void RtWvOut :: computeSample( const StkFloat sample )
{
  if ( stopped_ ) start();

  StkFloat input = sample;
  clipTest( input );
  for ( unsigned int j=0; j<nChannels_; j++ )
    buffer_[iBuffer_++] = input;

  this->incrementFrame();
}

void RtWvOut :: computeFrames( const StkFrames& frames )
{
  if ( stopped_ ) start();

  if ( frames.channels() != nChannels_ ) {
    errorString_ << "RtWvOut::computeFrames(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned int j;
  if ( nChannels_ == 1 || frames.interleaved() ) {

    unsigned int iFrames = 0;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      for ( j=0; j<nChannels_; j++ ) {
        buffer_[iBuffer_] = frames[iFrames++];
        clipTest( buffer_[iBuffer_++] );
      }

      this->incrementFrame();
    }
  }
  else { // non-interleaved frames

    unsigned long hop = frames.frames();
    unsigned int index;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      index = i;
      for ( j=0; j<nChannels_; j++ ) {
        buffer_[iBuffer_] = frames[index];
        clipTest( buffer_[iBuffer_++] );
        index += hop;
      }

      this->incrementFrame();
    }
  }
}



