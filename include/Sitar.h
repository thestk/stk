/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
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

#if !defined(__SITAR_H)
#define __SITAR_H

#include "Instrmnt.h"
#include "DelayA.h"
#include "OneZero.h"
#include "Noise.h"
#include "ADSR.h"

class Sitar : public Instrmnt
{
 public:
  //! Class constructor, taking the lowest desired playing frequency.
  Sitar(MY_FLOAT lowestFrequency);

  //! Class destructor.
  ~Sitar();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  void setFrequency(MY_FLOAT frequency);

  //! Pluck the string with the given amplitude using the current frequency.
  void pluck(MY_FLOAT amplitude);

  //! Start a note with the given frequency and amplitude.
  void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude);

  //! Stop a note with the given amplitude (speed of decay).
  void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  MY_FLOAT tick();

 protected:  
  DelayA *delayLine;
  OneZero *loopFilter;
  Noise *noise;
  ADSR *envelope;
  long length;
  MY_FLOAT loopGain;
  MY_FLOAT amGain;
  MY_FLOAT delay;
  MY_FLOAT targetDelay;

};

#endif

