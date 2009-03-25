/***************************************************/
/*! \class FM
    \brief STK abstract FM synthesis base class.

    This class controls an arbitrary number of
    waves and envelopes, determined via a
    constructor argument.

    Control Change Numbers: 
       - Control One = 2
       - Control Two = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "FM.h"
#include "SKINI.msg"

FM :: FM( unsigned int operators )
  : nOperators_(operators)
{
  if ( nOperators_ == 0 ) {
    errorString_ << "FM: Invalid number of operators (" << operators << ") argument to constructor!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  twozero_.setB2( -1.0 );
  twozero_.setGain( 0.0 );

  // Concatenate the STK rawwave path to the rawwave file
  vibrato_ = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true );
  vibrato_->setFrequency( 6.0 );

  unsigned int j;
  adsr_.resize( nOperators_ );
  waves_.resize( nOperators_ );
  for (j=0; j<nOperators_; j++ ) {
    ratios_.push_back( 1.0 );
    gains_.push_back( 1.0 );
    adsr_[j] = new ADSR();
  }

  modDepth_ = 0.0;
  control1_ = 1.0;
  control2_ = 1.0;
  baseFrequency_ = 440.0;

  int i;
  StkFloat temp = 1.0;
  for (i=99; i>=0; i--) {
    fmGains_[i] = temp;
    temp *= 0.933033;
  }

  temp = 1.0;
  for (i=15; i>=0; i--) {
    fmSusLevels_[i] = temp;
    temp *= 0.707101;
  }

  temp = 8.498186;
  for (i=0; i<32; i++) {
    fmAttTimes_[i] = temp;
    temp *= 0.707101;
  }
}

FM :: ~FM()
{
  delete vibrato_;

  for (unsigned int i=0; i<nOperators_; i++ ) {
    delete waves_[i];
    delete adsr_[i];
  }
}

void FM :: loadWaves(const char **filenames )
{
  for (unsigned int i=0; i<nOperators_; i++ )
    waves_[i] = new WaveLoop( filenames[i], true );
}

void FM :: setFrequency(StkFloat frequency)
{    
  baseFrequency_ = frequency;

  for (unsigned int i=0; i<nOperators_; i++ )
    waves_[i]->setFrequency( baseFrequency_ * ratios_[i] );
}

void FM :: setRatio(unsigned int waveIndex, StkFloat ratio)
{
  if ( waveIndex < 0 ) {
    errorString_ << "FM::setRatio: waveIndex parameter is less than zero!";
    handleError( StkError::WARNING );
    return;
  }
  else if ( waveIndex >= nOperators_ ) {
    errorString_ << "FM:setRatio: waveIndex parameter is greater than the number of operators!";
    handleError( StkError::WARNING );
    return;
  }

  ratios_[waveIndex] = ratio;
  if (ratio > 0.0) 
    waves_[waveIndex]->setFrequency( baseFrequency_ * ratio );
  else
    waves_[waveIndex]->setFrequency( ratio );
}

void FM :: setGain(unsigned int waveIndex, StkFloat gain)
{
  if ( waveIndex < 0 ) {
    errorString_ << "FM::setGain: waveIndex parameter is less than zero!";
    handleError( StkError::WARNING );
    return;
  }
  else if ( waveIndex >= nOperators_ ) {
    errorString_ << "FM::setGain: waveIndex parameter is greater than the number of operators!";
    handleError( StkError::WARNING );
    return;
  }

  gains_[waveIndex] = gain;
}

void FM :: setModulationSpeed(StkFloat mSpeed)
{
  vibrato_->setFrequency( mSpeed );
}

void FM :: setModulationDepth(StkFloat mDepth)
{
  modDepth_ = mDepth;
}

void FM :: setControl1(StkFloat cVal)
{
  control1_ = cVal * 2.0;
}

void FM :: setControl2(StkFloat cVal)
{
  control2_ = cVal * 2.0;
}

void FM :: keyOn()
{
  for (unsigned int i=0; i<nOperators_; i++ )
    adsr_[i]->keyOn();
}

void FM :: keyOff()
{
  for (unsigned int i=0; i<nOperators_; i++ )
    adsr_[i]->keyOff();
}

void FM :: noteOff(StkFloat amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  errorString_ << "FM::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void FM :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "FM::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "FM::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_Breath_) // 2
    this->setControl1( norm );
  else if (number == __SK_FootControl_) // 4
    this->setControl2( norm );
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_)	{ // 128
    //adsr_[0]->setTarget( norm );
    adsr_[1]->setTarget( norm );
    //adsr_[2]->setTarget( norm );
    adsr_[3]->setTarget( norm );
  }
  else {
    errorString_ << "FM::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "FM::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}

