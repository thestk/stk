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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__VOICFORM_H)
#define __VOICFORM_H

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
  //! Class constructor, taking the lowest desired playing frequency.
  VoicForm();

  //! Class destructor.
  ~VoicForm();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set instrument parameters for the given phoneme.  Returns FALSE if phoneme not found.
  bool setPhoneme(const char* phoneme);

  //! Set the voiced component gain.
  void setVoiced(MY_FLOAT vGain);

  //! Set the unvoiced component gain.
  void setUnVoiced(MY_FLOAT nGain);

  //! Set the sweep rate for a particular formant filter (0-3).
  void setFilterSweepRate(int whichOne, MY_FLOAT rate);

  //! Set voiced component pitch sweep rate.
  void setPitchSweepRate(MY_FLOAT rate);

  //! Start the voice.
  void speak();

  //! Stop the voice.
  void quiet();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

protected:
  SingWave *voiced;
  Noise    *noise;
  Envelope *noiseEnv;
  FormSwep  *filters[4];
  OnePole  *onepole;
  OneZero  *onezero;

};

#endif
