/***************************************************/
/*! \class VoicDrum
    \brief STK vocal drum sample player class.

    This class implements a drum sampling synthesizer using FileWvIn
    objects and one-pole filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately interpolated for other
    sample rates.  You can specify the maximum polyphony (maximum
    number of simultaneous voices) in VoicDrum.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "VoicDrum.h"
#include <cmath>

VoicDrum :: VoicDrum() : Instrmnt()
{
  // This counts the number of sounding voices.
  nSounding_ = 0;
  soundOrder_ = std::vector<int> (VOICE_POLYPHONY, -1);
  soundNumber_ = std::vector<int> (VOICE_POLYPHONY, -1);
}

VoicDrum :: ~VoicDrum()
{
}

char voiceNames[VOICE_NUMWAVES][11] =
  {
    "tak2.raw",
    "tak1.raw",
    "bee1.raw",
    "dee1.raw",
    "dee2.raw",
    "din1.raw",
    "gun1.raw",
    "jun1.raw",
    "jun2.raw",
    "tak3.raw",
    "tak4.raw"
  };

void VoicDrum :: noteOn( StkFloat instrument, StkFloat amplitude )
{
#if defined(_STK_DEBUG_)
  errorString_ << "VoicDrum::noteOn: instrument = " << instrument << ", amplitude = " << amplitude << '.';
  handleError( StkError::DEBUG_WARNING );
#endif

  StkFloat gain = amplitude;
  if ( amplitude > 1.0 ) {
    errorString_ << "VoicDrum::noteOn: amplitude parameter is greater than 1.0 ... setting to 1.0.";
    handleError( StkError::WARNING );
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    errorString_ << "VoicDrum::noteOn: amplitude parameter is less than 0.0 ... doing nothing.";
    handleError( StkError::WARNING );
    return;
  }

  int noteNumber = ( (int) instrument ) % 11;

  // If we already have a wave of this note number loaded, just reset
  // it.  Otherwise, look first for an unused wave or preempt the
  // oldest if already at maximum polyphony.
  int iWave;
  for ( iWave=0; iWave<VOICE_POLYPHONY; iWave++ ) {
    if ( soundNumber_[iWave] == noteNumber ) {
      if ( waves_[iWave].isFinished() ) {
        soundOrder_[iWave] = nSounding_;
        nSounding_++;
      }
      waves_[iWave].reset();
      filters_[iWave].setPole( 0.999 - (gain * 0.6) );
      filters_[iWave].setGain( gain );
      break;
    }
  }

  if ( iWave == VOICE_POLYPHONY ) { // This note number is not currently loaded.
    if ( nSounding_ < VOICE_POLYPHONY ) {
      for ( iWave=0; iWave<VOICE_POLYPHONY; iWave++ )
        if ( soundOrder_[iWave] < 0 ) break;
      nSounding_ += 1;
    }
    else {
      for ( iWave=0; iWave<VOICE_POLYPHONY; iWave++ )
        if ( soundOrder_[iWave] == 0 ) break;
      // Re-order the list.
      for ( int j=0; j<VOICE_POLYPHONY; j++ ) {
        if ( soundOrder_[j] > soundOrder_[iWave] )
          soundOrder_[j] -= 1;
      }
    }
    soundOrder_[iWave] = nSounding_ - 1;
    soundNumber_[iWave] = noteNumber;

    // Concatenate the rawwave path to the rawwave file
    waves_[iWave].openFile( (std::string("rawwaves/") + voiceNames[ noteNumber ]).c_str(), true );
    if ( Stk::sampleRate() != 22050.0 )
      waves_[iWave].setRate( 22050.0 / Stk::sampleRate() );
    filters_[iWave].setPole( 0.999 - (gain * 0.6) );
    filters_[iWave].setGain( gain );
  }

#if defined(_STK_DEBUG_)
  errorString_ << "VoicDrum::noteOn: number sounding = " << nSounding_ << '\n';
  for (i=0; i<nSounding_; i++) errorString_ << soundNumber_[i] << "  ";
  errorString_ << '\n';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void VoicDrum :: noteOff(StkFloat amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while ( i < nSounding_ ) filters_[i++].setGain( amplitude * 0.01 );
}

StkFloat VoicDrum :: computeSample()
{
  lastOutput_ = 0.0;
  if ( nSounding_ == 0 ) return lastOutput_;

  for ( int i=0; i<VOICE_POLYPHONY; i++ ) {
    if ( soundOrder_[i] >= 0 ) {
      if ( waves_[i].isFinished() ) {
        // Re-order the list.
        for ( int j=0; j<VOICE_POLYPHONY; j++ ) {
          if ( soundOrder_[j] > soundOrder_[i] )
            soundOrder_[j] -= 1;
        }
        soundOrder_[i] = -1;
        nSounding_--;
      }
      else
        lastOutput_ += filters_[i].tick( waves_[i].tick() );
    }
  }

  return lastOutput_;
}
