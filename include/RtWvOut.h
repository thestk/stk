/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to RtAudio for realtime
    audio output.  It is a subclass of WvOut.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    into which data is written.  This class should not be used when
    low-latency is desired.

    RtWvOut supports multi-channel data in interleaved format.  It is
    important to distinguish the tick() methods, which output single
    samples to all channels in a sample frame, from the tickFrame()
    method, which take a pointer or reference to multi-channel sample
    frame data.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_RTWVOUT_H
#define STK_RTWVOUT_H

#include "WvOut.h"
#include "RtAudio.h"

class RtWvOut : public WvOut
{
 public:

  //! Default constructor.
  /*!
    The \e device argument is passed to RtAudio during
    instantiation.  The default value (zero) will select the default
    device on your system.  On systems with multiple
    soundcards/devices, values greater than zero can be specified in
    accordance with the order that the devices are enumerated by the
    underlying audio API.  The default buffer size of RT_BUFFER_SIZE
    is defined in Stk.h.  An StkError will be thrown if an error
    occurs duing instantiation.
  */
  RtWvOut( unsigned int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(),
           int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );

  //! Class destructor.
  ~RtWvOut();

  //! Start the audio output stream.
  /*!
    The stream is started automatically, if necessary, when a
    tick() or tickFrame method is called.
  */
  void start( void );

  //! Stop the audio output stream.
  /*!
    It may be necessary to use this method to avoid undesireable
    audio buffer cycling if you wish to temporarily stop audio output.
  */
  void stop( void );

  // This function is not intended for general use but had to be made
  // public for access from the audio callback function.
  int readBuffer( void *buffer, unsigned int frameCount );

 protected:

  void computeSample( const StkFloat sample );
  void computeFrames( const StkFrames& frames );

  RtAudio dac_;
  bool stopped_;
  unsigned int readIndex_;
  unsigned int writeIndex_;
  long framesFilled_;
  unsigned int status_; // running = 0, emptying buffer = 1, finished = 2

};

#endif
