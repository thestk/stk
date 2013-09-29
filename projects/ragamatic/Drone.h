#ifndef STK_DRONE_H
#define STK_DRONE_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "OneZero.h"
#include "ADSR.h"
#include "Noise.h"

namespace stk {

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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

class Drone : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Drone( StkFloat lowestFrequency = 20 );

  //! Class destructor.
  ~Drone( void );

  //! Reset and clear all internal state.
  void clear( void );

  //! Set instrument parameters for a particular frequency.
  void setFrequency( StkFloat frequency );

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck( StkFloat amplitude );

  //! Start a note with the given frequency and amplitude.
  void noteOn( StkFloat frequency, StkFloat amplitude );

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff( StkFloat amplitude );

  //! Compute and return one output sample.
  StkFloat tick( unsigned int channel = 0 );

 protected:

  DelayA   delayLine_;
  OneZero  loopFilter_;
  ADSR     envelope_;
  Noise    noise_;
  StkFloat loopGain_;
  unsigned long length_;

};

inline StkFloat Drone :: tick( unsigned int )
{
  // Here's the whole inner loop of the instrument!!
  lastFrame_[0] = delayLine_.tick( loopFilter_.tick( delayLine_.lastOut() * loopGain_ )
                                 + ( 0.005 * envelope_.tick() * noise_.tick() ) );
  return lastFrame_[0];
}

} // stk namespace

#endif

