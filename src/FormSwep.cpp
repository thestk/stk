/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "FormSwep.h"

FormSwep :: FormSwep() : BiQuad()
{
  frequency_ = (StkFloat) 0.0;
  radius_ = (StkFloat) 0.0;
  targetGain_ = (StkFloat) 1.0;
  targetFrequency_ = (StkFloat) 0.0;
  targetRadius_ = (StkFloat) 0.0;
  deltaGain_ = (StkFloat) 0.0;
  deltaFrequency_ = (StkFloat) 0.0;
  deltaRadius_ = (StkFloat) 0.0;
  sweepState_ = (StkFloat)  0.0;
  sweepRate_ = (StkFloat) 0.002;
  dirty_ = false;
  this->clear();
}

FormSwep :: ~FormSwep()
{
}

void FormSwep :: setResonance(StkFloat frequency, StkFloat radius)
{
  dirty_ = false;
  radius_ = radius;
  frequency_ = frequency;

  BiQuad::setResonance( frequency_, radius_, true );
}

void FormSwep :: setStates(StkFloat frequency, StkFloat radius, StkFloat gain)
{
  dirty_ = false;

  if ( frequency_ != frequency || radius_ != radius )
    BiQuad::setResonance( frequency, radius, true );

  frequency_ = frequency;
  radius_ = radius;
  gain_ = gain;
  targetFrequency_ = frequency;
  targetRadius_ = radius;
  targetGain_ = gain;
}

void FormSwep :: setTargets(StkFloat frequency, StkFloat radius, StkFloat gain)
{
  dirty_ = true;
  startFrequency_ = frequency_;
  startRadius_ = radius_;
  startGain_ = gain_;
  targetFrequency_ = frequency;
  targetRadius_ = radius;
  targetGain_ = gain;
  deltaFrequency_ = frequency - frequency_;
  deltaRadius_ = radius - radius_;
  deltaGain_ = gain - gain_;
  sweepState_ = (StkFloat) 0.0;
}

void FormSwep :: setSweepRate(StkFloat rate)
{
  sweepRate_ = rate;
  if ( sweepRate_ > 1.0 ) sweepRate_ = 1.0;
  if ( sweepRate_ < 0.0 ) sweepRate_ = 0.0;
}

void FormSwep :: setSweepTime(StkFloat time)
{
  sweepRate_ = 1.0 / ( time * Stk::sampleRate() );
  if ( sweepRate_ > 1.0 ) sweepRate_ = 1.0;
  if ( sweepRate_ < 0.0 ) sweepRate_ = 0.0;
}

StkFloat FormSwep :: computeSample( StkFloat input )
{                                     
  if (dirty_)  {
    sweepState_ += sweepRate_;
    if ( sweepState_ >= 1.0 )   {
      sweepState_ = 1.0;
      dirty_ = false;
      radius_ = targetRadius_;
      frequency_ = targetFrequency_;
      gain_ = targetGain_;
    }
    else  {
      radius_ = startRadius_ + (deltaRadius_ * sweepState_);
      frequency_ = startFrequency_ + (deltaFrequency_ * sweepState_);
      gain_ = startGain_ + (deltaGain_ * sweepState_);
    }
    BiQuad::setResonance( frequency_, radius_, true );
  }

  return BiQuad::computeSample( input );
}

