/***************************************************/
/*! \class Drone
    \brief STK "drone" plucked string model.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Drone.h"

Drone :: Drone(StkFloat lowestFrequency)
{
  length_ = (unsigned long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain_ = 0.999;
  delayLine_.setMaximumDelay( length_ );
  delayLine_.setDelay( 0.5 * length_ );
  envelope_.setAllTimes( 2.0, 0.5, 0.0, 0.5 );
  this->clear();
}

Drone :: ~Drone()
{
}

void Drone :: clear()
{
  delayLine_.clear();
  loopFilter_.clear();
}

void Drone :: setFrequency(StkFloat frequency)
{
  StkFloat freakency = frequency;
  if ( frequency <= 0.0 ) {
    errorString_ << "Drone::setFrequency: parameter is less than or equal to zero!";
    handleError( StkError::WARNING );
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  StkFloat delay = (Stk::sampleRate() / freakency) - 0.5;
  if ( delay <= 0.0 )
    delay = 0.3;
  else if (delay > length_)
    delay = length_;
  delayLine_.setDelay( delay );
  loopGain_ = 0.997 + (freakency * 0.000002);
  if ( loopGain_ >= 1.0 ) loopGain_ = 0.99999;
}

void Drone :: pluck(StkFloat amplitude)
{
  envelope_.keyOn();
}

void Drone :: noteOn(StkFloat frequency, StkFloat amplitude)
{
  this->setFrequency( frequency );
  this->pluck( amplitude );

#if defined(_STK_DEBUG_)
  errorString_ << "Drone::NoteOn: frequency = " << frequency << ", amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Drone :: noteOff(StkFloat amplitude)
{
  loopGain_ = 1.0 - amplitude;
  if ( loopGain_ < 0.0 ) {
    errorString_ << "Drone::noteOff: amplitude is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    loopGain_ = 0.0;
  }
  else if ( loopGain_ > 1.0 ) {
    errorString_ << "Drone::noteOff: amplitude is < 0.0  ... setting to 0.0!";
    handleError( StkError::WARNING );
    loopGain_ = 0.99999;
  }

#if defined(_STK_DEBUG_)
  errorString_ << "Drone::noteOff: amplitude = " << amplitude << ".";
  handleError( StkError::DEBUG_WARNING );
#endif
}

StkFloat Drone :: tick()
{
  // Here's the whole inner loop of the instrument!!
  lastOutput_ = delayLine_.tick( loopFilter_.tick( delayLine_.lastOut() * loopGain_ ) + (0.005 * envelope_.tick() * noise_.tick())); 
  return lastOutput_;
}

StkFloat *Drone :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& Drone :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}
