/***************************************************/
/*! \class Drone
    \brief STK "drone" plucked string model.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_DRONE_H
#define STK_DRONE_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "OneZero.h"
#include "ADSR.h"
#include "Noise.h"

class Drone : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Drone( StkFloat lowestFrequency = 20 );

  //! Class destructor.
  ~Drone();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(StkFloat frequency);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(StkFloat amplitude);

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(StkFloat frequency, StkFloat amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(StkFloat amplitude);

  //! Compute one output sample.
  virtual StkFloat tick();

  //! Computer \e vectorSize outputs and return them in \e vector.
  virtual StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:  
  DelayA   delayLine_;
  OneZero  loopFilter_;
  ADSR     envelope_;
  Noise    noise_;
  StkFloat loopGain_;
  unsigned long length_;

};

#endif

