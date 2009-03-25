/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Simple.h"
#include "SKINI.msg"

Simple :: Simple()
{
  // Concatenate the STK rawwave path to the rawwave file
  loop_ = new WaveLoop( (Stk::rawwavePath() + "impuls10.raw").c_str(), true );

  filter_.setPole( 0.5 );
  baseFrequency_ = 440.0;
  setFrequency( baseFrequency_ );
  loopGain_ = 0.5;
}  

Simple :: ~Simple()
{
  delete loop_;
}

void Simple :: keyOn()
{
  adsr_.keyOn();
}

void Simple :: keyOff()
{
  adsr_.keyOff();
}

void Simple :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  this->keyOn();
  this->setFrequency( frequency );
  filter_.setGain( amplitude ); 

#if defined(_STK_DEBUG_)
  errorString_ << "Simple::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}
void Simple :: noteOff(StkFloat amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  errorString_ << "Simple::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Simple :: setFrequency(StkFloat frequency)
{
  biquad_.setResonance( frequency, 0.98, true );
  loop_->setFrequency( frequency );
}

StkFloat Simple :: tick()
{
  lastOutput_ = loopGain_ * loop_->tick();
  biquad_.tick( noise_.tick() );
  lastOutput_ += (1.0 - loopGain_) * biquad_.lastOut();
  lastOutput_ = filter_.tick( lastOutput_ );
  lastOutput_ *= adsr_.tick();
  return lastOutput_;
}

StkFloat *Simple :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& Simple :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}

void Simple :: controlChange(int number, StkFloat value)
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Simple::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Simple::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_Breath_) // 2
    filter_.setPole( 0.99 * (1.0 - (norm * 2.0)) );
  else if (number == __SK_NoiseLevel_) // 4
    loopGain_ = norm;
  else if (number == __SK_ModFrequency_) { // 11
    norm /= 0.2 * Stk::sampleRate();
    adsr_.setAttackRate( norm );
    adsr_.setDecayRate( norm );
    adsr_.setReleaseRate( norm );
  }
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( norm );
  else {
    errorString_ << "Simple::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Simple::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}
