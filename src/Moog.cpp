/***************************************************/
/*! \class Moog
    \brief STK moog-like swept filter sampling synthesis class.

    This instrument uses one attack wave, one
    looped wave, and an ADSR envelope (inherited
    from the Sampler class) and adds two sweepable
    formant (FormSwep) filters.

    Control Change Numbers: 
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "Moog.h"
#include "SKINI.msg"

Moog :: Moog()
{
  // Concatenate the STK rawwave path to the rawwave file
  attacks_.push_back( new FileWvIn( (Stk::rawwavePath() + "mandpluk.raw").c_str(), true ) );
  loops_.push_back ( new WaveLoop( (Stk::rawwavePath() + "impuls20.raw").c_str(), true ) );
  loops_.push_back ( new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), true ) ); // vibrato
  loops_[1]->setFrequency( 6.122 );

  filters_[0].setTargets( 0.0, 0.7 );
  filters_[1].setTargets( 0.0, 0.7 );

  adsr_.setAllTimes( 0.001, 1.5, 0.6, 0.250 );
  filterQ_ = 0.85;
  filterRate_ = 0.0001;
  modDepth_ = 0.0;
}  

Moog :: ~Moog()
{
}

void Moog :: setFrequency(StkFloat frequency)
{
  baseFrequency_ = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Moog::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    baseFrequency_ = 220.0;
  }

  StkFloat rate = attacks_[0]->getSize() * 0.01 * baseFrequency_ / Stk::sampleRate();
  attacks_[0]->setRate( rate );
  loops_[0]->setFrequency( baseFrequency_ );
}

void Moog :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  StkFloat temp;
    
  this->setFrequency( frequency );
  this->keyOn();
  attackGain_ = amplitude * 0.5;
  loopGain_ = amplitude;

  temp = filterQ_ + 0.05;
  filters_[0].setStates( 2000.0, temp );
  filters_[1].setStates( 2000.0, temp );

  temp = filterQ_ + 0.099;
  filters_[0].setTargets( frequency, temp );
  filters_[1].setTargets( frequency, temp );

  filters_[0].setSweepRate( filterRate_ * 22050.0 / Stk::sampleRate() );
  filters_[1].setSweepRate( filterRate_ * 22050.0 / Stk::sampleRate() );

#if defined(_STK_DEBUG_)
  errorString_ << "Moog::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Moog :: setModulationSpeed(StkFloat mSpeed)
{
  loops_[1]->setFrequency( mSpeed );
}

void Moog :: setModulationDepth(StkFloat mDepth)
{
  modDepth_ = mDepth * 0.5;
}

StkFloat Moog :: computeSample()
{
  StkFloat temp;

  if ( modDepth_ != 0.0 ) {
    temp = loops_[1]->tick() * modDepth_;    
    loops_[0]->setFrequency( baseFrequency_ * (1.0 + temp) );
  }

  temp = attackGain_ * attacks_[0]->tick();
  temp += loopGain_ * loops_[0]->tick();
  temp = filter_.tick( temp );
  temp *= adsr_.tick();
  temp = filters_[0].tick( temp );
  lastOutput_ = filters_[1].tick( temp );
  return lastOutput_ * 3.0;
}

void Moog :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Moog::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Moog::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_FilterQ_) // 2
    filterQ_ = 0.80 + ( 0.1 * norm );
  else if (number == __SK_FilterSweepRate_) // 4
    filterRate_ = norm * 0.0002;
  else if (number == __SK_ModFrequency_) // 11
    this->setModulationSpeed( norm * 12.0 );
  else if (number == __SK_ModWheel_)  // 1
    this->setModulationDepth( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( norm );
  else {
    errorString_ << "Moog::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Moog::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}
