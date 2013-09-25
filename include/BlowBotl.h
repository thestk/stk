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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOTTLE_H)
#define __BOTTLE_H

#include "Instrmnt.h"
#include "JetTabl.h"
#include "BiQuad.h"
#include "PoleZero.h"
#include "Noise.h"
#include "ADSR.h"
#include "WaveLoop.h"

class BlowBotl : public Instrmnt
{
 public:
  //! Class constructor.
  BlowBotl();

  //! Class destructor.
  ~BlowBotl();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath velocity with given rate of decrease.
  void stopBlowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:  
  JetTabl *jetTable;
  BiQuad *resonator;
  PoleZero *dcBlock;
  Noise *noise;
  ADSR *adsr;
  WaveLoop *vibrato;
  MY_FLOAT maxPressure;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT outputGain;

};

#endif
