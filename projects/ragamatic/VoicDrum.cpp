/***************************************************/
/*! \class VoicDrum
    \brief STK vocal drum sample player class.

    This class implements a drum sampling
    synthesizer using WvIn objects and one-pole
    filters.  The drum rawwave files are sampled
    at 22050 Hz, but will be appropriately
    interpolated for other sample rates.  You can
    specify the maximum polyphony (maximum number
    of simultaneous voices) via a #define in the
    Drummer.h.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "VoicDrum.h"
#include <string.h>
#include <math.h>

VoicDrum :: VoicDrum() : Instrmnt()
{
  for (int i=0; i<VOICE_POLYPHONY; i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding = 0;
}

VoicDrum :: ~VoicDrum()
{
  int i;
  for ( i=0; i<nSounding-1; i++ ) delete waves[i];
  for ( i=0; i<VOICE_POLYPHONY; i++ ) delete filters[i];
}

void VoicDrum :: noteOn(MY_FLOAT instrument, MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  cerr << "VoicDrum: NoteOn instrument = " << instrument << ", amplitude = " << amplitude << endl;
#endif

  MY_FLOAT gain = amplitude;
  if ( amplitude > 1.0 ) {
    cerr << "VoicDrum: noteOn amplitude parameter is greater than 1.0!" << endl;
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    cerr << "VoicDrum: noteOn amplitude parameter is less than 0.0!" << endl;
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

  int noteNum = ((int) instrument) % 11;

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for (i=0; i<VOICE_POLYPHONY; i++) {
    if (sounding[i] == noteNum) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves[waveIndex]->reset();
    filters[waveIndex]->setPole((MY_FLOAT) 0.999 - (gain * 0.6));
    filters[waveIndex]->setGain(gain);
  }
  else {
    if (nSounding == VOICE_POLYPHONY) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves[0];
      filters[0]->clear();
      WvIn *tempWv = waves[0];
      OnePole *tempFilt = filters[0];
      // Re-order the list.
      for (i=0; i<VOICE_POLYPHONY-1; i++) {
        waves[i] = waves[i+1];
        filters[i] = filters[i+1];
      }
      waves[VOICE_POLYPHONY-1] = tempWv;
      filters[VOICE_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding += 1;

    sounding[nSounding-1] = noteNum;
    // Concatenate the STK RAWWAVE_PATH to the rawwave file
    char path[128];
    strcpy(path, "rawwaves/");
    strcat(path, voiceNames[noteNum]);
    waves[nSounding-1] = new WvIn(path, TRUE);
    waves[nSounding-1]->normalize(0.4);
    filters[nSounding-1]->setPole((MY_FLOAT) 0.999 - (gain * 0.6) );
    filters[nSounding-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  cerr << "Number Sounding = " << nSounding << endl;
  for (i=0; i<nSounding; i++) cerr << sounding[i] << "  ";
  cerr << "\n";
#endif
}

void VoicDrum :: noteOff(MY_FLOAT amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while(i < nSounding) {
    filters[i++]->setGain( amplitude * 0.01 );
  }
}

MY_FLOAT VoicDrum :: tick()
{
  MY_FLOAT output = 0.0;
  OnePole *tempFilt;

  int j, i = 0;
  while (i < nSounding) {
    if ( waves[i]->isFinished() ) {
      delete waves[i];
	    tempFilt = filters[i];
      // Re-order the list.
      for (j=i; j<nSounding-1; j++) {
        sounding[j] = sounding[j+1];
        waves[j] = waves[j+1];
        filters[j] = filters[j+1];
      }
      filters[j] = tempFilt;
      filters[j]->clear();
      sounding[j] = -1;
      nSounding -= 1;
      i -= 1;
    }
    else
      output += filters[i]->tick( waves[i]->tick() );
    i++;
  }

  return output;
}
