#ifndef STK_BLOWBOTL_H
#define STK_BLOWBOTL_H

#include "Instrmnt.h"
#include "JetTable.h"
#include "BiQuad.h"
#include "PoleZero.h"
#include "Noise.h"
#include "ADSR.h"
#include "SineWave.h"

namespace stk {

/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class BlowBotl : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BlowBotl( void );

  //! Class destructor.
  ~BlowBotl( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat frequency );

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing( StkFloat amplitude, StkFloat rate );

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing( StkFloat rate );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange( int number, StkFloat value );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  JetTable jetTable_;
  BiQuad resonator_;
  PoleZero dcBlock_;
  Noise noise_;
  ADSR adsr_;
  SineWave vibrato_;
  StkFloat maxPressure_;
  StkFloat noiseGain_;
  StkFloat vibratoGain_;
  StkFloat outputGain_;

};

inline StkFloat BlowBotl :: tick( unsigned int )
{
  StkFloat breathPressure;
  StkFloat randPressure;
  StkFloat pressureDiff;

  // Calculate the breath pressure (envelope + vibrato)
  breathPressure = maxPressure_ * adsr_.tick();
  breathPressure += vibratoGain_ * vibrato_.tick();

  pressureDiff = breathPressure - resonator_.lastOut();

  randPressure = noiseGain_ * noise_.tick();
  randPressure *= breathPressure;
  randPressure *= (1.0 + pressureDiff);

  resonator_.tick( breathPressure + randPressure - ( jetTable_.tick( pressureDiff ) * pressureDiff ) );
  lastFrame_[0] = 0.2 * outputGain_ * dcBlock_.tick( pressureDiff );

  return lastFrame_[0];
}

} // stk namespace

#endif
