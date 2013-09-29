/***************************************************/
/*! \class VoicDrum
    \brief STK vocal drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) in Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "VoicDrum.h"
#include <math.h>

VoicDrum :: VoicDrum() : Instrmnt()
{
  for ( int i=0; i<VOICE_POLYPHONY; i++ ) {
    filters_[i] = new OnePole;
    sounding_[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding_ = 0;
}

VoicDrum :: ~VoicDrum()
{
  int i;
  for ( i=0; i<nSounding_; i++ ) delete waves_[i];
  for ( i=0; i<VOICE_POLYPHONY; i++ ) delete filters_[i];
}

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

  int noteNum = ( (int) instrument ) % 11;

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for ( i=0; i<VOICE_POLYPHONY; i++ ) {
    if ( sounding_[i] == noteNum ) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves_[waveIndex]->reset();
    filters_[waveIndex]->setPole( 0.999 - (gain * 0.6) );
    filters_[waveIndex]->setGain( gain );
  }
  else {
    if ( nSounding_ == VOICE_POLYPHONY ) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves_[0];
      filters_[0]->clear();
      OnePole *tempFilt = filters_[0];
      // Re-order the list.
      for ( i=0; i<VOICE_POLYPHONY-1; i++ ) {
        waves_[i] = waves_[i+1];
        filters_[i] = filters_[i+1];
      }
      waves_[VOICE_POLYPHONY-1] = 0;
      filters_[VOICE_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding_ += 1;

    sounding_[nSounding_-1] = noteNum;
    // Concatenate the rawwave path to the file name.
    waves_[nSounding_-1] = new WvIn( (std::string("rawwaves/") + voiceNames[noteNum]).c_str(), true );
    waves_[nSounding_-1]->normalize(0.4);
    if (Stk::sampleRate() != 22050.0)
      waves_[nSounding_-1]->setRate( 22050.0 / Stk::sampleRate() );
    filters_[nSounding_-1]->setPole( 0.999 - (gain * 0.6) );
    filters_[nSounding_-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  errorString_ << "VoicDrum::noteOn: number sounding = " << nSounding_ << '\n';
  for (i=0; i<nSounding_; i++) errorString_ << sounding_[i] << "  ";
  errorString_ << '\n';
  handleError( StkError::DEBUG_WARNING );
#endif
}

void VoicDrum :: noteOff(StkFloat amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while ( i < nSounding_ ) filters_[i++]->setGain( amplitude * 0.01 );
}

StkFloat VoicDrum :: tick()
{
  OnePole *tempFilt;

  int j, i = 0;
  lastOutput_ = 0.0;
  while ( i < nSounding_ ) {
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

StkFloat *VoicDrum :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return Instrmnt::tick( vector, vectorSize );
}

StkFrames& VoicDrum :: tick( StkFrames& frames, unsigned int channel )
{
  return Instrmnt::tick( frames, channel );
}
