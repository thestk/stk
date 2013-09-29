/***************************************************/
/*! \class RtWvOut
    \brief STK realtime audio (blocking) output class.

    This class provides a simplified interface to
    RtAudio for realtime audio output.  It is a
    protected subclass of WvOut.  Because this
    class makes use of RtAudio's blocking output
    routines, its performance is less robust on
    systems where the audio API is callback-based
    (Macintosh CoreAudio and Windows ASIO).

    RtWvOut supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which output
    single samples to all channels in a sample
    frame, from the tickFrame() method, which
    takes a pointer to multi-channel sample
    frame data.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_RTWVOUT_H
#define STK_RTWVOUT_H

#include "WvOut.h"
#include "RtAudio.h"

class RtWvOut : protected WvOut
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
  RtWvOut(unsigned int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(), int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 4 );

  //! Class destructor.
  ~RtWvOut();

  //! Start the audio output stream.
  /*!
    The stream is started automatically, if necessary, when a tick() or tickFrame method is called.
  */
  void start(void);

  //! Stop the audio output stream.
  /*!
    It may be necessary to use this method to avoid undesireable audio buffer cycling if you wish to temporarily stop audio output.
  */
  void stop(void);

  //! Return the number of sample frames output.
  unsigned long getFrames( void ) const;

  //! Return the number of seconds of data output.
  StkFloat getTime( void ) const;

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError will be thrown if an error occurs during output.
  */
  void tick( const StkFloat sample );

  //! Output each sample in \e vector to all channels in \e vectorSize sample frames.
  /*!
    An StkError will be thrown if an error occurs during output.
  */
  void tick( const StkFloat *vector, unsigned int vectorSize );

  //! Output a channel of the StkFrames object to all channels of the RtWvOut object.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if an
    error occurs during output or the \c channel argument is zero or
    it is greater than the number of channels in the StkFrames object.
  */
  void tick( const StkFrames& frames, unsigned int channel = 1 );

  //! Output the \e frameVector of sample frames of the given length.
  /*!
    An StkError will be thrown if an error occurs during output.
  */
  void tickFrame( const StkFloat *frameVector, unsigned int frames = 1 );

  //! Output the StkFrames data to the RtWvOut object.
  /*!
    An StkError will be thrown if an error occurs during output or
    if there is an incompatability between the number of channels in
    the WvOut object and that in the StkFrames object.
  */
  void tickFrame( const StkFrames& frames );

 protected:

  RtAudio *audio_;
  StkFloat *dataPtr_;
  bool stopped_;
  int bufferSize_;

};

#endif
