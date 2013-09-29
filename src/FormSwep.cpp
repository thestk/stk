/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This class implements a formant (resonance) which can be "swept"
    over time from one frequency setting to another.  It provides
    methods for controlling the sweep rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "FormSwep.h"
#include <cmath>

namespace stk {

FormSwep :: FormSwep( void )
{
  frequency_ = 0.0;
  radius_ = 0.0;
  targetGain_ = 1.0;
  targetFrequency_ = 0.0;
  targetRadius_ = 0.0;
  deltaGain_ = 0.0;
  deltaFrequency_ = 0.0;
  deltaRadius_ = 0.0;
  sweepState_ = 0.0;
  sweepRate_ = 0.002;
  dirty_ = false;

  b_.resize( 3, 0.0 );
  a_.resize( 3, 0.0 );
  a_[0] = 1.0;
  inputs_.resize( 3, 1, 0.0 );
  outputs_.resize( 3, 1, 0.0 );

  Stk::addSampleRateAlert( this );
}

FormSwep :: ~FormSwep()
{
  Stk::removeSampleRateAlert( this );
}

void FormSwep :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  if ( !ignoreSampleRateChange_ ) {
    errorString_ << "FormSwep::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}

void FormSwep :: setResonance( StkFloat frequency, StkFloat radius )
{
  radius_ = radius;
  frequency_ = frequency;

  a_[2] = radius * radius;
  a_[1] = -2.0 * radius * cos( TWO_PI * frequency / Stk::sampleRate() );

  // Use zeros at +- 1 and normalize the filter peak gain.
  b_[0] = 0.5 - 0.5 * a_[2];
  b_[1] = 0.0;
  b_[2] = -b_[0];
}

void FormSwep :: setStates( StkFloat frequency, StkFloat radius, StkFloat gain )
{
  dirty_ = false;

  if ( frequency_ != frequency || radius_ != radius )
    this->setResonance( frequency, radius );

  gain_ = gain;
  targetFrequency_ = frequency;
  targetRadius_ = radius;
  targetGain_ = gain;
}

void FormSwep :: setTargets( StkFloat frequency, StkFloat radius, StkFloat gain )
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
  sweepState_ = 0.0;
}

void FormSwep :: setSweepRate( StkFloat rate )
{
  sweepRate_ = rate;
  if ( sweepRate_ > 1.0 ) sweepRate_ = 1.0;
  if ( sweepRate_ < 0.0 ) sweepRate_ = 0.0;
}

void FormSwep :: setSweepTime( StkFloat time )
{
  sweepRate_ = 1.0 / ( time * Stk::sampleRate() );
  if ( sweepRate_ > 1.0 ) sweepRate_ = 1.0;
  if ( sweepRate_ < 0.0 ) sweepRate_ = 0.0;
}

} // stk namespace
