/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This class implements a traditional ADSR (Attack, Decay, Sustain,
    Release) envelope.  It responds to simple keyOn and keyOff
    messages, keeping track of its state.  The \e state = ADSR::DONE
    after the envelope value reaches 0.0 in the ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2010.
*/
/***************************************************/

#include "ADSR.h"

namespace stk {

ADSR :: ADSR( void )
{
  target_ = 0.0;
  value_ = 0.0;
  attackRate_ = 0.001;
  decayRate_ = 0.001;
  releaseRate_ = 0.005;
  sustainLevel_ = 0.5;
  state_ = ATTACK;
  Stk::addSampleRateAlert( this );
}

ADSR :: ~ADSR( void )
{
}

void ADSR :: sampleRateChanged( StkFloat newRate, StkFloat oldRate )
{
  if ( !ignoreSampleRateChange_ ) {
    attackRate_ = oldRate * attackRate_ / newRate;
    decayRate_ = oldRate * decayRate_ / newRate;
    releaseRate_ = oldRate * releaseRate_ / newRate;
  }
}

void ADSR :: keyOn()
{
  target_ = 1.0;
  state_ = ATTACK;
}

void ADSR :: keyOff()
{
  target_ = 0.0;
  state_ = RELEASE;
}

void ADSR :: setAttackRate( StkFloat rate )
{
  if ( rate < 0.0 ) {
    errorString_ << "ADSR::setAttackRate: negative rates not allowed ... correcting!";
    handleError( StkError::WARNING );
    attackRate_ = -rate;
  }
  else attackRate_ = rate;
}

void ADSR :: setDecayRate( StkFloat rate )
{
  if ( rate < 0.0 ) {
    errorString_ << "ADSR::setDecayRate: negative rates not allowed ... correcting!";
    handleError( StkError::WARNING );
    decayRate_ = -rate;
  }
  else decayRate_ = rate;
}

void ADSR :: setSustainLevel( StkFloat level )
{
  if ( level < 0.0 ) {
    errorString_ << "ADSR::setSustainLevel: level out of range ... correcting!";
    handleError( StkError::WARNING );
    sustainLevel_ = 0.0;
  }
  else sustainLevel_ = level;
}

void ADSR :: setReleaseRate( StkFloat rate )
{
  if ( rate < 0.0 ) {
    errorString_ << "ADSR::setReleaseRate: negative rates not allowed ... correcting!";
    handleError( StkError::WARNING );
    releaseRate_ = -rate;
  }
  else releaseRate_ = rate;
}

void ADSR :: setAttackTime( StkFloat time )
{
  if ( time < 0.0 ) {
    errorString_ << "ADSR::setAttackTime: negative times not allowed ... correcting!";
    handleError( StkError::WARNING );
    attackRate_ = 1.0 / ( -time * Stk::sampleRate() );
  }
  else attackRate_ = 1.0 / ( time * Stk::sampleRate() );
}

void ADSR :: setDecayTime( StkFloat time )
{
  if ( time < 0.0 ) {
    errorString_ << "ADSR::setDecayTime: negative times not allowed ... correcting!";
    handleError( StkError::WARNING );
    decayRate_ = 1.0 / ( -time * Stk::sampleRate() );
  }
  else decayRate_ = 1.0 / ( time * Stk::sampleRate() );
}

void ADSR :: setReleaseTime( StkFloat time )
{
  if ( time < 0.0 ) {
    errorString_ << "ADSR::setReleaseTime: negative times not allowed ... correcting!";
    handleError( StkError::WARNING );
    releaseRate_ = sustainLevel_ / ( -time * Stk::sampleRate() );
  }
  else releaseRate_ = sustainLevel_ / ( time * Stk::sampleRate() );
}

void ADSR :: setAllTimes( StkFloat aTime, StkFloat dTime, StkFloat sLevel, StkFloat rTime )
{
  this->setAttackTime( aTime );
  this->setDecayTime( dTime );
  this->setSustainLevel( sLevel );
  this->setReleaseTime( rTime );
}

void ADSR :: setTarget( StkFloat target )
{
  target_ = target;
  if ( value_ < target_ ) {
    state_ = ATTACK;
    this->setSustainLevel( target_ );
  }
  if ( value_ > target_ ) {
    this->setSustainLevel( target_ );
    state_ = DECAY;
  }
}

void ADSR :: setValue( StkFloat value )
{
  state_ = SUSTAIN;
  target_ = value;
  value_ = value;
  this->setSustainLevel( value );
  lastFrame_[0] = value;
}

} // stk namespace
