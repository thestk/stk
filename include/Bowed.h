/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BOWED_H)
#define __BOWED_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "BowTabl.h"
#include "OnePole.h"
#include "BiQuad.h"
#include "WaveLoop.h"
#include "ADSR.h"

class Bowed : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Bowed(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Bowed();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set vibrato gain.
  void setVibrato(MY_FLOAT gain);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBowing(MY_FLOAT rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, MY_FLOAT value);

 protected:  
  DelayL *neckDelay;
  DelayL *bridgeDelay;
  BowTabl *bowTable;
  OnePole *stringFilter;
  BiQuad *bodyFilter;
  WaveLoop *vibrato;
  ADSR *adsr;
  MY_FLOAT maxVelocity;
  MY_FLOAT baseDelay;
  MY_FLOAT vibratoGain;
  MY_FLOAT betaRatio;

};

#endif
