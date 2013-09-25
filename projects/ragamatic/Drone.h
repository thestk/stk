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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DRONE_H)
#define __DRONE_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "OneZero.h"
#include "ADSR.h"
#include "Noise.h"

class Drone : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Drone(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Drone();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

 protected:  
  DelayA *delayLine;
  OneZero *loopFilter;
  ADSR *envelope;
  Noise *noise;
  long length;
  MY_FLOAT loopGain;

};

#endif

