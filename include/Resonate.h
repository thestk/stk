/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_RESONATE_H
#define STK_RESONATE_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "BiQuad.h"
#include "Noise.h"

class Resonate : public Instrmnt
{
 public:
  //! Class constructor.
  Resonate();

  //! Class destructor.
  ~Resonate();

  //! Reset and clear all internal state.
  void clear();

  //! Set the filter for a resonance at the given frequency (Hz) and radius.
  void setResonance(StkFloat frequency, StkFloat radius);

  //! Set the filter for a notch at the given frequency (Hz) and radius.
  void setNotch(StkFloat frequency, StkFloat radius);

  //! Set the filter zero coefficients for contant resonance gain.
  void setEqualGainZeroes();

  //! Initiate the envelope with a key-on event.
  void keyOn();

  //! Signal a key-off event to the envelope.
  void keyOff();

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:

  StkFloat computeSample( void );

  ADSR     adsr_;
  BiQuad   filter_;
  Noise    noise_;
  StkFloat poleFrequency_;
  StkFloat poleRadius_;
  StkFloat zeroFrequency_;
  StkFloat zeroRadius_;

};

#endif
