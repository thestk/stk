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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "VoicForm.h"
#include "Phonemes.h"
#include "SKINI.msg"

#include <cmath>

VoicForm :: VoicForm() : Instrmnt()
{
  // Concatenate the STK rawwave path to the rawwave file
	voiced_ = new SingWave( (Stk::rawwavePath() + "impuls20.raw").c_str(), true );
	voiced_->setGainRate( 0.001 );
	voiced_->setGainTarget( 0.0 );

  for ( int i=0; i<4; i++ )
    filters_[i].setSweepRate( 0.001 );
    
	onezero_.setZero( -0.9 );
	onepole_.setPole( 0.9 );
    
	noiseEnv_.setRate( 0.001 );
	noiseEnv_.setTarget( 0.0 );
    
	this->setPhoneme( "eee" );
	this->clear();
}  

VoicForm :: ~VoicForm()
{
	delete voiced_;
}

void VoicForm :: clear()
{
	onezero_.clear();
	onepole_.clear();
  for ( int i=0; i<4; i++ ) {
    filters_[i].clear();
  }
}

void VoicForm :: setFrequency( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "VoicForm::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

	voiced_->setFrequency( freakency );
}

bool VoicForm :: setPhoneme( const char *phoneme )
{
	bool found = false;
  unsigned int i = 0;
	while( i < 32 && !found ) {
		if ( !strcmp( Phonemes::name(i), phoneme ) ) {
			found = true;
      filters_[0].setTargets( Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
      filters_[1].setTargets( Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
      filters_[2].setTargets( Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
      filters_[3].setTargets( Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
      this->setVoiced( Phonemes::voiceGain( i ) );
      this->setUnVoiced( Phonemes::noiseGain( i ) );
#if defined(_STK_DEBUG_)
      errorString_ << "VoicForm::setPhoneme: found formant " << phoneme << " (number " << i << ").";
      handleError( StkError::DEBUG_WARNING );
#endif
		}
		i++;
	}

	if ( !found ) {
    errorString_ << "VoicForm::setPhoneme: phoneme " << phoneme << " not found!";
    handleError( StkError::WARNING );
  }

	return found;
}

void VoicForm :: setVoiced(StkFloat vGain)
{
	voiced_->setGainTarget(vGain);
}

void VoicForm :: setUnVoiced(StkFloat nGain)
{
	noiseEnv_.setTarget(nGain);
}

void VoicForm :: setFilterSweepRate(unsigned int whichOne, StkFloat rate)
{
  if ( whichOne < 0 || whichOne > 3 ) {
    errorString_ << "VoicForm::setFilterSweepRate: filter select argument outside range 0-3!";
    handleError( StkError::WARNING );
    return;
  }

	filters_[whichOne].setSweepRate(rate);
}

void VoicForm :: setPitchSweepRate(StkFloat rate)
{
	voiced_->setSweepRate(rate);
}

void VoicForm :: speak()
{
	voiced_->noteOn();
}

void VoicForm :: quiet()
{
	voiced_->noteOff();
	noiseEnv_.setTarget( 0.0 );
}

void VoicForm :: noteOn(StkFloat frequency, StkFloat amplitude)
{
	this->setFrequency( frequency );
	voiced_->setGainTarget( amplitude );
	onepole_.setPole( 0.97 - (amplitude * 0.2) );
}

void VoicForm :: noteOff(StkFloat amplitude)
{
	this->quiet();
}

StkFloat VoicForm :: computeSample()
{
  StkFloat temp;
  temp = onepole_.tick( onezero_.tick( voiced_->tick() ) );
  temp += noiseEnv_.tick() * noise_.tick();
  lastOutput_ = filters_[0].tick(temp);
  lastOutput_ += filters_[1].tick(temp);
  lastOutput_ += filters_[2].tick(temp);
  lastOutput_ += filters_[3].tick(temp);
  /*
    temp  += noiseEnv_.tick() * noise_.tick();
    lastOutput_  = filters_[0].tick(temp);
    lastOutput_  = filters_[1].tick(lastOutput_);
    lastOutput_  = filters_[2].tick(lastOutput_);
    lastOutput_  = filters_[3].tick(lastOutput_);
  */
	return lastOutput_;
}
 
void VoicForm :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "VoicForm::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "VoicForm::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

	if (number == __SK_Breath_)	{ // 2
		this->setVoiced( 1.0 - norm );
		this->setUnVoiced( 0.01 * norm );
	}
	else if (number == __SK_FootControl_)	{ // 4
    StkFloat temp = 0.0;
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
    filters_[0].setTargets( temp * Phonemes::formantFrequency(i, 0), Phonemes::formantRadius(i, 0), pow(10.0, Phonemes::formantGain(i, 0 ) / 20.0) );
    filters_[1].setTargets( temp * Phonemes::formantFrequency(i, 1), Phonemes::formantRadius(i, 1), pow(10.0, Phonemes::formantGain(i, 1 ) / 20.0) );
    filters_[2].setTargets( temp * Phonemes::formantFrequency(i, 2), Phonemes::formantRadius(i, 2), pow(10.0, Phonemes::formantGain(i, 2 ) / 20.0) );
    filters_[3].setTargets( temp * Phonemes::formantFrequency(i, 3), Phonemes::formantRadius(i, 3), pow(10.0, Phonemes::formantGain(i, 3 ) / 20.0) );
    this->setVoiced( Phonemes::voiceGain( i ) );
    this->setUnVoiced( Phonemes::noiseGain( i ) );
	}
	else if (number == __SK_ModFrequency_) // 11
		voiced_->setVibratoRate( norm * 12.0);  // 0 to 12 Hz
	else if (number == __SK_ModWheel_) // 1
		voiced_->setVibratoGain( norm * 0.2);
	else if (number == __SK_AfterTouch_Cont_)	{ // 128
		this->setVoiced( norm );
		onepole_.setPole( 0.97 - ( norm * 0.2) );
	}
  else {
    errorString_ << "VoicForm::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "VoicForm::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}
