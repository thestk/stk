/***************************************************/
/*! \class Flute
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FLUTE_H)
#define __FLUTE_H

#include "Instrmnt.h"
#include "JetTabl.h"
#include "DelayL.h"
#include "OnePole.h"
#include "PoleZero.h"
#include "Noise.h"
#include "ADSR.h"
#include "WaveLoop.h"

class Flute : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Flute(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Flute();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the reflection coefficient for the jet delay (-1.0 - 1.0).
  void setJetReflection(MY_FLOAT coefficient);

  //! Set the reflection coefficient for the air column delay (-1.0 - 1.0).
  void setEndReflection(MY_FLOAT coefficient);

  //! Set the length of the jet delay in terms of a ratio of jet delay to air column delay lengths.
  void setJetDelay(MY_FLOAT aRatio);

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
  DelayL *jetDelay;
  DelayL *boreDelay;
  JetTabl *jetTable;
  OnePole *filter;
  PoleZero *dcBlock;
  Noise *noise;
  ADSR *adsr;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT lastFrequency;
  MY_FLOAT maxPressure;
  MY_FLOAT jetReflection;
  MY_FLOAT endReflection;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;
  MY_FLOAT outputGain;
  MY_FLOAT jetRatio;

};

#endif
