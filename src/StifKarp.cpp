/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "StifKarp.h"
#include "SKINI.msg"
#include <cmath>

namespace stk {

StifKarp :: StifKarp( StkFloat lowestFrequency )
{
  length_ = (unsigned long) ( Stk::sampleRate() / lowestFrequency + 1 );
  delayLine_.setMaximumDelay( length_ );
  delayLine_.setDelay( 0.5 * length_ );
  combDelay_.setMaximumDelay( length_ );
  combDelay_.setDelay( 0.2 * length_ );

  pluckAmplitude_ = 0.3;
  pickupPosition_ = 0.4;
  lastFrequency_ = lowestFrequency * 2.0;
  lastLength_ = length_ * 0.5;
  stretching_ = 0.9999;
  baseLoopGain_ = 0.995;
  loopGain_ = 0.999;

  this->clear();
}

StifKarp :: ~StifKarp( void )
{
}

void StifKarp :: clear( void )
{
  delayLine_.clear();
  combDelay_.clear();
  filter_.clear();
}

void StifKarp :: setFrequency( StkFloat frequency )
{
  lastFrequency_ = frequency; 
  if ( frequency <= 0.0 ) {
    errorString_ << "StifKarp::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    lastFrequency_ = 220.0;
  }

  lastLength_ = Stk::sampleRate() / lastFrequency_;
  StkFloat delay = lastLength_ - 0.5;
  if (delay <= 0.0)
    delay = 0.3;
  else if (delay > length_)
    delay = length_;
  delayLine_.setDelay( delay );

  loopGain_ = baseLoopGain_ + (frequency * 0.000005);
  if (loopGain_ >= 1.0) loopGain_ = 0.99999;

  setStretch(stretching_);

  combDelay_.setDelay( 0.5 * pickupPosition_ * lastLength_ ); 
}

void StifKarp :: setStretch( StkFloat stretch )
{
  stretching_ = stretch;
  StkFloat coefficient;
  StkFloat freq = lastFrequency_ * 2.0;
  StkFloat dFreq = ( (0.5 * Stk::sampleRate()) - freq ) * 0.25;
  StkFloat temp = 0.5 + (stretch * 0.5);
  if (temp > 0.9999) temp = 0.9999;
  for (int i=0; i<4; i++)	{
    coefficient = temp * temp;
    biquad_[i].setA2( coefficient );
    biquad_[i].setB0( coefficient );
    biquad_[i].setB2( 1.0 );

    coefficient = -2.0 * temp * cos(TWO_PI * freq / Stk::sampleRate());
    biquad_[i].setA1( coefficient );
    biquad_[i].setB1( coefficient );

    freq += dFreq;
  }
}

void StifKarp :: setPickupPosition( StkFloat position ) {
  pickupPosition_ = position;
  if ( position < 0.0 ) {
    errorString_ << "StifKarp::setPickupPosition: parameter is less than zero ... setting to 0.0!";
    handleError( StkError::WARNING );
    pickupPosition_ = 0.0;
  }
  else if ( position > 1.0 ) {
    errorString_ << "StifKarp::setPickupPosition: parameter is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    pickupPosition_ = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay_.setDelay( 0.5 * pickupPosition_ * lastLength_ );
}

void StifKarp :: setBaseLoopGain( StkFloat aGain )
{
  baseLoopGain_ = aGain;
  loopGain_ = baseLoopGain_ + (lastFrequency_ * 0.000005);
  if ( loopGain_ > 0.99999 ) loopGain_ = (StkFloat) 0.99999;
}

void StifKarp :: pluck( StkFloat amplitude )
{
  StkFloat gain = amplitude;
  if ( gain > 1.0 ) {
    errorString_ << "StifKarp::pluck: amplitude is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    errorString_ << "StifKarp::pluck: amplitude is less than zero ... setting to 0.0!";
    handleError( StkError::WARNING );
    gain = 0.0;
  }

  pluckAmplitude_ = amplitude;
  for (unsigned long i=0; i<length_; i++)  {
    // Fill delay with noise additively with current contents.
    delayLine_.tick( (delayLine_.lastOut() * 0.6) + 0.4 * noise_.tick() * pluckAmplitude_ );
    //delayLine_.tick( combDelay_.tick((delayLine_.lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude_) );
  }
}

void StifKarp :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );

#if defined(_STK_DEBUG_)
  errorString_ << "StifKarp::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void StifKarp :: noteOff( StkFloat amplitude )
{
  StkFloat gain = amplitude;
  if ( gain > 1.0 ) {
    errorString_ << "StifKarp::noteOff: amplitude is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    errorString_ << "StifKarp::noteOff: amplitude is < 0.0  ... setting to 0.0!";
    handleError( StkError::WARNING );
    gain = 0.0;
  }
  loopGain_ =  (1.0 - gain) * 0.5;

#if defined(_STK_DEBUG_)
  errorString_ << "StifKarp::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void StifKarp :: controlChange( int number, StkFloat value )
{
  StkFloat norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    errorString_ << "StifKarp::controlChange: control value less than zero ... setting to zero!";
    handleError( StkError::WARNING );
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    errorString_ << "StifKarp::controlChange: control value greater than 128.0 ... setting to 128.0!";
    handleError( StkError::WARNING );
  }

  if (number == __SK_PickPosition_) // 4
    this->setPickupPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    this->setBaseLoopGain( 0.97 + (norm * 0.03) );
  else if (number == __SK_StringDetune_) // 1
    this->setStretch( 0.9 + (0.1 * (1.0 - norm)) );
  else {
    errorString_ << "StifKarp::controlChange: undefined control number (" << number << ")!";
    handleError( StkError::WARNING );
  }

#if defined(_STK_DEBUG_)
    errorString_ << "StifKarp::controlChange: number = " << number << ", value = " << value << ".";
    handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
