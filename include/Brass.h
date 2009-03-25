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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_BRASS_H
#define STK_BRASS_H

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
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  Brass(StkFloat lowestFrequency);

  //! Class destructor.
  ~Brass();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set the lips frequency.
  void setLip(StkFloat frequency);

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
  DelayA   delayLine_;
  BiQuad   lipFilter_;
  PoleZero dcBlock_;
  ADSR     adsr_;
  WaveLoop *vibrato_;
  unsigned long length_;
  StkFloat lipTarget_;
  StkFloat slideTarget_;
  StkFloat vibratoGain_;
  StkFloat maxPressure_;

};

#endif
