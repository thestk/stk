/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__CLARINET_H)
#define __CLARINET_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "ReedTabl.h"
#include "OneZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "WaveLoop.h"

class Clarinet : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Clarinet(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Clarinet();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude, MY_FLOAT rate);

  //! Decrease breath pressure with given rate of decrease.
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
  DelayL *delayLine;
  ReedTabl *reedTable;
  OneZero *filter;
  Envelope *envelope;
  Noise *noise;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT outputGain;
  MY_FLOAT noiseGain;
  MY_FLOAT vibratoGain;

};

#endif
