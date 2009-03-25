/***************************************************/
/*! \class Blotar
    \brief STK Blotar class.

    This class is a port of Dan Trueman's MSP/PeRColate "blotar"
    instrument, which itself was a modified version of the STK
    waveguide flute model.

    This port was made by Simon de Leon in the Music Technology area
    at McGill University in 2006.

    This is a digital waveguide model, making its use possibly subject
    to patents held by Stanford University, Yamaha, and others.  There
    exist at least two patents, assigned to Stanford, bearing the
    names of Karplus and/or Strong.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "Blotar.h"
#include "SKINI.msg"

Blotar :: Blotar()
{
  this->initialize( 100.0 );
}

Blotar :: Blotar( StkFloat lowestFrequency )
{
  this->initialize( lowestFrequency );
}

Blotar :: ~Blotar()
{
  for ( int i=0; i<12; i++ )
    delete soundfile_[i];
}

void Blotar :: initialize( StkFloat lowestFrequency )
{
  length_ = (unsigned long) (Stk::sampleRate() / lowestFrequency + 1);
  boreDelay_.setMaximumDelay( length_ );
  boreDelay_.setDelay( 100.0 );

  jetDelay_.setMaximumDelay( 2 * length_ );
  jetDelay_.setDelay( 49.0 );

  combDelay_.setMaximumDelay( length_ );
	combDelay_.setDelay( length_ / 2 );

  vibrato_.setFrequency( 5.0 );
  dcBlock_.setBlockZero();

  this->clear();
  
  // Initialize control variables
  pluckAmplitude_ = 0.8;
  pluckPosition_ = 0.4;
  lastFrequency_ = 220.0;
  //endReflection_ = 0.5;
  //jetReflection_ = 0.5;
  endReflection_ = 0.99;
  jetReflection_ = 0.06;
  noiseGain_     = 0.15;    // Breath pressure random component.
  vibratoGain_   = 0.05;    // Breath periodic vibrato component.
  jetRatio_      = 0.32;
  filterRatio_ = 1.0;

  // Impulse responses
  soundfile_[0] = new FileWvIn( (Stk::rawwavePath() + "mand1.raw").c_str(), true );
  soundfile_[1] = new FileWvIn( (Stk::rawwavePath() + "mand2.raw").c_str(), true );
  soundfile_[2] = new FileWvIn( (Stk::rawwavePath() + "mand3.raw").c_str(), true );
  soundfile_[3] = new FileWvIn( (Stk::rawwavePath() + "mand4.raw").c_str(), true );
  soundfile_[4] = new FileWvIn( (Stk::rawwavePath() + "mand5.raw").c_str(), true );
  soundfile_[5] = new FileWvIn( (Stk::rawwavePath() + "mand6.raw").c_str(), true );
  soundfile_[6] = new FileWvIn( (Stk::rawwavePath() + "mand7.raw").c_str(), true );
  soundfile_[7] = new FileWvIn( (Stk::rawwavePath() + "mand8.raw").c_str(), true );
  soundfile_[8] = new FileWvIn( (Stk::rawwavePath() + "mand9.raw").c_str(), true );
  soundfile_[9] = new FileWvIn( (Stk::rawwavePath() + "mand10.raw").c_str(), true );
  soundfile_[10] = new FileWvIn( (Stk::rawwavePath() + "mand11.raw").c_str(), true );
  soundfile_[11] = new FileWvIn( (Stk::rawwavePath() + "mand12.raw").c_str(), true );

  mic_ = 0;
	waveDone_ = soundfile_[mic_]->isFinished();

  this->setFrequency( lastFrequency_ );

  fluteFilter_.setPole( 0.7 - ((StkFloat) 0.1 * 22050.0 / Stk::sampleRate() ) );
  fluteFilter_.setGain( -1.0 );
}

void Blotar :: clear()
{  
  boreDelay_.clear();
  jetDelay_.clear();
  combDelay_.clear();
  dcBlock_.clear();
  fluteFilter_.clear();
}

void Blotar :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  if ( frequency != lastFrequency_ )
    this->setFrequency( frequency );

	this->pluck( amplitude );

  #if defined(_STK_DEBUG_)
    errorString_ << "Blotar::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
    handleError( StkError::DEBUG_WARNING );
  #endif
}

void Blotar :: noteOff( StkFloat amplitude )
{
}

void Blotar :: setFrequency( StkFloat frequency )
{
  lastFrequency_ = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Blotar::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    lastFrequency_ = 220.0;
  }

  // We're overblowing here.
  lastFrequency_ *= 0.66666;

  // delay = length - approximate filter delay.
  StkFloat delay = Stk::sampleRate() / lastFrequency_ - (StkFloat) 2.0;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length_ ) delay = length_;

  boreDelay_.setDelay( delay );
  jetDelay_.setDelay( delay * jetRatio_ );
}

void Blotar :: pluck( StkFloat amplitude )
{
  // This function gets interesting, because pluck may be longer than
  // string length, so we just reset the soundfile and add in the
  // pluck in the tick method.
  soundfile_[mic_]->reset();
  waveDone_ = false;
  pluckAmplitude_ = amplitude;
  if ( amplitude < 0.0 ) {
    errorString_ << "Blotar::pluck: amplitude parameter less than zero ... setting to 0.0!";
    handleError( StkError::WARNING );
    pluckAmplitude_ = 0.0;
  }
  else if ( amplitude > 1.0 ) {
    errorString_ << "Blotar::pluck: amplitude parameter greater than one ... setting to 1.0!";
    handleError( StkError::WARNING );
    pluckAmplitude_ = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay_.setDelay( 0.5 * pluckPosition_ * length_ ); 
}

void Blotar :: setPluckPosition( StkFloat position )
{
  pluckPosition_ = position;
}

void Blotar :: setBodySize( StkFloat size )
{
  for ( int i=0; i<12; i++ ) 
    soundfile_[i]->setRate( size );
}

void Blotar :: setBreathPressure( StkFloat bp )
{
  maxPressure_ = bp;
}

void Blotar :: setJetDelay( StkFloat aRatio )
{
  // Delay = length - approximate filter delay.
  StkFloat temp = Stk::sampleRate() / lastFrequency_ - (StkFloat) 2.0;
  jetRatio_ = aRatio;
  jetDelay_.setDelay(temp * aRatio); // Scaled by ratio.
}

/*
void Blotar :: setJetDelay( StkFloat frequency )
{
	StkFloat temp;
	if ( frequency < WATCHIT )
		frequency = WATCHIT;
	StkFloat temp = (Stk::sampleRate() / frequency) - 2;

	// Control jet length directly, not as function of bore length
	jetDelay_.setDelay( temp );

	jd_ = frequency; 
}
*/

void Blotar :: setNoiseGain( StkFloat noiseGain )
{
  noiseGain_ = noiseGain;
}

void Blotar :: setVibratoGain( StkFloat gain )
{
  vibratoGain_ = gain;
}
void Blotar :: setJetReflection( StkFloat jetReflection )
{
  jetReflection_ = jetReflection;
}

void Blotar :: setEndReflection( StkFloat endReflection )
{
  endReflection_ = endReflection;
}

void Blotar :: setFilterRatio( StkFloat filterRatio )
{
  filterRatio_ = filterRatio;
}

StkFloat Blotar :: computeSample()
{  
	StkFloat temp = 0.0;
	if ( !waveDone_ ) {
		// Scale the pluck excitation with comb
		// filtering for the duration of the file.
		temp =  soundfile_[mic_]->tick() * pluckAmplitude_;
		temp = temp - combDelay_.tick( temp );
		waveDone_ = soundfile_[mic_]->isFinished();
	}

	// Interpolate between one-pole filter of flute and one-zero filter of string
  StkFloat flute = fluteFilter_.tick( boreDelay_.lastOut() + temp );  
  StkFloat string = lowpass_.tick( boreDelay_.lastOut() + temp );
	temp = filterRatio_ * flute + ( 1 - filterRatio_ ) * string;
  temp = dcBlock_.tick( temp );

	// Random turbulence and vibrato
  StkFloat breathPressure = maxPressure_ * ( noiseGain_ * noise_.tick() + vibratoGain_ * vibrato_.tick() );

  StkFloat pressureDiff = breathPressure - ( jetReflection_ * temp );
  pressureDiff = jetDelay_.tick( pressureDiff );
    
  // Implement the "classic" mode of MSP blotar~ as if "mode" was default 
  //pressureDiff = jetTable_.tick( pressureDiff + ( endReflection_ * temp ) );
  pressureDiff = jetTable_.tick( pressureDiff ) + ( endReflection_ * temp );
  lastOutput_ = boreDelay_.tick( pressureDiff );

  return lastOutput_;
}

void Blotar :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Blotar::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Blotar::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_JetDelay_) // 2
  //if (number == __SK_BodySize_) // 2
    //this->setBodySize( norm * 2.0 );
    this->setJetDelay( (StkFloat) (0.08 + (0.48 * norm)) );
  //else if (number == __SK_NoiseLevel_) // 4
  else if (number == __SK_PickPosition_) // 4
    this->setPluckPosition( norm );
  //this->setNoiseGain( norm * 0.4 );
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setVibratoGain( norm * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    this->setBreathPressure( norm );
  else {
    errorString_ << "Blotar::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Blotar::controlChange: number = " << number << ", value = " << value << ".";
    handleError( StkError::DEBUG_WARNING );
#endif
}
