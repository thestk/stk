/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "Resonate.h"
#include "SKINI.msg"

namespace stk {

Resonate :: Resonate( void )
{
  poleFrequency_ = 4000.0;
  poleRadius_ = 0.95;
  // Set the filter parameters.
  filter_.setResonance( poleFrequency_, poleRadius_, true );
  zeroFrequency_ = 0.0;
  zeroRadius_ = 0.0;
}  

Resonate :: ~Resonate( void )
{
}

void Resonate :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  adsr_.setTarget( amplitude );
  this->keyOn();
  this->setResonance( frequency, poleRadius_ );

#if defined(_STK_DEBUG_)
  errorString_ << "Resonate::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}
void Resonate :: noteOff( StkFloat amplitude )
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  errorString_ << "Resonate::NoteOff: amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Resonate :: setResonance( StkFloat frequency, StkFloat radius )
{
  poleFrequency_ = frequency;
  if ( frequency < 0.0 ) {
    errorString_ << "Resonate::setResonance: frequency parameter is less than zero ... setting to 0.0!";
    handleError( StkError::WARNING );
    poleFrequency_ = 0.0;
  }

  poleRadius_ = radius;
  if ( radius < 0.0 ) {
    std::cerr << "Resonate::setResonance: radius parameter is less than 0.0 ... setting to 0.0!";
    handleError( StkError::WARNING );
    poleRadius_ = 0.0;
  }
  else if ( radius >= 1.0 ) {
    errorString_ << "Resonate::setResonance: radius parameter is greater than or equal to 1.0, which is unstable ... correcting!";
    handleError( StkError::WARNING );
    poleRadius_ = 0.9999;
  }
  filter_.setResonance( poleFrequency_, poleRadius_, true );
}

void Resonate :: setNotch( StkFloat frequency, StkFloat radius )
{
  zeroFrequency_ = frequency;
  if ( frequency < 0.0 ) {
    errorString_ << "Resonate::setNotch: frequency parameter is less than zero ... setting to 0.0!";
    handleError( StkError::WARNING );
    zeroFrequency_ = 0.0;
  }

  zeroRadius_ = radius;
  if ( radius < 0.0 ) {
    errorString_ << "Resonate::setNotch: radius parameter is less than 0.0 ... setting to 0.0!";
    handleError( StkError::WARNING );
    zeroRadius_ = 0.0;
  }
  
  filter_.setNotch( zeroFrequency_, zeroRadius_ );
}

void Resonate :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Resonate::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Resonate::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == 2) // 2
    setResonance( norm * Stk::sampleRate() * 0.5, poleRadius_ );
  else if (number == 4) // 4
    setResonance( poleFrequency_, norm*0.9999 );
  else if (number == 11) // 11
    this->setNotch( norm * Stk::sampleRate() * 0.5, zeroRadius_ );
  else if (number == 1)
    this->setNotch( zeroFrequency_, norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( norm );
  else {
    errorString_ << "Resonate::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Resonate::controlChange: number = " << number << ", value = " << value << '.';
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
