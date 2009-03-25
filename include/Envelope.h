/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_ENVELOPE_H
#define STK_ENVELOPE_H

#include "Generator.h"

class Envelope : public Generator
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
  void setRate(StkFloat rate);

  //! Set the \e rate based on a time duration.
  void setTime(StkFloat time);

  //! Set the target value.
  virtual void setTarget(StkFloat target);

  //! Set current and target values to \e aValue.
  virtual void setValue(StkFloat value);

  //! Return the current envelope \e state (0 = at target, 1 otherwise).
  virtual int getState(void) const;

  //! Return one envelope output value.
  virtual StkFloat tick(void);

  //! Compute \e vectorSize outputs and return them in \e vector.
  virtual StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:
  StkFloat value_;
  StkFloat target_;
  StkFloat rate_;
  int state_;
};

#endif
