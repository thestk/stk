/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "TwoZero.h"
#include <math.h>

TwoZero :: TwoZero() : Filter()
{
  std::vector<StkFloat> b(3, 0.0);
  b[0] = 1.0;
  std::vector<StkFloat> a(1, 1.0);
  Filter::setCoefficients( b, a );
  Stk::addSampleRateAlert( this );
}

TwoZero :: ~TwoZero()
{
  Stk::removeSampleRateAlert( this );
}

void TwoZero :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  if ( !ignoreSampleRateChange_ ) {
    errorString_ << "TwoZero::sampleRateChanged: you may need to recompute filter coefficients!";
    handleError( StkError::WARNING );
  }
}

void TwoZero :: clear(void)
{
  Filter::clear();
}

void TwoZero :: setB0(StkFloat b0)
{
  b_[0] = b0;
}

void TwoZero :: setB1(StkFloat b1)
{
  b_[1] = b1;
}

void TwoZero :: setB2(StkFloat b2)
{
  b_[2] = b2;
}

void TwoZero :: setNotch(StkFloat frequency, StkFloat radius)
{
  b_[2] = radius * radius;
  b_[1] = (StkFloat) -2.0 * radius * cos(TWO_PI * (double) frequency / Stk::sampleRate());

  // Normalize the filter gain.
  if (b_[1] > 0.0) // Maximum at z = 0.
    b_[0] = 1.0 / (1.0+b_[1]+b_[2]);
  else            // Maximum at z = -1.
    b_[0] = 1.0 / (1.0-b_[1]+b_[2]);
  b_[1] *= b_[0];
  b_[2] *= b_[0];
}

void TwoZero :: setGain(StkFloat gain)
{
  Filter::setGain(gain);
}

StkFloat TwoZero :: getGain(void) const
{
  return Filter::getGain();
}

StkFloat TwoZero :: lastOut(void) const
{
  return Filter::lastOut();
}

StkFloat TwoZero :: tick( StkFloat input )
{
  inputs_[0] = gain_ * input;
  outputs_[0] = b_[2] * inputs_[2] + b_[1] * inputs_[1] + b_[0] * inputs_[0];
  inputs_[2] = inputs_[1];
  inputs_[1] = inputs_[0];

  return outputs_[0];
}

StkFrames& TwoZero :: tick( StkFrames& frames, unsigned int channel )
{
  return Filter::tick( frames, channel );
}
