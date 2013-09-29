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

#ifndef STK_RTDUPLEX_H
#define STK_RTDUPLEX_H

#include "Stk.h"
#include "RtAudio.h"

class RtDuplex : public Stk
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
  RtDuplex(int nChannels = 1, StkFloat sampleRate = Stk::sampleRate(), int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 2);

  //! Class destructor.
  ~RtDuplex();

  //! Start the audio input/output stream.
  /*!
    The stream is started automatically, if necessary, when a tick() or tickFrame method is called.
  */
  void start(void);

  //! Stop the audio input/output stream.
  /*!
    It may be necessary to use this method to avoid audio overflow/underflow problems if you wish to temporarily stop the audio stream.
  */
  void stop(void);

  //! Return the average across the last output sample frame.
  StkFloat lastOut(void) const;

  //! Output a single sample to all channels in a sample frame and return the average across one new input sample frame of data.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  StkFloat tick(const StkFloat sample);

  //! Output each sample in \e vector to all channels per frame and return averaged input sample frames of new data in \e vector.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Output a channel of the StkFrames object to all channels and replace with averaged sample frames of input.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if an
    error occurs during input/outpu or the \c channel argument is zero
    or it is greater than the number of channels in the StkFrames
    object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Return a pointer to the last output sample frame.
  const StkFloat *lastFrame(void) const;

  //! Output sample \e frames from \e frameVector and return new input frames in \e frameVector.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  StkFloat *tickFrame(StkFloat *frameVector, unsigned int frames = 1);

  //! Output the StkFrames data and replace with new input frames.
  /*!
    An StkError will be thrown if an error occurs during
    input/output or if there is an incompatability between the number
    of channels in the RtDuplex object and that in the StkFrames
    object.
  */
  StkFrames& tickFrame( StkFrames& frames );

protected:

	RtAudio *audio_;
  StkFloat *data_;
  StkFloat *lastOutput_;
  int bufferSize_;
  bool stopped_;
  long counter_;
  unsigned int channels_;

};

#endif
