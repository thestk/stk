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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_CLARINET_H
#define STK_CLARINET_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "ReedTable.h"
#include "OneZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "WaveLoop.h"

class Clarinet : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Clarinet(StkFloat lowestFrequency);

  //! Class destructor.
  ~Clarinet();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(StkFloat rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Compute one output sample.
  StkFloat tick();

  //! Computer \e vectorSize outputs and return them in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:
  DelayL delayLine_;
  ReedTable reedTable_;
  OneZero filter_;
  Envelope envelope_;
  Noise noise_;
  WaveLoop *vibrato_;
  long length_;
  StkFloat outputGain_;
  StkFloat noiseGain_;
  StkFloat vibratoGain_;

};

#endif
