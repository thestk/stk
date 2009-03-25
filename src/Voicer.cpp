/***************************************************/
/*! \class Voicer
    \brief STK voice manager class.

    This class can be used to manage a group of
    STK instrument classes.  Individual voices can
    be controlled via unique note tags.
    Instrument groups can be controlled by channel
    number.

    A previously constructed STK instrument class
    is linked with a voice manager using the
    addInstrument() function.  An optional channel
    number argument can be specified to the
    addInstrument() function as well (default
    channel = 0).  The voice manager does not
    delete any instrument instances ... it is the
    responsibility of the user to allocate and
    deallocate all instruments.

    The tick() function returns the mix of all
    sounding voices.  Each noteOn returns a unique
    tag (credits to the NeXT MusicKit), so you can
    send control changes to specific voices within
    an ensemble.  Alternately, control changes can
    be sent to all voices on a given channel.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#include "Voicer.h"
#include <stdlib.h>
#include <math.h>

Voicer :: Voicer( StkFloat decayTime )
{
  tags_ = 23456;
  muteTime_ = (int) ( decayTime * Stk::sampleRate() );
}

Voicer :: ~Voicer()
{
}

void Voicer :: addInstrument( Instrmnt *instrument, int channel )
{
  Voicer::Voice voice;
  voice.instrument = instrument;
  voice.channel = channel;
  voice.noteNumber = -1;
  voices_.push_back( voice );
}

void Voicer :: removeInstrument( Instrmnt *instrument )
{
  bool found = false;
  std::vector< Voicer::Voice >::iterator i;
  for ( i=voices_.begin(); i!=voices_.end(); ++i ) {
    if ( (*i).instrument != instrument ) continue;
    voices_.erase( i );
    found = true;
    break;
  }

  if ( !found ) {
    errorString_ << "Voicer::removeInstrument: instrument pointer not found in current voices!";
    handleError( StkError::WARNING );
  }
}

long Voicer :: noteOn(StkFloat noteNumber, StkFloat amplitude, int channel )
{
  unsigned int i;
  StkFloat frequency = (StkFloat) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( i=0; i<voices_.size(); i++ ) {
    if (voices_[i].noteNumber < 0 && voices_[i].channel == channel) {
	    voices_[i].tag = tags_++;
      voices_[i].channel = channel;
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
	    voices_[i].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
      voices_[i].sounding = 1;
      return voices_[i].tag;
    }
  }

  // All voices are sounding, so interrupt the oldest voice.
  int voice = -1;
  for ( i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].channel == channel ) {
      if ( voice == -1 ) voice = i;
      else if ( voices_[i].tag < voices_[voice].tag ) voice = (int) i;
    }
  }

  if ( voice >= 0 ) {
    voices_[voice].tag = tags_++;
    voices_[voice].channel = channel;
    voices_[voice].noteNumber = noteNumber;
    voices_[voice].frequency = frequency;
    voices_[voice].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
    voices_[voice].sounding = 1;
    return voices_[voice].tag;
  }

  return -1;
}

void Voicer :: noteOff( StkFloat noteNumber, StkFloat amplitude, int channel )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].noteNumber == noteNumber && voices_[i].channel == channel ) {
      voices_[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices_[i].sounding = -muteTime_;
    }
  }
}

void Voicer :: noteOff( long tag, StkFloat amplitude )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices_[i].sounding = -muteTime_;
      break;
    }
  }
}

void Voicer :: setFrequency( StkFloat noteNumber, int channel )
{
  StkFloat frequency = (StkFloat) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].channel == channel ) {
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
      voices_[i].instrument->setFrequency( frequency );
    }
  }
}

void Voicer :: setFrequency( long tag, StkFloat noteNumber )
{
  StkFloat frequency = (StkFloat) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].noteNumber = noteNumber;
      voices_[i].frequency = frequency;
      voices_[i].instrument->setFrequency( frequency );
      break;
    }
  }
}

void Voicer :: pitchBend( StkFloat value, int channel )
{
  StkFloat pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].channel == channel )
      voices_[i].instrument->setFrequency( (StkFloat) (voices_[i].frequency * pitchScaler) );
  }
}

void Voicer :: pitchBend( long tag, StkFloat value )
{
  StkFloat pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->setFrequency( (StkFloat) (voices_[i].frequency * pitchScaler) );
      break;
    }
  }
}

void Voicer :: controlChange( int number, StkFloat value, int channel )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].channel == channel )
      voices_[i].instrument->controlChange( number, value );
  }
}

void Voicer :: controlChange( long tag, int number, StkFloat value )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].tag == tag ) {
      voices_[i].instrument->controlChange( number, value );
      break;
    }
  }
}

void Voicer :: silence( void )
{
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].sounding > 0 )
      voices_[i].instrument->noteOff( 0.5 );
  }
}

StkFloat Voicer :: tick()
{
  lastOutput_ = lastOutputLeft_ = lastOutputRight_ = 0.0;
  for ( unsigned int i=0; i<voices_.size(); i++ ) {
    if ( voices_[i].sounding != 0 ) {
      lastOutput_ += voices_[i].instrument->tick();
      lastOutputLeft_ += voices_[i].instrument->lastOutLeft();
      lastOutputRight_ += voices_[i].instrument->lastOutRight();
    }
    if ( voices_[i].sounding < 0 ) {
      voices_[i].sounding++;
      if ( voices_[i].sounding == 0 )
        voices_[i].noteNumber = -1;
    }
  }
  return lastOutput_ / voices_.size();
}

StkFloat *Voicer :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

StkFrames& Voicer :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "Voicer::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick();
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick();
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick();
  }

  return frames;
}

StkFloat Voicer :: lastOut() const
{
  return lastOutput_;
}

StkFloat Voicer :: lastOutLeft() const
{
  return lastOutputLeft_;
}

StkFloat Voicer :: lastOutRight() const
{
  return lastOutputRight_;
}

