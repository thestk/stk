/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */
/*                                         */
/*  This instrument contains a bunch of    */
/*  RawWvIn objects, run through a bunch   */
/*  of one-pole filters.  All the          */
/*  corresponding rawwave files have been  */
/*  sampled at 22050 Hz.  Thus, if the     */
/*  compile-time SRATE = 22050, then       */
/*  no interpolation is used.  Otherwise,  */
/*  the rawwave data is appropriately      */
/*  interpolated for the current SRATE.    */
/*  You can specify the maximum Polyphony  */
/*  (maximum number of simultaneous voices)*/
/*  in a #define in the .h file.           */
/*                                         */
/*  Modified for RawWvIn class             */
/*  by Gary P. Scavone (4/99)              */
/*******************************************/

#include "VoicDrum.h"
#include <string.h>

VoicDrum :: VoicDrum() : Instrmnt()
{
  int i;

  for (i=0;i<DRUM_POLYPHONY;i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }
  /* This counts the number of sounding voices */
  numSounding = 0;

  /* Print warning about aliasing if SRATE < 22050 */
  if (SRATE < 22050) {
    printf("\nWarning: VoicDrum is designed for sampling rates of\n");
    printf("22050 Hz or greater.  You will likely encounter aliasing\n");
    printf("at the current sampling rate of %.0f Hz.\n\n", SRATE);
  }
}

VoicDrum :: ~VoicDrum()
{
  int i;
  for ( i=0; i<numSounding-1; i++ ) delete waves[i];
  for ( i=0; i<DRUM_POLYPHONY; i++ ) delete filters[i];
}

void VoicDrum :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  int i, notDone;
  int noteNum;
  int vel;
  char tempString[64];
  RawWvIn *tempWv;
  OnePole *tempFilt;
  char waveNames[DRUM_NUMWAVES][16] = { 
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

  /* Yes I know, this is tres kludgey */
  noteNum = (int)freq;
  vel = (int) (amp * 127);

#if defined(_debug_)
  printf("NoteOn: %i vel=%i\n",noteNum,vel);
#endif

  notDone = -1;
  for (i=0;i<DRUM_POLYPHONY;i++)      {        /* Check first to see     */
    if (sounding[i] == noteNum) notDone = i;   /* if there's already     */
  }                                            /* one like this sounding */

  if (notDone<0) {                             /* If not, then       */
    if (numSounding == DRUM_POLYPHONY) {       /* If we're already   */
      delete waves[0];                         /* at max polyphony,  */
      filters[0]->clear();                     /* then               */
      tempWv = waves[0];
      tempFilt = filters[0];
      for (i=0;i<DRUM_POLYPHONY-1;i++) {       /* preempt oldest     */
        waves[i] = waves[i+1];                 /* voice and   	     */
        filters[i] = filters[i+1];             /* ripple all down    */
      }
      waves[DRUM_POLYPHONY-1] = tempWv;
      filters[DRUM_POLYPHONY-1] = tempFilt;
    } else {
      numSounding += 1;                     /* otherwise just add one */
    }

    sounding[numSounding-1] = noteNum;           /* allocate new wave  */
    strcpy(tempString,"rawwaves/");
    strcat(tempString,waveNames[noteNum]);
    waves[numSounding-1] = new RawWvIn(tempString, "oneshot");
    if (SRATE != 22050) {
      waves[numSounding-1]->setRate((MY_FLOAT) (22050.0/SRATE));
    }
    waves[numSounding-1]->normalize((MY_FLOAT) 0.4);
    filters[numSounding-1]->setPole((MY_FLOAT) 0.999 - ((MY_FLOAT) vel * NORM_7 * 0.6));
    filters[numSounding-1]->setGain(vel / (MY_FLOAT) 128.0);
  }
  else {
    waves[notDone]->reset();
    filters[notDone]->setPole((MY_FLOAT) 0.999 - ((MY_FLOAT) vel * NORM_7 * 0.6));
    filters[notDone]->setGain(vel / (MY_FLOAT) 128.0);
  }

#if defined(_debug_)
  printf("Number Sounding = %i\n",numSounding);
  for (i=0;i<numSounding;i++) printf(" %i  ",sounding[i]);
  printf("\n");   
#endif
}

MY_FLOAT VoicDrum :: tick()
{
  int j, i = 0;
  MY_FLOAT output = 0.0;
  OnePole *tempFilt;

  while (i < numSounding) {
    output += filters[i]->tick(waves[i]->lastOut());
    if (waves[i]->informTick() == 1) {
      delete waves[i];
	    tempFilt = filters[i];
    
      for (j=i;j<numSounding-1;j++) {
        sounding[j] = sounding[j+1];
        waves[j] = waves[j+1];
        filters[j] = filters[j+1];
      }
      filters[j] = tempFilt;
      filters[j]->clear();
      sounding[j] = -1;
      numSounding -= 1;
      i -= 1;
    }
    i++;
  }    
  return output;
}
