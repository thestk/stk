/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "Clarinet.h"
#include "SKINI.msg"

namespace stk {

Clarinet :: Clarinet( StkFloat lowestFrequency )
{
  length_ = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine_.setMaximumDelay( length_ );
  delayLine_.setDelay( length_ / 2.0 );
  reedTable_.setOffset((StkFloat) 0.7);
  reedTable_.setSlope((StkFloat) -0.3);

  vibrato_.setFrequency((StkFloat) 5.735);
  outputGain_ = (StkFloat) 1.0;
  noiseGain_ = (StkFloat) 0.2;
  vibratoGain_ = (StkFloat) 0.1;
}

Clarinet :: ~Clarinet( void )
{
}

void Clarinet :: clear( void )
{
  delayLine_.clear();
  filter_.tick( 0.0 );
}

void Clarinet :: setFrequency( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Clarinet::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  StkFloat delay = (Stk::sampleRate() / freakency) * 0.5 - 1.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length_) delay = length_;
  delayLine_.setDelay(delay);
}

void Clarinet :: startBlowing( StkFloat amplitude, StkFloat rate )
{
  envelope_.setRate(rate);
  envelope_.setTarget(amplitude); 
}

void Clarinet :: stopBlowing( StkFloat rate )
{
  envelope_.setRate(rate);
  envelope_.setTarget((StkFloat) 0.0); 
}

void Clarinet :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  this->setFrequency(frequency);
  this->startBlowing((StkFloat) 0.55 + (amplitude * (StkFloat) 0.30), amplitude * (StkFloat) 0.005);
  outputGain_ = amplitude + (StkFloat) 0.001;

#if defined(_STK_DEBUG_)
  errorString_ << "Clarinet::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Clarinet :: noteOff( StkFloat amplitude )
{
  this->stopBlowing( amplitude * 0.01 );

#if defined(_STK_DEBUG_)
  errorString_ << "Clarinet::NoteOff: amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Clarinet :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Clarinet::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Clarinet::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable_.setSlope((StkFloat) -0.44 + ( (StkFloat) 0.26 * norm ));
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = (norm * (StkFloat) 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency((norm * (StkFloat) 12.0));
  else if (number == __SK_ModWheel_) // 1
    vibratoGain_ = (norm * (StkFloat) 0.5);
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope_.setValue(norm);
  else {
    errorString_ << "Clarinet::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Clarinet::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
