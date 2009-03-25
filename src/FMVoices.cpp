/***************************************************/
/*! \class FMVoices
    \brief STK singing FM synthesis instrument.

    This class implements 3 carriers and a common
    modulator, also referred to as algorithm 6 of
    the TX81Z.

    \code
    Algorithm 6 is :
                        /->1 -\
                     4-|-->2 - +-> Out
                        \->3 -/
    \endcode

    Control Change Numbers: 
       - Vowel = 2
       - Spectral Tilt = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "FMVoices.h"
#include "SKINI.msg"
#include "Phonemes.h"

FMVoices :: FMVoices()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( unsigned int i=0; i<3; i++ )
    waves_[i] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  waves_[3] = new WaveLoop( (Stk::rawwavePath() + "fwavblnk.raw").c_str(), true );

  this->setRatio(0, 2.00);
  this->setRatio(1, 4.00);
  this->setRatio(2, 12.0);
  this->setRatio(3, 1.00);

  gains_[3] = fmGains_[80];

  adsr_[0]->setAllTimes( 0.05, 0.05, fmSusLevels_[15], 0.05);
  adsr_[1]->setAllTimes( 0.05, 0.05, fmSusLevels_[15], 0.05);
  adsr_[2]->setAllTimes( 0.05, 0.05, fmSusLevels_[15], 0.05);
  adsr_[3]->setAllTimes( 0.01, 0.01, fmSusLevels_[15], 0.5);

  twozero_.setGain( 0.0 );
  modDepth_ = (StkFloat) 0.005;
  currentVowel_ = 0;
  tilt_[0] = 1.0;
  tilt_[1] = 0.5;
  tilt_[2] = 0.2;    
  mods_[0] = 1.0;
  mods_[1] = 1.1;
  mods_[2] = 1.1;
  baseFrequency_ = 110.0;
  this->setFrequency( 110.0 );    
}  

FMVoices :: ~FMVoices()
{
}

void FMVoices :: setFrequency(StkFloat frequency)
{
  StkFloat temp, temp2 = 0.0;
  int tempi = 0;
  unsigned int i = 0;

  if (currentVowel_ < 32)	{
    i = currentVowel_;
    temp2 = 0.9;
  }
  else if (currentVowel_ < 64)	{
    i = currentVowel_ - 32;
    temp2 = 1.0;
  }
  else if (currentVowel_ < 96)	{
    i = currentVowel_ - 64;
    temp2 = 1.1;
  }
  else if (currentVowel_ <= 128)	{
    i = currentVowel_ - 96;
    temp2 = 1.2;
  }

  baseFrequency_ = frequency;
  temp = (temp2 * Phonemes::formantFrequency(i, 0) / baseFrequency_) + 0.5;
  tempi = (int) temp;
  this->setRatio( 0, (StkFloat) tempi );
  temp = (temp2 * Phonemes::formantFrequency(i, 1) / baseFrequency_) + 0.5;
  tempi = (int) temp;
  this->setRatio( 1, (StkFloat) tempi );
  temp = (temp2 * Phonemes::formantFrequency(i, 2) / baseFrequency_) + 0.5;
  tempi = (int) temp;
  this->setRatio( 2, (StkFloat) tempi );    
  gains_[0] = 1.0;
  gains_[1] = 1.0;
  gains_[2] = 1.0;
}

void FMVoices :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  this->setFrequency( frequency );
  tilt_[0] = amplitude;
  tilt_[1] = amplitude * amplitude;
  tilt_[2] = tilt_[1] * amplitude;
  this->keyOn();

#if defined(_STK_DEBUG_)
  errorString_ << "FMVoices::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat FMVoices :: computeSample()
{
  register StkFloat temp, temp2;

  temp = gains_[3] * adsr_[3]->tick() * waves_[3]->tick();
  temp2 = vibrato_.tick() * modDepth_ * 0.1;

  waves_[0]->setFrequency(baseFrequency_ * (1.0 + temp2) * ratios_[0]);
  waves_[1]->setFrequency(baseFrequency_ * (1.0 + temp2) * ratios_[1]);
  waves_[2]->setFrequency(baseFrequency_ * (1.0 + temp2) * ratios_[2]);
  waves_[3]->setFrequency(baseFrequency_ * (1.0 + temp2) * ratios_[3]);

  waves_[0]->addPhaseOffset(temp * mods_[0]);
  waves_[1]->addPhaseOffset(temp * mods_[1]);
  waves_[2]->addPhaseOffset(temp * mods_[2]);
  waves_[3]->addPhaseOffset( twozero_.lastOut() );
  twozero_.tick( temp );
  temp =  gains_[0] * tilt_[0] * adsr_[0]->tick() * waves_[0]->tick();
  temp += gains_[1] * tilt_[1] * adsr_[1]->tick() * waves_[1]->tick();
  temp += gains_[2] * tilt_[2] * adsr_[2]->tick() * waves_[2]->tick();

  lastOutput_ = temp * 0.33;
  return lastOutput_;
}

void FMVoices :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "FMVoices::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "FMVoices::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }


  if (number == __SK_Breath_) // 2
    gains_[3] = fmGains_[(int) ( norm * 99.9 )];
  else if (number == __SK_FootControl_)	{ // 4
    currentVowel_ = (int) (norm * 128.0);
    this->setFrequency(baseFrequency_);
  }
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    tilt_[0] = norm;
    tilt_[1] = norm * norm;
    tilt_[2] = tilt_[1] * norm;
  }
  else {
    errorString_ << "FMVoices::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "FMVoices::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}
