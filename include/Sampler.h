/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument provides an ADSR envelope, a one-pole filter, and
    structures for an arbitrary number of attack and loop waves.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_SAMPLER_H
#define STK_SAMPLER_H

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
  virtual void setFrequency(StkFloat frequency) = 0;

  //! Initiate the envelopes with a key-on event and reset the attack waves.
  void keyOn();

  //! Signal a key-off event to the envelopes.
  void keyOff();

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(StkFloat amplitude);

  //! Compute one output sample.
  virtual StkFloat tick() = 0;

  //! Computer \e vectorSize outputs and return them in \e vector.
  virtual StkFloat *tick(StkFloat *vector, unsigned int vectorSize) = 0;

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 ) = 0;

  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, StkFloat value) = 0;

 protected:  
  ADSR adsr_;
  std::vector<WvIn *> attacks_;
  std::vector<WaveLoop *> loops_;
  OnePole filter_;
  StkFloat baseFrequency_;
  std::vector<StkFloat> attackRatios_;
  std::vector<StkFloat> loopRatios_;
  StkFloat attackGain_;
  StkFloat loopGain_;

};

#endif
