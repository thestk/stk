/***************************************************/
/*! \class Plucked
    \brief STK plucked string model class.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "Plucked.h"

namespace stk {

Plucked :: Plucked( StkFloat lowestFrequency )
{
  length_ = (unsigned long) ( Stk::sampleRate() / lowestFrequency + 1 );
  loopGain_ = 0.999;
  delayLine_.setMaximumDelay( length_ );
  delayLine_.setDelay( 0.5 * length_ );
  this->clear();
}

Plucked :: ~Plucked( void )
{
}

void Plucked :: clear( void )
{
  delayLine_.clear();
  loopFilter_.clear();
  pickFilter_.clear();
}

void Plucked :: setFrequency( StkFloat frequency )
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Plucked::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  StkFloat delay = (Stk::sampleRate() / freakency) - 0.5;
  if ( delay <= 0.0 )
    delay = 0.3;
  else if ( delay > length_ )
    delay = length_;
  delayLine_.setDelay( delay );

  loopGain_ = 0.995 + (freakency * 0.000005);
  if ( loopGain_ >= 1.0 ) loopGain_ = 0.99999;
}

void Plucked :: pluck( StkFloat amplitude )
{
  StkFloat gain = amplitude;
  if ( gain > 1.0 ) {
    errorString_ << "Plucked::pluck: amplitude is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    errorString_ << "Plucked::pluck: amplitude is < 0.0  ... setting to 0.0!";
    handleError( StkError::WARNING );
    gain = 0.0;
  }

  pickFilter_.setPole( 0.999 - (gain * 0.15) );
  pickFilter_.setGain( gain * 0.5 );
  for (unsigned long i=0; i<length_; i++)
    // Fill delay with noise additively with current contents.
    delayLine_.tick( 0.6 * delayLine_.lastOut() + pickFilter_.tick( noise_.tick() ) );
}

void Plucked :: noteOn( StkFloat frequency, StkFloat amplitude )
{
  this->setFrequency( frequency );
  this->pluck( amplitude );

#if defined(_STK_DEBUG_)
  errorString_ << "Plucked::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Plucked :: noteOff( StkFloat amplitude )
{
  loopGain_ = 1.0 - amplitude;
  if ( loopGain_ < 0.0 ) {
    errorString_ << "Plucked::noteOff: amplitude is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    loopGain_ = 0.0;
  }
  else if ( loopGain_ > 1.0 ) {
    errorString_ << "Plucked::noteOff: amplitude is < 0.0  ... setting to 0.0!";
    handleError( StkError::WARNING );
    loopGain_ = (StkFloat) 0.99999;
  }

#if defined(_STK_DEBUG_)
  errorString_ << "Plucked::NoteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

} // stk namespace
