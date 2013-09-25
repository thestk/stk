/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains a bunch of    */
/*  NI1sWave objects (Non-Interpolating,   */
/*  1 shot players), run through a bunch   */
/*  of one-pole filters.  You can specify  */
/*  the maximum Polyphony (maximum number  */
/*  of simultaneous voices) in a #define   */
/*  in the .h file.  			   */
/*******************************************/

#include "DrumSynt.h"
#include <string.h>

/*  Not really General MIDI yet.  Coming soon.  */
unsigned char genMIDIMap[128] = { 0,0,0,0,0,0,0,0,		// 0-7
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
				  0,0,0,0,0,0,0,0};		// 120-127
				  
char waveNames[DRUM_NUMWAVES][16] = { 
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

DrumSynt :: DrumSynt() : Instrmnt()
{
  int i;
    
  for (i=0;i<DRUM_POLYPHONY;i++)   {
    filters[i] = new OnePole;
    sounding[i] = -1;
  }                 
  numSounding = 0;      /*  This counts the number */
  /*  of sounding voices     */
}  

void DrumSynt :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  int i,notDone;
  int noteNum;
  int vel;
  char tempString[64];
  RawWvIn *tempWv;
  OnePole *tempFilt;

  noteNum = (int) ((12*log(freq/220)/log(2)) + 57.01);	 // Yes I know, this is tres kludgey
  vel = (int) (amp * 127);

#if defined(_debug_)
  printf("NoteOn: %s vel=%i\n",waveNames[genMIDIMap[noteNum]],vel);
#endif

  notDone = -1;
  for (i=0;i<DRUM_POLYPHONY;i++)      {        /* Check first to see     */
    if (sounding[i] == noteNum) notDone = i;   /* if there's already     */
  }                                            /* one like this sounding */
    
  if (notDone<0)        {                      /* If not, then           */
    if (numSounding == DRUM_POLYPHONY)   {     /* If we're already   */
      delete waves[0];                         /* at max polyphony,  */
      filters[0]->clear();                     /* then               */
      tempWv = waves[0];
      tempFilt = filters[0];
      for (i=0;i<DRUM_POLYPHONY-1;i++)   {     /* preempt oldest     */
        waves[i] = waves[i+1];                 /* voice and   	   */
        filters[i] = filters[i+1];             /* ripple all down    */
      }
      waves[DRUM_POLYPHONY-1] = tempWv;
      filters[DRUM_POLYPHONY-1] = tempFilt;
    }    
    else    {
      numSounding += 1;                    /* otherwise just add one */
    }

    sounding[numSounding-1] = noteNum;           /* allocate new wave  */
    strcpy(tempString,"rawwaves/");
    strcat(tempString,waveNames[genMIDIMap[noteNum]]);
    waves[numSounding-1] = new RawWvIn(tempString);
    waves[numSounding-1]->normalize((MY_FLOAT) 0.2);        
    filters[numSounding-1]->setPole((MY_FLOAT) 0.999 - ((MY_FLOAT) vel * NORM_7 * 0.6));
    filters[numSounding-1]->setGain(vel / (MY_FLOAT) 128.0);
  }
  else        {
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

MY_FLOAT DrumSynt :: tick()
{
  int i, j, notDone;
  MY_FLOAT output;
  OnePole *tempFilt;
    
  i = 0;
  notDone = 1;
  output = (MY_FLOAT) 0.0;
    
  if (numSounding == 0) notDone = 0;
  while (notDone && (i < numSounding))     {
        
    output += filters[i]->tick(waves[i]->lastOut());
        
    if (waves[i]->informTick() == 1)     {
#if defined(_debug_)
      printf("Wave %i %i down here\n",i,sounding[i]);
#endif
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
      if (numSounding == 0) notDone = 0;
      i -= 1;
    }
    i++;
  }    
    
  return output * 2;
}

/**************  Test Main Program  *********************/
/*
#include "miditabl.h"
#include "RawWvOut.h"
#include "Reverb.h"
#include "Noise.h"

void main()
{
    long i,j;
    DrumSynt instrument;    
    RawWvOut output("test.snd");
    Reverb reverb((MY_FLOAT) 2137);
    Noise noise;

    for (j=0;j<100;j++) {
        i = (int) (fabs(noise.tick()) * DRUM_NUMWAVES);
	instrument.noteOn(i,1.0);
        for (i=0;i<2000;i++) output.tick(reverb.tick(instrument.tick()));
    }

    for (i=0;i<22000;i++) output.tick(reverb.tick(instrument.tick()));

}
*/
