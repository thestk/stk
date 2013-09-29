/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SINGWAVE_H)
#define __SINGWAVE_H

#include "WaveLoop.h"
#include "Modulate.h"
#include "Envelope.h"

class SingWave : public Stk
{
 public:
  //! Class constructor taking filename argument.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  SingWave(const char *fileName, bool raw=FALSE);

  //! Class destructor.
  ~SingWave();

  //! Reset file to beginning.
  void reset();

  //! Normalize the file to a maximum of +-1.0.
  void normalize();

  //! Normalize the file to a maximum of \e +- peak.
  void normalize(MY_FLOAT peak);

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the vibrato frequency in Hz.
  void setVibratoRate(MY_FLOAT aRate);

  //! Set the vibrato gain.
  void setVibratoGain(MY_FLOAT gain);

  //! Set the random-ness amount.
  void setRandomGain(MY_FLOAT gain);

  //! Set the sweep rate.
  void setSweepRate(MY_FLOAT aRate);

  //! Set the gain rate.
  void setGainRate(MY_FLOAT aRate);    

  //! Set the gain target value.
  void setGainTarget(MY_FLOAT target);

  //! Start a note.
  void noteOn();

  //! Stop a note.
  void noteOff();

  //! Return the last output value.
  MY_FLOAT lastOut();

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:

  WaveLoop *wave;
  Modulate *modulator;
  Envelope *envelope;
  Envelope *pitchEnvelope;
  MY_FLOAT rate;
  MY_FLOAT sweepRate;
  MY_FLOAT lastOutput;

};

#endif
