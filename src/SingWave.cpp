/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "SingWave.h"
 
SingWave :: SingWave( std::string fileName, bool raw)
{
  // An exception could be thrown here.
  wave_ = new WaveLoop( fileName, raw );

	rate_ = 1.0;
	sweepRate_ = 0.001;
	modulator_.setVibratoRate( 6.0 );
	modulator_.setVibratoGain( 0.04 );
	modulator_.setRandomGain( 0.005 );
	this->setFrequency( 75.0 );
	pitchEnvelope_.setRate( 1.0 );
	this->computeSample();
	this->computeSample();
	pitchEnvelope_.setRate( sweepRate_ * rate_ );
}

SingWave :: ~SingWave()
{
  delete wave_;
}

void SingWave :: reset()
{
  wave_->reset();
	lastOutput_ = 0.0;
}

void SingWave :: normalize()
{
  wave_->normalize();
}

void SingWave :: normalize(StkFloat peak)
{
  wave_->normalize( peak );
}

void SingWave :: setFrequency(StkFloat frequency)
{
	StkFloat temp = rate_;
	rate_ = wave_->getSize() * frequency / Stk::sampleRate();
	temp -= rate_;
	if ( temp < 0) temp = -temp;
	pitchEnvelope_.setTarget( rate_ );
	pitchEnvelope_.setRate( sweepRate_ * temp );
}

void SingWave :: setVibratoRate(StkFloat rate)
{
	modulator_.setVibratoRate( rate );
}

void SingWave :: setVibratoGain(StkFloat gain)
{
	modulator_.setVibratoGain(gain);
}

void SingWave :: setRandomGain(StkFloat gain)
{
	modulator_.setRandomGain(gain);
}

void SingWave :: setSweepRate(StkFloat rate)
{
	sweepRate_ = rate;
}

void SingWave :: setGainRate(StkFloat rate)
{
	envelope_.setRate(rate);
}

void SingWave :: setGainTarget(StkFloat target)
{
	envelope_.setTarget(target);
}

void SingWave :: noteOn()
{
	envelope_.keyOn();
}

void SingWave :: noteOff()
{
	envelope_.keyOff();
}

StkFloat SingWave :: computeSample()
{
  // Set the wave rate.
  StkFloat newRate = pitchEnvelope_.tick();
  newRate += newRate * modulator_.tick();
  wave_->setRate( newRate );

  lastOutput_ = wave_->tick();
	lastOutput_ *= envelope_.tick();

	return lastOutput_;             
}
