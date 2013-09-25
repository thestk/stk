/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ENVELOPE_H)
#define __ENVELOPE_H

#include "Stk.h"

class Envelope : public Stk
{
 public:

  //! Default constructor.
  Envelope(void);

  //! Class destructor.
  virtual ~Envelope(void);

  //! Set target = 1.
  virtual void keyOn(void);

  //! Set target = 0.
  virtual void keyOff(void);

  //! Set the \e rate.
  void setRate(MY_FLOAT aRate);

  //! Set the \e rate based on a time duration.
  void setTime(MY_FLOAT aTime);

  //! Set the target value.
  virtual void setTarget(MY_FLOAT aTarget);

  //! Set current and target values to \e aValue.
  virtual void setValue(MY_FLOAT aValue);

  //! Return the current envelope \e state (0 = at target, 1 otherwise).
  virtual int getState(void) const;

  //! Return one envelope output value.
  virtual MY_FLOAT tick(void);

  //! Return \e vectorSize envelope outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

 protected:
  MY_FLOAT value;
  MY_FLOAT target;
  MY_FLOAT rate;
  int state;
};

#endif
