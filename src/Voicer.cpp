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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Voicer.h"
#include <stdlib.h>
#include <math.h>

Voicer :: Voicer( int maxInstruments, MY_FLOAT decayTime )
{
  nVoices = 0;
  maxVoices = maxInstruments;
  voices = (Voice *) new Voice[maxVoices];
  tags = 23456;
  muteTime = (int) ( decayTime * Stk::sampleRate() );
}

Voicer :: ~Voicer()
{
  delete [] voices;
  //free( voices );
}

void Voicer :: addInstrument( Instrmnt *instrument, int channel )
{
  //voices = (Voice *) realloc( (void *) voices, nVoices+1 * sizeof( Voice ) );
  if ( nVoices == maxVoices ) {
    cerr << "Voicer: Maximum number of voices already added!!" << endl;
    return;
  }

  voices[nVoices].instrument = instrument;
  voices[nVoices].tag = 0;
  voices[nVoices].channel = channel;
  voices[nVoices].noteNumber = -1;
  voices[nVoices].frequency = 0.0;
  voices[nVoices].sounding = 0;
  nVoices++;
}

void Voicer :: removeInstrument( Instrmnt *instrument )
{
  bool found = false;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].instrument == instrument ) found = true;
    if ( found && i+1 < nVoices ) {
      voices[i].instrument = voices[i+1].instrument;
      voices[i].tag = voices[i+1].tag;
      voices[i].noteNumber = voices[i+1].noteNumber;
      voices[i].frequency = voices[i+1].frequency;
      voices[i].sounding = voices[i+1].sounding;
      voices[i].channel = voices[i+1].channel;
    }
  }

  if ( found )
    nVoices--;
    //voices = (Voice *) realloc( voices, --nVoices * sizeof( Voice ) );

}

long Voicer :: noteOn(MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  int i;
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( i=0; i<nVoices; i++ ) {
    if (voices[i].noteNumber < 0 && voices[i].channel == channel) {
	    voices[i].tag = tags++;
      voices[i].channel = channel;
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
	    voices[i].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
      voices[i].sounding = 1;
      return voices[i].tag;
    }
  }

  // All voices are sounding, so interrupt the oldest voice.
  int voice = -1;
  for ( i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      if ( voice == -1 ) voice = i;
      else if ( voices[i].tag < voices[voice].tag ) voice = i;
    }
  }

  if ( voice >= 0 ) {
    voices[voice].tag = tags++;
    voices[voice].channel = channel;
    voices[voice].noteNumber = noteNumber;
    voices[voice].frequency = frequency;
    voices[voice].instrument->noteOn( frequency, amplitude * ONE_OVER_128 );
    voices[voice].sounding = 1;
    return voices[voice].tag;
  }

  return -1;
}

void Voicer :: noteOff( MY_FLOAT noteNumber, MY_FLOAT amplitude, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].noteNumber == noteNumber && voices[i].channel == channel ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
    }
  }
}

void Voicer :: noteOff( long tag, MY_FLOAT amplitude )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->noteOff( amplitude * ONE_OVER_128 );
      voices[i].sounding = -muteTime;
      break;
    }
  }
}

void Voicer :: setFrequency( MY_FLOAT noteNumber, int channel )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
    }
  }
}

void Voicer :: setFrequency( long tag, MY_FLOAT noteNumber )
{
  MY_FLOAT frequency = (MY_FLOAT) 220.0 * pow( 2.0, (noteNumber - 57.0) / 12.0 );
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].noteNumber = noteNumber;
      voices[i].frequency = frequency;
      voices[i].instrument->setFrequency( frequency );
      break;
    }
  }
}

void Voicer :: pitchBend( MY_FLOAT value, int channel )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
  }
}

void Voicer :: pitchBend( long tag, MY_FLOAT value )
{
  MY_FLOAT pitchScaler;
  if ( value < 64.0 )
    pitchScaler = pow(0.5, (64.0-value)/64.0);
  else
    pitchScaler = pow(2.0, (value-64.0)/64.0);
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->setFrequency( (MY_FLOAT) (voices[i].frequency * pitchScaler) );
      break;
    }
  }
}

void Voicer :: controlChange( int number, MY_FLOAT value, int channel )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].channel == channel )
      voices[i].instrument->controlChange( number, value );
  }
}

void Voicer :: controlChange( long tag, int number, MY_FLOAT value )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].tag == tag ) {
      voices[i].instrument->controlChange( number, value );
      break;
    }
  }
}

void Voicer :: silence( void )
{
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding > 0 )
      voices[i].instrument->noteOff( 0.5 );
  }
}

MY_FLOAT Voicer :: tick()
{
  lastOutput = 0.0;
  for ( int i=0; i<nVoices; i++ ) {
    if ( voices[i].sounding != 0 )
      lastOutput += voices[i].instrument->tick();
    if ( voices[i].sounding < 0 ) {
      voices[i].sounding++;
      if ( voices[i].sounding == 0 )
        voices[i].noteNumber = -1;
    }
  }
  return lastOutput / nVoices;
}

MY_FLOAT *Voicer :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Voicer :: lastOut() const
{
  return lastOutput;
}

