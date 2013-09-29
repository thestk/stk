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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_BLOWBOTL_H
#define STK_BLOWBOTL_H

#include "Instrmnt.h"
#include "JetTable.h"
#include "BiQuad.h"
#include "PoleZero.h"
#include "Noise.h"
#include "ADSR.h"
#include "SineWave.h"

class BlowBotl : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BlowBotl();

  //! Class destructor.
  ~BlowBotl();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(StkFloat rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:

  StkFloat computeSample( void );
  
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

#endif
