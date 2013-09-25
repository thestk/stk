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

#if !defined(__RTWVIN_H)
#define __RTWVIN_H

#include "Stk.h"
#include "WvIn.h"
#include "RtAudio.h"

class RtWvIn : protected WvIn
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
  RtWvIn(int nChannels = 1, MY_FLOAT sampleRate = Stk::sampleRate(), int device = 0, int bufferFrames = RT_BUFFER_SIZE, int nBuffers = 2);

  //! Class destructor.
  ~RtWvIn();

  //! Start the audio input stream.
  /*!
    The stream is started automatically, if necessary, when a tick() or tickFrame method is called.
  */
  void start(void);

  //! Stop the audio input stream.
  /*!
    It may be necessary to use this method to avoid audio underflow problems if you wish to temporarily stop audio input.
  */
  void stop(void);

  //! Return the average across the last output sample frame.
  MY_FLOAT lastOut(void) const;

  //! Read out the average across one sample frame of data.
  /*!
    An StkError will be thrown if an error occurs during input.
  */
  MY_FLOAT tick(void);

  //! Read out vectorSize averaged sample frames of data in \e vector.
  /*!
    An StkError will be thrown if an error occurs during input.
  */
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return a pointer to the last output sample frame.
  const MY_FLOAT *lastFrame(void) const;

  //! Return a pointer to the next sample frame of data.
  /*!
    An StkError will be thrown if an error occurs during input.
  */
  const MY_FLOAT *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  /*!
    An StkError will be thrown if an error occurs during input.
  */
  MY_FLOAT *tickFrame(MY_FLOAT *frameVector, unsigned int frames);

protected:

	RtAudio *audio;
  bool stopped;
  int stream;
  long counter;

};

#endif
