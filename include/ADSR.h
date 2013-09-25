/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ADSR_H)
#define __ADSR_H

#include "Envelope.h"

class ADSR : public Envelope
{
 public:

  //! Envelope states.
  enum { ATTACK, DECAY, SUSTAIN, RELEASE, DONE };

  //! Default constructor.
  ADSR(void);

  //! Class destructor.
  ~ADSR(void);

  //! Set target = 1, state = \e ADSR::ATTACK.
  void keyOn(void);

  //! Set target = 0, state = \e ADSR::RELEASE.
  void keyOff(void);

  //! Set the attack rate.
  void setAttackRate(MY_FLOAT aRate);

  //! Set the decay rate.
  void setDecayRate(MY_FLOAT aRate);

  //! Set the sustain level.
  void setSustainLevel(MY_FLOAT aLevel);

  //! Set the release rate.
  void setReleaseRate(MY_FLOAT aRate);

  //! Set the attack rate based on a time duration.
  void setAttackTime(MY_FLOAT aTime);

  //! Set the decay rate based on a time duration.
  void setDecayTime(MY_FLOAT aTime);

  //! Set the release rate based on a time duration.
  void setReleaseTime(MY_FLOAT aTime);

  //! Set sustain level and attack, decay, and release state rates based on time durations.
  void setAllTimes(MY_FLOAT aTime, MY_FLOAT dTime, MY_FLOAT sLevel, MY_FLOAT rTime);

  //! Set the target value.
  void setTarget(MY_FLOAT aTarget);

  //! Return the current envelope \e state (ATTACK, DECAY, SUSTAIN, RELEASE, DONE).
  int getState(void) const;

  //! Set to state = ADSR::SUSTAIN with current and target values of \e aValue.
  void setValue(MY_FLOAT aValue);

  //! Return one envelope output value.
  MY_FLOAT tick(void);

  //! Return \e vectorSize envelope outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:  
  MY_FLOAT attackRate;
  MY_FLOAT decayRate;
  MY_FLOAT sustainLevel;
  MY_FLOAT releaseRate;
};

#endif
