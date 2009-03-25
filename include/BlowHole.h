/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_BLOWHOLE_H
#define STK_BLOWHOLE_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "ReedTable.h"
#include "OneZero.h"
#include "PoleZero.h"
#include "Envelope.h"
#include "Noise.h"
#include "SineWave.h"

class BlowHole : public Instrmnt
{
 public:
  //! Class constructor.
  /*!
    An StkError will be thrown if the rawwave path is incorrectly set.
  */
  BlowHole(StkFloat lowestFrequency);

  //! Class destructor.
  ~BlowHole();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set the tonehole state (0.0 = closed, 1.0 = fully open).
  void setTonehole(StkFloat newValue);

  //! Set the register hole state (0.0 = closed, 1.0 = fully open).
  void setVent(StkFloat newValue);

  //! Apply breath pressure to instrument with given amplitude and rate of increase.
  void startBlowing(StkFloat amplitude, StkFloat rate);

  //! Decrease breath pressure with given rate of decrease.
  void stopBlowing(StkFloat rate);

  //! Start a note with the given frequency and amplitude.
  void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(StkFloat amplitude);

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  void controlChange(int number, StkFloat value);

 protected:

  StkFloat computeSample( void );

  DelayL    delays_[3];
  ReedTable reedTable_;
  OneZero   filter_;
  PoleZero  tonehole_;
  PoleZero  vent_;
  Envelope  envelope_;
  Noise     noise_;
  SineWave vibrato_;
  unsigned long length_;
  StkFloat scatter_;
  StkFloat thCoeff_;
  StkFloat rhGain_;
  StkFloat outputGain_;
  StkFloat noiseGain_;
  StkFloat vibratoGain_;

};

#endif
