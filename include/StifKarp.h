/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_STIFKARP_H
#define STK_STIFKARP_H

#include "Instrmnt.h"
#include "DelayL.h"
#include "DelayA.h"
#include "OneZero.h"
#include "Noise.h"
#include "BiQuad.h"

class StifKarp : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  StifKarp(StkFloat lowestFrequency);

  //! Class destructor.
  ~StifKarp();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(StkFloat frequency);

  //! Set the stretch "factor" of the string (0.0 - 1.0).
  void setStretch(StkFloat stretch);

  //! Set the pluck or "excitation" position along the string (0.0 - 1.0).
  void setPickupPosition(StkFloat position);

  //! Set the base loop gain.
  /*!
    The actual loop gain is set according to the frequency.
    Because of high-frequency loop filter roll-off, higher
    frequency settings have greater loop gains.
  */
  void setBaseLoopGain(StkFloat aGain);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(StkFloat amplitude);

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
    DelayA  delayLine_;
    DelayL  combDelay_;
    OneZero filter_;
    Noise   noise_;
    BiQuad  biquad_[4];

    unsigned long length_;
    StkFloat loopGain_;
    StkFloat baseLoopGain_;
    StkFloat lastFrequency_;
    StkFloat lastLength_;
    StkFloat stretching_;
    StkFloat pluckAmplitude_;
    StkFloat pickupPosition_;

};

#endif
