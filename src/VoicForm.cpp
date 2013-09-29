/***************************************************/
/*! \class VoicForm
    \brief Four formant synthesis instrument.

    This instrument contains an excitation singing
    wavetable (looping wave with random and
    periodic vibrato, smoothing on frequency,
    etc.), excitation noise, and four sweepable
    complex resonances.

    Measured formant data is included, and enough
    data is there to support either parallel or
    cascade synthesis.  In the floating point case
    cascade synthesis is the most natural so
    that's what you'll find here.

    Control Change Numbers: 
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "VoicForm.h"
#include "Phonemes.h"
#include "SKINI.msg"

#include <math.h>
#include <string.h>

VoicForm :: VoicForm() : Instrmnt()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
	voiced = new SingWave( strcat(file,"impuls20.raw"), TRUE );
	voiced->setGainRate( 0.001 );
	voiced->setGainTarget( 0.0 );

	noise = new Noise;

  for ( int i=0; i<4; i++ ) {
    filters[i] = new FormSwep;
    filters[i]->setSweepRate( 0.001 );
  }
    
	onezero = new OneZero;
	onezero->setZero( -0.9 );
	onepole = new OnePole;
	onepole->setPole( 0.9 );
    
	noiseEnv = new Envelope;
	noiseEnv->setRate( 0.001 );
	noiseEnv->setTarget( 0.0 );
    
	this->setPhoneme( "eee" );
	this->clear();
}  

VoicForm :: ~VoicForm()
{
	delete voiced;
	delete noise;
	delete onezero;
	delete onepole;
	delete noiseEnv;
  for ( int i=0; i<4; i++ ) {
    delete filters[i];
  }
}

void VoicForm :: clear()
{
	onezero->clear();
	onepole->clear();
  for ( int i=0; i<4; i++ ) {
    filters[i]->clear();
  }
}

void VoicForm :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "VoicForm: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

	voiced->setFrequency( freakency );
}

bool VoicForm :: setPhoneme(const char *phoneme )
{
	bool found = false;
  unsigned int i = 0;
	while( i < 32 && !found ) {
		if ( !strcmp( Phonemes::name(i), phoneme ) ) {
			found = true;
      filters[0]->setTargets( Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
      filters[1]->setTargets( Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
      filters[2]->setTargets( Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
      filters[3]->setTargets( Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
      setVoiced( Phonemes::voiceGain( i ) );
      setUnVoiced( Phonemes::noiseGain( i ) );
#if defined(_STK_DEBUG_)
			cout << "VoicForm: found formant " << phoneme << " (number " << i << ")" << endl;
#endif
		}
		i++;
	}

	if ( !found )
    cerr << "VoicForm: phoneme " << phoneme << " not found!" << endl;

	return found;
}

void VoicForm :: setVoiced(MY_FLOAT vGain)
{
	voiced->setGainTarget(vGain);
}

void VoicForm :: setUnVoiced(MY_FLOAT nGain)
{
	noiseEnv->setTarget(nGain);
}

void VoicForm :: setFilterSweepRate(int whichOne, MY_FLOAT rate)
{
  if ( whichOne < 0 || whichOne > 3 ) {
    cerr << "VoicForm: setFilterSweepRate filter argument outside range 0-3!" << endl;
    return;
  }

	filters[whichOne]->setSweepRate(rate);
}

void VoicForm :: setPitchSweepRate(MY_FLOAT rate)
{
	voiced->setSweepRate(rate);
}

void VoicForm :: speak()
{
	voiced->noteOn();
}

void VoicForm :: quiet()
{
	voiced->noteOff();
	noiseEnv->setTarget( 0.0 );
}

void VoicForm :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
	setFrequency(frequency);
	voiced->setGainTarget(amplitude);
	onepole->setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOff(MY_FLOAT amplitude)
{
	this->quiet();
}

MY_FLOAT VoicForm :: tick()
{
	MY_FLOAT temp;
	temp = onepole->tick( onezero->tick( voiced->tick() ) );
	temp += noiseEnv->tick() * noise->tick();
	lastOutput = filters[0]->tick(temp);
	lastOutput += filters[1]->tick(temp);
	lastOutput += filters[2]->tick(temp);
	lastOutput += filters[3]->tick(temp);
  /*
	temp  += noiseEnv->tick() * noise->tick();
	lastOutput  = filters[0]->tick(temp);
	lastOutput  = filters[1]->tick(lastOutput);
	lastOutput  = filters[2]->tick(lastOutput);
	lastOutput  = filters[3]->tick(lastOutput);
  */
	return lastOutput;
}
 
void VoicForm :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "VoicForm: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "VoicForm: Control value greater than 128.0!" << endl;
  }

	if (number == __SK_Breath_)	{ // 2
		this->setVoiced( 1.0 - norm );
		this->setUnVoiced( 0.01 * norm );
	}
	else if (number == __SK_FootControl_)	{ // 4
    MY_FLOAT temp = 0.0;
		unsigned int i = (int) value;
		if (i < 32)	{
      temp = 0.9;
		}
		else if (i < 64)	{
      i -= 32;
      temp = 1.0;
		}
		else if (i < 96)	{
      i -= 64;
      temp = 1.1;
		}
		else if (i < 128)	{
      i -= 96;
      temp = 1.2;
		}
		else if (i == 128)	{
      i = 0;
      temp = 1.4;
		}
    filters[0]->setTargets( temp * Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
    filters[1]->setTargets( temp * Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
    filters[2]->setTargets( temp * Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
    filters[3]->setTargets( temp * Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
    setVoiced( Phonemes::voiceGain( i ) );
    setUnVoiced( Phonemes::noiseGain( i ) );
	}
	else if (number == __SK_ModFrequency_) // 11
		voiced->setVibratoRate( norm * 12.0);  // 0 to 12 Hz
	else if (number == __SK_ModWheel_) // 1
		voiced->setVibratoGain( norm * 0.2);
	else if (number == __SK_AfterTouch_Cont_)	{ // 128
		setVoiced( norm );
		onepole->setPole( 0.97 - ( norm * 0.2) );
	}
  else
    cerr << "VoicForm: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "VoicForm: controlChange number = " << number << ", value = " << value << endl;
#endif
}
