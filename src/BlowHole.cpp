/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "BlowHole.h"
#include "SKINI.msg"
#include <cmath>

namespace stk {

BlowHole :: BlowHole( StkFloat lowestFrequency )
{
  length_ = (unsigned long) ( Stk::sampleRate() / lowestFrequency + 1 );
  // delays[0] is the delay line between the reed and the register vent.
  delays_[0].setDelay( 5.0 * Stk::sampleRate() / 22050.0 );
  // delays[1] is the delay line between the register vent and the tonehole.
  delays_[1].setMaximumDelay( length_ );
  delays_[1].setDelay( length_ >> 1 );
  // delays[2] is the delay line between the tonehole and the end of the bore.
  delays_[2].setDelay( 4.0 * Stk::sampleRate() / 22050.0 );

  reedTable_.setOffset( 0.7 );
  reedTable_.setSlope( -0.3 );

  // Calculate the initial tonehole three-port scattering coefficient
  StkFloat rb = 0.0075;    // main bore radius
  StkFloat rth = 0.003;    // tonehole radius
  scatter_ = -pow(rth,2) / ( pow(rth,2) + 2*pow(rb,2) );

  // Calculate tonehole coefficients and set for initially open.
  StkFloat te = 1.4 * rth;    // effective length of the open hole
  thCoeff_ = (te*2*Stk::sampleRate() - 347.23) / (te*2*Stk::sampleRate() + 347.23);
  tonehole_.setA1(-thCoeff_);
  tonehole_.setB0(thCoeff_);
  tonehole_.setB1(-1.0);

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    // register vent radius
  te = 1.4 * r_rh;         // effective length of the open hole
  double xi = 0.0;         // series resistance term
  double zeta = 347.23 + 2*PI*pow(rb,2)*xi/1.1769;
  double psi = 2*PI*pow(rb,2)*te / (PI*pow(r_rh,2));
  StkFloat rhCoeff = (zeta - 2 * Stk::sampleRate() * psi) / (zeta + 2 * Stk::sampleRate() * psi);
  rhGain_ = -347.23 / (zeta + 2 * Stk::sampleRate() * psi);
  vent_.setA1( rhCoeff );
  vent_.setB0(1.0);
  vent_.setB1(1.0);
  // Start with register vent closed
  vent_.setGain(0.0);

  vibrato_.setFrequency((StkFloat) 5.735);
  outputGain_ = 1.0;
  noiseGain_ = 0.2;
  vibratoGain_ = 0.01;
}

BlowHole :: ~BlowHole( void )
{
}

void BlowHole :: clear( void )
{
  delays_[0].clear();
  delays_[1].clear();
  delays_[2].clear();
  filter_.tick( 0.0 );
  tonehole_.tick( 0.0 );
  vent_.tick( 0.0 );
}

void BlowHole :: setFrequency( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "BlowHole: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  StkFloat delay = (Stk::sampleRate() / freakency) * 0.5 - 3.5;
  delay -= delays_[0].getDelay() + delays_[2].getDelay();

  if (delay <= 0.0) delay = 0.3;
  else if (delay > length_) delay = length_;
  delays_[1].setDelay(delay);
}

void BlowHole :: setVent( StkFloat newValue )
{
  // This method allows setting of the register vent "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).

  StkFloat gain;

  if (newValue <= 0.0)
    gain = 0.0;
  else if (newValue >= 1.0)
    gain = rhGain_;
  else
    gain = newValue * rhGain_;

  vent_.setGain( gain );
}

void BlowHole :: setTonehole( StkFloat newValue )
{
  // This method allows setting of the tonehole "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).
  StkFloat new_coeff;

  if ( newValue <= 0.0 )
    new_coeff = 0.9995;
  else if ( newValue >= 1.0 )
    new_coeff = thCoeff_;
  else
    new_coeff = (newValue * (thCoeff_ - 0.9995)) + 0.9995;

  tonehole_.setA1( -new_coeff );
  tonehole_.setB0( new_coeff );
}

void BlowHole :: startBlowing( StkFloat amplitude, StkFloat rate )
{
  envelope_.setRate( rate );
  envelope_.setTarget( amplitude );
}

void BlowHole :: stopBlowing( StkFloat rate )
{
  envelope_.setRate( rate );
  envelope_.setTarget( 0.0 ); 
}

void BlowHole :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  this->setFrequency( frequency );
  this->startBlowing( 0.55 + (amplitude * 0.30), amplitude * 0.005 );
  outputGain_ = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  errorString_ << "BlowHole::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void BlowHole :: noteOff( StkFloat amplitude )
{
  this->stopBlowing( amplitude * 0.01 );

#if defined(_STK_DEBUG_)
  errorString_ << "BlowHole::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void BlowHole :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "BlowHole::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "BlowHole::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable_.setSlope( -0.44 + (0.26 * norm) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain_ = ( norm * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    this->setTonehole( norm );
  else if (number == __SK_ModWheel_) // 1
    this->setVent( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope_.setValue( norm );
  else {
    errorString_ << "BlowHole::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "BlowHole::controlChange: number = " << number << ", value = " << value << ".";
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
