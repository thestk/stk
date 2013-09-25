/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument contains up to 5 attack waves,
    5 looped waves, and an ADSR envelope.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__SAMPLER_H)
#define __SAMPLER_H

#include "Instrmnt.h"
#include "ADSR.h"
#include "WvIn.h"
#include "WaveLoop.h"
#include "OnePole.h"

class Sampler : public Instrmnt
{
 public:
  //! Default constructor.
  Sampler();

  //! Class destructor.
  virtual ~Sampler();

  //! Reset and clear all internal state.
  void clear();

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency) = 0;

  //! Initiate the envelopes with a key-on event and reset the attack waves.
  void keyOn();

  //! Signal a key-off event to the envelopes.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude);

  //! Compute one output sample.
  virtual MY_FLOAT tick();

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value) = 0;

 protected:  
  ADSR     *adsr; 
  WvIn     *attacks[5];
  WaveLoop *loops[5];
  OnePole  *filter;
  MY_FLOAT baseFrequency;
  MY_FLOAT attackRatios[5];
  MY_FLOAT loopRatios[5];
  MY_FLOAT attackGain;
  MY_FLOAT loopGain;
  int whichOne;

};

#endif
