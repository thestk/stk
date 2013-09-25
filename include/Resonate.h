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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__RESONATE_H)
#define __RESONATE_H

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
  void setResonance(MY_FLOAT frequency, MY_FLOAT radius);

  //! Set the filter for a notch at the given frequency (Hz) and radius.
  void setNotch(MY_FLOAT frequency, MY_FLOAT radius);

  //! Set the filter zero coefficients for contant resonance gain.
  void setEqualGainZeroes();

  //! Initiate the envelope with a key-on event.
  void keyOn();

  //! Signal a key-off event to the envelope.
  void keyOff();

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

 protected:  
  ADSR     *adsr;
  BiQuad   *filter;
  Noise    *noise;
  MY_FLOAT poleFrequency;
  MY_FLOAT poleRadius;
  MY_FLOAT zeroFrequency;
  MY_FLOAT zeroRadius;

};

#endif
