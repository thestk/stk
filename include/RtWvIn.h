/***************************************************/
/*! \class RtWvIn
    \brief STK realtime audio (blocking) input class.

    This class provides a simplified interface to RtAudio for realtime
    audio input.  It is a subclass of WvIn.  This class makes use of
    RtAudio's callback functionality by creating a large ring-buffer
    from which data is read.  This class should not be used when
    low-latency is desired.

    RtWvIn supports multi-channel data in both interleaved and
    non-interleaved formats.  It is important to distinguish the
    tick() methods, which return samples produced by averaging across
    sample frames, from the tickFrame() methods, which return
    references or pointers to multi-channel sample frames.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_RTWVIN_H
#define STK_RTWVIN_H

#include "WvIn.h"
#include "RtAudio.h"

class RtWvIn : public WvIn
{
public:
  //! Default constructor.
  /*!
    The \e device argument is passed to RtAudio during
    instantiation.  The default value (zero) will select the default
    device on your system or the first device found meeting the
    specified parameters.  On systems with multiple
    soundcards/devices, values greater than zero can be specified in
    accordance with the order that the devices are enumerated by the
    underlying audio API.  The default buffer size of RT_BUFFER_SIZE
    is defined in Stk.h.  An StkError will be thrown if an error
    occurs duing instantiation.
  */
  RtWvIn( unsigned int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(), int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 20 );

  //! Class destructor.
  ~RtWvIn();

  //! Start the audio input stream.
  /*!
    The stream is started automatically, if necessary, when a
    tick() or tickFrame() method is called.
  */
  void start( void );

  //! Stop the audio input stream.
  /*!
    It may be necessary to use this method to avoid audio underflow
    problems if you wish to temporarily stop audio input.
  */
  void stop( void );

  // This function is not intended for general use but had to be made
  // public for access from the audio callback function.
  void fillBuffer( void *buffer, unsigned int nFrames );

protected:

  void computeFrame( void );

	RtAudio adc_;
  bool stopped_;
  unsigned int readIndex_;
  unsigned int writeIndex_;
  unsigned int framesFilled_;

};

#endif
