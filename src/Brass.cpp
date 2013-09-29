/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "Brass.h"
#include "SKINI.msg"
#include <cmath>

namespace stk {

Brass :: Brass( StkFloat lowestFrequency )
{
  length_ = (unsigned long) ( Stk::sampleRate() / lowestFrequency + 1 );
  delayLine_.setMaximumDelay( length_ );
  delayLine_.setDelay( 0.5 * length_ );

  lipFilter_.setGain( 0.03 );
  dcBlock_.setBlockZero();

  adsr_.setAllTimes( 0.005, 0.001, 1.0, 0.010 );

  vibrato_.setFrequency( 6.137 );
  vibratoGain_ = 0.0;

  this->clear();
	maxPressure_ = 0.0;
  lipTarget_ = 0.0;

  // This is necessary to initialize variables.
  this->setFrequency( 220.0 );
}

Brass :: ~Brass( void )
{
}

void Brass :: clear( void )
{
  delayLine_.clear();
  lipFilter_.clear();
  dcBlock_.clear();
}

void Brass :: setFrequency( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Brass::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  // Fudge correction for filter delays.
  slideTarget_ = (Stk::sampleRate() / freakency * 2.0) + 3.0;
  delayLine_.setDelay( slideTarget_ ); // play a harmonic

  lipTarget_ = freakency;
  lipFilter_.setResonance( freakency, 0.997 );
}

void Brass :: setLip( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Brass::setLip: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  lipFilter_.setResonance( freakency, 0.997 );
}

void Brass :: startBlowing( StkFloat amplitude, StkFloat rate )
{
  adsr_.setAttackRate( rate );
  maxPressure_ = amplitude;
  adsr_.keyOn();
}

void Brass :: stopBlowing( StkFloat rate )
{
  adsr_.setReleaseRate( rate );
  adsr_.keyOff();
}

void Brass :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( amplitude, amplitude * 0.001 );

#if defined(_STK_DEBUG_)
  errorString_ << "Brass::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Brass :: noteOff( StkFloat amplitude )
{
  this->stopBlowing( amplitude * 0.005 );

#if defined(_STK_DEBUG_)
  errorString_ << "Brass::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Brass :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "Brass::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "Brass::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_LipTension_)	{ // 2
    StkFloat temp = lipTarget_ * pow( 4.0, (2.0 * norm) - 1.0 );
    this->setLip(temp);
  }
  else if (number == __SK_SlideLength_) // 4
    delayLine_.setDelay( slideTarget_ * (0.5 + norm) );
  else if (number == __SK_ModFrequency_) // 11
    vibrato_.setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_ ) // 1
    vibratoGain_ = norm * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr_.setTarget( norm );
  else {
    errorString_ << "Brass::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "Brass::controlChange: number = " << number << ", value = " << value << ".";
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
