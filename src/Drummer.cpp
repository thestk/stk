/***************************************************/
/*! \class Drummer
    \brief STK drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Drummer.h"
#include <math.h>

// Not really General MIDI yet.
unsigned char genMIDIMap[128] =
  { 0,0,0,0,0,0,0,0,		// 0-7
    0,0,0,0,0,0,0,0,		// 8-15
    0,0,0,0,0,0,0,0,		// 16-23
    0,0,0,0,0,0,0,0,		// 24-31
    0,0,0,0,1,0,2,0,		// 32-39
    2,3,6,3,6,4,7,4,		// 40-47
    5,8,5,0,0,0,10,0,		// 48-55
    9,0,0,0,0,0,0,0,		// 56-63
    0,0,0,0,0,0,0,0,		// 64-71
    0,0,0,0,0,0,0,0,		// 72-79
    0,0,0,0,0,0,0,0,		// 80-87
    0,0,0,0,0,0,0,0,		// 88-95
    0,0,0,0,0,0,0,0,		// 96-103
    0,0,0,0,0,0,0,0,		// 104-111
    0,0,0,0,0,0,0,0,		// 112-119
    0,0,0,0,0,0,0,0     // 120-127
  };
				  
char waveNames[DRUM_NUMWAVES][16] =
  { 
    "dope.raw",
    "bassdrum.raw",
    "snardrum.raw",
    "tomlowdr.raw",
    "tommiddr.raw",
    "tomhidrm.raw",
    "hihatcym.raw",
    "ridecymb.raw",
    "crashcym.raw", 
    "cowbell1.raw", 
    "tambourn.raw"
  };

Drummer :: Drummer() : Instrmnt()
{
  for ( int i=0; i<DRUM_POLYPHONY; i++ ) {
    filters_[i] = new OnePole;
    sounding_[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding_ = 0;
}

Drummer :: ~Drummer()
{
  int i;
  for ( i=0; i<nSounding_; i++ ) delete waves_[i];
  for ( i=0; i<DRUM_POLYPHONY; i++ ) delete filters_[i];
}

void Drummer :: noteOn(StkFloat instrument, StkFloat amplitude)
{
#if defined(_STK_DEBUG_)
  errorString_ << "Drummer::NoteOn: instrument = " << instrument << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  StkFloat gain = amplitude;
  if ( amplitude > 1.0 ) {
    errorString_ << "Drummer::noteOn: amplitude parameter is greater than 1.0 ... setting to 1.0!";
    handleError( StkError::WARNING );
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    errorString_ << "Drummer::noteOn: amplitude parameter is less than 0.0 ... doing nothing!";
    handleError( StkError::WARNING );
    return;
  }

  // Yes, this is tres kludgey.
  int noteNum = (int) ( ( 12*log(instrument/220.0)/log(2.0) ) + 57.01 );

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for ( i=0; i<DRUM_POLYPHONY; i++ ) {
    if (sounding_[i] == noteNum) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves_[waveIndex]->reset();
    filters_[waveIndex]->setPole( 0.999 - (gain * 0.6) );
    filters_[waveIndex]->setGain( gain );
  }
  else {
    if (nSounding_ == DRUM_POLYPHONY) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves_[0];
      filters_[0]->clear();
      OnePole *tempFilt = filters_[0];
      // Re-order the list.
      for ( i=0; i<DRUM_POLYPHONY-1; i++ ) {
        waves_[i] = waves_[i+1];
        filters_[i] = filters_[i+1];
      }
      waves_[DRUM_POLYPHONY-1] = 0;
      filters_[DRUM_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding_ += 1;

    sounding_[nSounding_-1] = noteNum;
    // Concatenate the STK rawwave path to the rawwave file
    waves_[nSounding_-1] = new WvIn( (Stk::rawwavePath() + waveNames[genMIDIMap[noteNum]]).c_str(), true );
    if (Stk::sampleRate() != 22050.0)
      waves_[nSounding_-1]->setRate( 22050.0 / Stk::sampleRate() );
    filters_[nSounding_-1]->setPole( 0.999 - (gain * 0.6) );
    filters_[nSounding_-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  errorString_ << "Drummer::noteOn: number sounding = " << nSounding_ << '\n';
  for (i=0; i<nSounding_; i++) errorString_ << sounding_[i] << "  ";
  errorString_ << '\n';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void Drummer :: noteOff(StkFloat amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while ( i < nSounding_ ) filters_[i++]->setGain( amplitude * 0.01 );
}

StkFloat Drummer :: tick()
{
  OnePole *tempFilt;

  int j, i = 0;
  lastOutput_ = 0.0;
  while (i < nSounding_) {
    if ( waves_[i]->isFinished() ) {
      delete waves_[i];
	    tempFilt = filters_[i];
      // Re-order the list.
      for ( j=i; j<nSounding_-1; j++ ) {
        sounding_[j] = sounding_[j+1];
        waves_[j] = waves_[j+1];
        filters_[j] = filters_[j+1];
      }
      filters_[j] = tempFilt;
      filters_[j]->clear();
      sounding_[j] = -1;
      nSounding_ -= 1;
      i -= 1;
    }
    else
      lastOutput_ += filters_[i]->tick( waves_[i]->tick() );
    i++;
  }

  return lastOutput_;
}

StkFloat *Drummer :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& Drummer :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}
