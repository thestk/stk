/***************************************************/
/*! \class WaveLoop
    \brief STK waveform oscillator class.

    This class inherits from WvIn and provides
    audio file looping functionality.

    WaveLoop supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_WAVELOOP_H
#define STK_WAVELOOP_H

#include "WvIn.h"

class WaveLoop : public WvIn
{
public:
  //! Class constructor.
  WaveLoop( std::string fileName, bool raw = false );

  //! Class destructor.
  virtual ~WaveLoop();

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e aFrequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
   */
  void setFrequency(StkFloat frequency);

  //! Increment the read pointer by \e aTime samples, modulo file size.
  void addTime(StkFloat time);

  //! Increment current read pointer by \e anAngle, relative to a looping frequency.
  /*!
    This function increments the read pointer based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhase(StkFloat angle);

  //! Add a phase offset to the current read pointer.
  /*!
    This function determines a time offset based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhaseOffset(StkFloat angle);

  //! Return a pointer to the next sample frame of data.
  const StkFloat *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  StkFloat *tickFrame(StkFloat *frameVector, unsigned int frames);

  //! Fill the StkFrames object with sample frames of data and return the same reference.
  /*!
    An StkError will be thrown if a file is read incrementally and
    a read error occurs or there is an incompatability between the
    number of channels in the RtWvIn object and that in the StkFrames
    object.
  */
  StkFrames& tickFrame( StkFrames& frames );

protected:

  // Read file data.
  void readData(unsigned long index);

  StkFloat phaseOffset_;

};

#endif
