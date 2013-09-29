/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Modulate.h"

Modulate :: Modulate()
{
  // Concatenate the STK rawwave path to the rawwave file
  vibrato_ = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  vibrato_->setFrequency( 6.0 );
  vibratoGain_ = 0.04;

  noise_.setRate( 330 );
  randomGain_ = 0.05;

  filter_.setPole( 0.999 );
  filter_.setGain( randomGain_ );
}

Modulate :: ~Modulate()
{
  delete vibrato_;
}

void Modulate :: reset()
{
  lastOutput_ = (StkFloat)  0.0;
}

void Modulate :: setVibratoRate(StkFloat rate)
{
  vibrato_->setFrequency( rate );
}

void Modulate :: setVibratoGain(StkFloat gain)
{
  vibratoGain_ = gain;
}

void Modulate :: setRandomGain(StkFloat gain)
{
  randomGain_ = gain;
  filter_.setGain( randomGain_ );
}

StkFloat Modulate :: tick()
{
  // Compute periodic and random modulations.
  lastOutput_ = vibratoGain_ * vibrato_->tick();
  lastOutput_ += filter_.tick( noise_.tick() );
  return lastOutput_;
}

StkFloat *Modulate :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Generator::tick( vector, vectorSize );
}

StkFrames& Modulate :: tick( StkFrames& frames, unsigned int channel )
{
  return Generator::tick( frames, channel );
}

