/***************************************************/
/*! \class Asymp
    \brief STK asymptotic curve envelope class

    This class implements a simple envelope generator
    which asymptotically approaches a target value.
    The algorithm used is of the form:

    x[n] = a x[n-1] + (1-a) target,

    where a = exp(-T/tau), T is the sample period, and
    tau is a time constant.  The user can set the time
    constant (default value = 0.3) and target value.
    Theoretically, this recursion never reaches its
    target, though the calculations in this class are
    stopped when the current value gets within a small
    threshold value of the target (at which time the
    current value is set to the target).  It responds
    to \e keyOn and \e keyOff messages by ramping to
    1.0 on keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_ASYMP_H
#define STK_ASYMP_H

#include "Envelope.h"

const StkFloat TARGET_THRESHOLD = 0.000001;

class Asymp : public Envelope
{
 public:

  //! Default constructor.
  Asymp(void);

  //! Class destructor.
  ~Asymp(void);

  //! Set target = 1.
  void keyOn(void);

  //! Set target = 0.
  void keyOff(void);

  //! Set the asymptotic rate via the time factor \e tau (must be > 0).
  /*!
    The rate is computed as described above.  The value of \e tau
    must be greater than zero.  Values of \e tau close to zero produce
    fast approach rates, while values greater than 1.0 produce rather
    slow rates.
  */
  void setTau(StkFloat tau);

  //! Set the asymptotic rate based on a time duration (must be > 0).
  void setTime(StkFloat time);

  //! Set the target value.
  void setTarget(StkFloat target);

  //! Return one envelope output value.
  StkFloat tick(void);

  //! Compute \e vectorSize outputs and return them in \e vector.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:
  StkFloat factor_;
  StkFloat constant_;
};

#endif
