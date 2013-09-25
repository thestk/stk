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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Drummer.h"
#include <string.h>
#include <math.h>

// Not really General MIDI yet.  Coming soon.
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
  for (int i=0; i<DRUM_POLYPHONY; i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }

  // This counts the number of sounding voices.
  nSounding = 0;
}

Drummer :: ~Drummer()
{
  int i;
  for ( i=0; i<nSounding-1; i++ ) delete waves[i];
  for ( i=0; i<DRUM_POLYPHONY; i++ ) delete filters[i];
}

void Drummer :: noteOn(MY_FLOAT instrument, MY_FLOAT amplitude)
{
#if defined(_STK_DEBUG_)
  cerr << "Drummer: NoteOn instrument = " << instrument << ", amplitude = " << amplitude << endl;
#endif

  MY_FLOAT gain = amplitude;
  if ( amplitude > 1.0 ) {
    cerr << "Drummer: noteOn amplitude parameter is greater than 1.0!" << endl;
    gain = 1.0;
  }
  else if ( amplitude < 0.0 ) {
    cerr << "Drummer: noteOn amplitude parameter is less than 0.0!" << endl;
    return;
  }

  // Yes, this is tres kludgey.
  int noteNum = (int) ((12*log(instrument/220.0)/log(2.0)) + 57.01);

  // Check first to see if there's already one like this sounding.
  int i, waveIndex = -1;
  for (i=0; i<DRUM_POLYPHONY; i++) {
    if (sounding[i] == noteNum) waveIndex = i;
  }

  if ( waveIndex >= 0 ) {
    // Reset this sound.
    waves[waveIndex]->reset();
    filters[waveIndex]->setPole((MY_FLOAT) 0.999 - (gain * 0.6));
    filters[waveIndex]->setGain(gain);
  }
  else {
    if (nSounding == DRUM_POLYPHONY) {
      // If we're already at maximum polyphony, then preempt the oldest voice.
      delete waves[0];
      filters[0]->clear();
      WvIn *tempWv = waves[0];
      OnePole *tempFilt = filters[0];
      // Re-order the list.
      for (i=0; i<DRUM_POLYPHONY-1; i++) {
        waves[i] = waves[i+1];
        filters[i] = filters[i+1];
      }
      waves[DRUM_POLYPHONY-1] = tempWv;
      filters[DRUM_POLYPHONY-1] = tempFilt;
    }
    else
      nSounding += 1;

    sounding[nSounding-1] = noteNum;
    // Concatenate the STK RAWWAVE_PATH to the rawwave file
    char path[128];
    strcpy(path, RAWWAVE_PATH);
    strcat(path, "rawwaves/");
    strcat(path, waveNames[genMIDIMap[noteNum]]);
    waves[nSounding-1] = new WvIn(path, TRUE);
    if (Stk::sampleRate() != 22050.0)
      waves[nSounding-1]->setRate( 22050.0 / Stk::sampleRate() );
    filters[nSounding-1]->setPole((MY_FLOAT) 0.999 - (gain * 0.6) );
    filters[nSounding-1]->setGain( gain );
  }

#if defined(_STK_DEBUG_)
  cerr << "Number Sounding = " << nSounding << endl;
  for (i=0; i<nSounding; i++) cerr << sounding[i] << "  ";
  cerr << "\n";
#endif
}

void Drummer :: noteOff(MY_FLOAT amplitude)
{
  // Set all sounding wave filter gains low.
  int i = 0;
  while(i < nSounding) {
    filters[i++]->setGain( amplitude * 0.01 );
  }
}

MY_FLOAT Drummer :: tick()
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
