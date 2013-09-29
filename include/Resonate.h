#ifndef STK_RESONATE_H
#define STK_RESONATE_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "BiQuad.h"
#include "Noise.h"

namespace stk {

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class Resonate : public Instrmnt
{
 public:
  //! Class constructor.
  Resonate( void );

  //! Class destructor.
  ~Resonate( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set the filter for a resonance at the given frequency (Hz) and radius.
  void setResonance( StkFloat frequency, StkFloat radius );

  //! Set the filter for a notch at the given frequency (Hz) and radius.
  void setNotch( StkFloat frequency, StkFloat radius );

  //! Set the filter zero coefficients for contant resonance gain.
  void setEqualGainZeroes( void ) { filter_.setEqualGainZeroes(); };

  //! Initiate the envelope with a key-on event.
  void keyOn( void ) { adsr_.keyOn(); };

  //! Signal a key-off event to the envelope.
  void keyOff( void ) { adsr_.keyOff(); };

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  ADSR     adsr_;
  BiQuad   filter_;
  Noise    noise_;
  StkFloat poleFrequency_;
  StkFloat poleRadius_;
  StkFloat zeroFrequency_;
  StkFloat zeroRadius_;

};

inline StkFloat Resonate :: tick( unsigned int )
{
  lastFrame_[0] = filter_.tick( noise_.tick() );
  lastFrame_[0] *= adsr_.tick();
  return lastFrame_[0];
}

} // stk namespace

#endif
