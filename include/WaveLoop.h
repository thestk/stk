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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__WAVELOOP_H)
#define __WAVELOOP_H

#include "WvIn.h"
#include <stdio.h>

class WaveLoop : public WvIn
{
public:
  //! Class constructor.
  WaveLoop( const char *fileName, bool raw = FALSE );

  //! Class destructor.
  virtual ~WaveLoop();

  //! Set the data interpolation rate based on a looping frequency.
  /*!
    This function determines the interpolation rate based on the file
    size and the current Stk::sampleRate.  The \e aFrequency value
    corresponds to file cycles per second.  The frequency can be
    negative, in which case the loop is read in reverse order.
   */
  void setFrequency(MY_FLOAT aFrequency);

  //! Increment the read pointer by \e aTime samples, modulo file size.
  void addTime(MY_FLOAT aTime);

  //! Increment current read pointer by \e anAngle, relative to a looping frequency.
  /*!
    This function increments the read pointer based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhase(MY_FLOAT anAngle);

  //! Add a phase offset to the current read pointer.
  /*!
    This function determines a time offset based on the file
    size and the current Stk::sampleRate.  The \e anAngle value
    is a multiple of file size.
   */
  void addPhaseOffset(MY_FLOAT anAngle);

  //! Return a pointer to the next sample frame of data.
  const MY_FLOAT *tickFrame(void);

protected:

  // Read file data.
  void readData(unsigned long index);

  MY_FLOAT phaseOffset;

};

#endif // defined(__WAVELOOP_H)
