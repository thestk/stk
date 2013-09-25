/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__BRASS_H)
#define __BRASS_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "BiQuad.h"
#include "PoleZero.h"
#include "ADSR.h"
#include "WaveLoop.h"

class Brass: public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Brass(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Brass();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Set the lips frequency.
  void setLip(MY_FLOAT frequency);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(MY_FLOAT amplitude,MY_FLOAT rate);

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
  DelayA *delayLine;
  BiQuad *lipFilter;
  PoleZero *dcBlock;
  ADSR *adsr;
  WaveLoop *vibrato;
  long length;
  MY_FLOAT lipTarget;
  MY_FLOAT slideTarget;
  MY_FLOAT vibratoGain;
  MY_FLOAT maxPressure;

};

#endif
