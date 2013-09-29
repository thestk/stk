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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_FLUTE_H
#define STK_FLUTE_H

#include "Instrmnt.h"
#include "JetTable.h"
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
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Flute(StkFloat lowestFrequency);

  //! Class destructor.
  ~Flute();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set the reflection coefficient for the jet delay (-1.0 - 1.0).
  void setJetReflection(StkFloat coefficient);

  //! Set the reflection coefficient for the air column delay (-1.0 - 1.0).
  void setEndReflection(StkFloat coefficient);

  //! Set the length of the jet delay in terms of a ratio of jet delay to air column delay lengths.
  void setJetDelay(StkFloat aRatio);

  //! Apply breath velocity to instrument with given amplitude and rate of increase.
  void startBlowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath velocity with given rate of decrease.
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
  DelayL   jetDelay_;
  DelayL   boreDelay_;
  JetTable jetTable_;
  OnePole  filter_;
  PoleZero dcBlock_;
  Noise    noise_;
  ADSR     adsr_;
  WaveLoop *vibrato_;
  unsigned long length_;
  StkFloat lastFrequency_;
  StkFloat maxPressure_;
  StkFloat jetReflection_;
  StkFloat endReflection_;
  StkFloat noiseGain_;
  StkFloat vibratoGain_;
  StkFloat outputGain_;
  StkFloat jetRatio_;

};

#endif
