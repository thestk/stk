/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "TwoPole.h"
#include <math.h>

TwoPole :: TwoPole() : Filter()
{
  std::vector<StkFloat> b(1, 1.0);
  std::vector<StkFloat> a(3, 0.0);
  a[0] = 1.0;
  Filter::setCoefficients( b, a );
}

TwoPole :: ~TwoPole()
{
}

void TwoPole :: clear(void)
{
  Filter::clear();
}

void TwoPole :: setB0(StkFloat b0)
{
  b_[0] = b0;
}

void TwoPole :: setA1(StkFloat a1)
{
  a_[1] = a1;
}

void TwoPole :: setA2(StkFloat a2)
{
  a_[2] = a2;
}

void TwoPole :: setResonance(StkFloat frequency, StkFloat radius, bool normalize)
{
  a_[2] = radius * radius;
  a_[1] = (StkFloat) -2.0 * radius * cos(TWO_PI * frequency / Stk::sampleRate());

  if ( normalize ) {
    // Normalize the filter gain ... not terribly efficient.
    StkFloat real = 1 - radius + (a_[2] - radius) * cos(TWO_PI * 2 * frequency / Stk::sampleRate());
    StkFloat imag = (a_[2] - radius) * sin(TWO_PI * 2 * frequency / Stk::sampleRate());
    b_[0] = sqrt( pow(real, 2) + pow(imag, 2) );
  }
}

void TwoPole :: setGain(StkFloat gain)
{
  Filter::setGain(gain);
}

StkFloat TwoPole :: getGain(void) const
{
  return Filter::getGain();
}

StkFloat TwoPole :: lastOut(void) const
{
  return Filter::lastOut();
}

StkFloat TwoPole :: tick(StkFloat sample)
{
  inputs_[0] = gain_ * sample;
  outputs_[0] = b_[0] * inputs_[0] - a_[2] * outputs_[2] - a_[1] * outputs_[1];
  outputs_[2] = outputs_[1];
  outputs_[1] = outputs_[0];

  return outputs_[0];
}

StkFloat *TwoPole :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Filter::tick( vector, vectorSize );
}

StkFrames& TwoPole :: tick( StkFrames& frames, unsigned int channel )
{
  return Filter::tick( frames, channel );
}
