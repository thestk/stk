/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_VOICFORM_H
#define STK_VOICFORM_H

#include "Instrmnt.h"
#include "Envelope.h"
#include "Noise.h"
#include "SingWave.h"
#include "FormSwep.h"
#include "OnePole.h"
#include "OneZero.h"

class VoicForm : public Instrmnt
{
  public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  VoicForm();

  //! Class destructor.
  ~VoicForm();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set instrument parameters for the given phoneme.  Returns false if phoneme not found.
  bool setPhoneme(const char* phoneme);

  //! Set the voiced component gain.
  void setVoiced(StkFloat vGain);

  //! Set the unvoiced component gain.
  void setUnVoiced(StkFloat nGain);

  //! Set the sweep rate for a particular formant filter (0-3).
  void setFilterSweepRate(unsigned int whichOne, StkFloat rate);

  //! Set voiced component pitch sweep rate.
  void setPitchSweepRate(StkFloat rate);

  //! Start the voice.
  void speak();

  //! Stop the voice.
  void quiet();

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

protected:

  StkFloat computeSample( void );

  SingWave *voiced_;
  Noise    noise_;
  Envelope noiseEnv_;
  FormSwep filters_[4];
  OnePole  onepole_;
  OneZero  onezero_;

};

#endif
