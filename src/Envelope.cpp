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

#include "Envelope.h"

Envelope :: Envelope(void) : Generator()
{    
  target_ = 0.0;
  value_ = 0.0;
  rate_ = 0.001;
  state_ = 0;
}

Envelope :: ~Envelope(void)
{    
}

void Envelope :: keyOn(void)
{
  target_ = 1.0;
  if (value_ != target_) state_ = 1;
}

void Envelope :: keyOff(void)
{
  target_ = 0.0;
  if (value_ != target_) state_ = 1;
}

void Envelope :: setRate(StkFloat rate)
{
  if (rate < 0.0) {
    errorString_ << "Envelope::setRate: negative rates not allowed ... correcting!";
    handleError( StkError::WARNING );
    rate_ = -rate;
  }
  else
    rate_ = rate;
}

void Envelope :: setTime(StkFloat time)
{
  if (time < 0.0) {
    errorString_ << "Envelope::setTime: negative times not allowed ... correcting!";
    handleError( StkError::WARNING );
    rate_ = 1.0 / (-time * Stk::sampleRate());
  }
  else
    rate_ = 1.0 / (time * Stk::sampleRate());
}

void Envelope :: setTarget(StkFloat target)
{
  target_ = target;
  if (value_ != target_) state_ = 1;
}

void Envelope :: setValue(StkFloat value)
{
  state_ = 0;
  target_ = value;
  value_ = value;
}

int Envelope :: getState(void) const
{
  return state_;
}

StkFloat Envelope :: tick(void)
{
  if (state_) {
    if (target_ > value_) {
      value_ += rate_;
      if (value_ >= target_) {
        value_ = target_;
        state_ = 0;
      }
    }
    else {
      value_ -= rate_;
      if (value_ <= target_) {
        value_ = target_;
        state_ = 0;
      }
    }
  }

  lastOutput_ = value_;
  return value_;
}

StkFloat *Envelope :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Generator::tick( vector, vectorSize );
}

StkFrames& Envelope :: tick( StkFrames& frames, unsigned int channel )
{
  return Generator::tick( frames, channel );
}
