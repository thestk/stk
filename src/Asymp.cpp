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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "Asymp.h"
#include <cmath>

Asymp :: Asymp(void) : Envelope()
{
  factor_ = exp( -1.0 / ( 0.3 * Stk::sampleRate() ) );
  constant_ = 0.0;
}

Asymp :: ~Asymp(void)
{    
}

void Asymp :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  if ( !ignoreSampleRateChange_ ) {
    StkFloat tau = -1.0 / ( std::log( factor_ ) * oldRate );
    factor_ = std::exp( -1.0 / ( tau * newRate ) );
  }
}

void Asymp :: keyOn(void)
{
  Envelope::keyOn();
  constant_ = ( 1.0 - factor_ ) * target_;
}

void Asymp :: keyOff(void)
{
  Envelope::keyOff();
  constant_ = ( 1.0 - factor_ ) * target_;
}

void Asymp :: setTau(StkFloat tau)
{
 if (tau <= 0.0) {
    errorString_ << "Asymp::setTau: negative or zero tau not allowed ... ignoring!";
    handleError( StkError::WARNING );
    return;
  }

 factor_ = std::exp( -1.0 / ( tau * Stk::sampleRate() ) );
  constant_ = ( 1.0 - factor_ ) * target_;
}

void Asymp :: setTime(StkFloat time)
{
  if (time <= 0.0) {
    errorString_ << "Asymp::setTime: negative or zero times not allowed ... ignoring!";
    handleError( StkError::WARNING );
    return;
  }

  StkFloat tau = -time / std::log( TARGET_THRESHOLD );
  factor_ = std::exp( -1.0 / ( tau * Stk::sampleRate() ) );
  constant_ = ( 1.0 - factor_ ) * target_;
}

void Asymp :: setTarget(StkFloat target)
{
  Envelope::setTarget( target );
  constant_ = ( 1.0 - factor_ ) * target_;
}

StkFloat Asymp :: computeSample(void)
{
  if (state_) {

    value_ = factor_ * value_ + constant_;

    // Check threshold.
    if ( target_ > value_ ) {
      if ( target_ - value_ <= TARGET_THRESHOLD ) {
        value_ = target_;
        state_ = 0;
      }
    }
    else {
      if ( value_ - target_ <= TARGET_THRESHOLD ) {
        value_ = target_;
        state_ = 0;
      }
    }
  }

  lastOutput_ = value_;
  return value_;
}

