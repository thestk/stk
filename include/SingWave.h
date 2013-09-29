/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_SINGWAVE_H
#define STK_SINGWAVE_H

#include "WaveLoop.h"
#include "Modulate.h"
#include "Envelope.h"

class SingWave : public Generator
{
 public:
  //! Class constructor taking filename argument.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, a read error occurs, or the rawwave path is incorrectly set.
  */
  SingWave( std::string fileName, bool raw = false);

  //! Class destructor.
  ~SingWave();

  //! Reset file to beginning.
  void reset();

  //! Normalize the file to a maximum of +-1.0.
  void normalize();

  //! Normalize the file to a maximum of \e +- peak.
  void normalize(StkFloat peak);

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set the vibrato frequency in Hz.
  void setVibratoRate(StkFloat rate);

  //! Set the vibrato gain.
  void setVibratoGain(StkFloat gain);

  //! Set the random-ness amount.
  void setRandomGain(StkFloat gain);

  //! Set the sweep rate.
  void setSweepRate(StkFloat rate);

  //! Set the gain rate.
  void setGainRate(StkFloat rate);    

  //! Set the gain target value.
  void setGainTarget(StkFloat target);

  //! Start a note.
  void noteOn();

  //! Stop a note.
  void noteOff();

 protected:

  StkFloat computeSample( void );

  WaveLoop *wave_;
  Modulate modulator_;
  Envelope envelope_;
  Envelope pitchEnvelope_;
  StkFloat rate_;
  StkFloat sweepRate_;

};

#endif
