/***************************************************/
/*! \class SingWave
    \brief STK "singing" looped soundfile class.

    This class contains all that is needed to make
    a pitched musical sound, like a simple voice
    or violin.  In general, it will not be used
    alone because of munchkinification effects
    from pitch shifting.  It will be used as an
    excitation source for other instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "SingWave.h"
 
SingWave :: SingWave(const char *fileName, bool raw)
{
  // An exception could be thrown here.
  wave = new WaveLoop( fileName, raw );

	rate = 1.0;
	sweepRate = 0.001;
	modulator = new Modulate();
	modulator->setVibratoRate( 6.0 );
	modulator->setVibratoGain( 0.04 );
	modulator->setRandomGain( 0.005 );
	envelope = new Envelope;
	pitchEnvelope = new Envelope;
	setFrequency( 75.0 );
	pitchEnvelope->setRate( 1.0 );
	this->tick();
	this->tick();
	pitchEnvelope->setRate( sweepRate * rate );
}

SingWave :: ~SingWave()
{
  delete wave;
	delete modulator;
	delete envelope;
	delete pitchEnvelope;
}

void SingWave :: reset()
{
  wave->reset();
	lastOutput = 0.0;
}

void SingWave :: normalize()
{
  wave->normalize();
}

void SingWave :: normalize(MY_FLOAT newPeak)
{
  wave->normalize( newPeak );
}

void SingWave :: setFrequency(MY_FLOAT frequency)
{
	MY_FLOAT temp = rate;
	rate = wave->getSize() * frequency / Stk::sampleRate();
	temp -= rate;
	if ( temp < 0) temp = -temp;
	pitchEnvelope->setTarget( rate );
	pitchEnvelope->setRate( sweepRate * temp );
}

void SingWave :: setVibratoRate(MY_FLOAT aRate)
{
	modulator->setVibratoRate( aRate );
}

void SingWave :: setVibratoGain(MY_FLOAT gain)
{
	modulator->setVibratoGain(gain);
}

void SingWave :: setRandomGain(MY_FLOAT gain)
{
	modulator->setRandomGain(gain);
}

void SingWave :: setSweepRate(MY_FLOAT aRate)
{
	sweepRate = aRate;
}

void SingWave :: setGainRate(MY_FLOAT aRate)
{
	envelope->setRate(aRate);
}

void SingWave :: setGainTarget(MY_FLOAT target)
{
	envelope->setTarget(target);
}

void SingWave :: noteOn()
{
	envelope->keyOn();
}

void SingWave :: noteOff()
{
	envelope->keyOff();
}

MY_FLOAT SingWave :: tick()
{
  // Set the wave rate.
  MY_FLOAT newRate = pitchEnvelope->tick();
  newRate += newRate * modulator->tick();
  wave->setRate( newRate );

  lastOutput = wave->tick();
	lastOutput *= envelope->tick();
    
	return lastOutput;             
}

MY_FLOAT SingWave :: lastOut()
{
	return lastOutput;
}
