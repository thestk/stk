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

#if !defined(__RTDUPLEX_H)
#define __RTDUPLEX_H

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
  RtDuplex(int nChannels = 1, MY_FLOAT sampleRate = Stk::sampleRate(), int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 2);

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
  MY_FLOAT lastOut(void) const;

  //! Output a single sample to all channels in a sample frame and return the average across one new input sample frame of data.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  MY_FLOAT tick(const MY_FLOAT sample);

  //! Output each sample in \vector to all channels per frame and return averaged input sample frames of new data in \e vector.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return a pointer to the last output sample frame.
  const MY_FLOAT *lastFrame(void) const;

  //! Output sample \e frames from \e frameVector and return new input frames in \e frameVector.
  /*!
    An StkError will be thrown if an error occurs during input/output.
  */
  MY_FLOAT *tickFrame(MY_FLOAT *frameVector, unsigned int frames = 1);

protected:

	RtAudio *audio_;
  MY_FLOAT *data_;
  MY_FLOAT *lastOutput_;
  int bufferSize_;
  bool stopped_;
  long counter_;
  unsigned int channels_;

};

#endif
