/*******************************************/
/*  Master Class for Drum Synthesizer      */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains a bunch of    */
/*  RawWvIn objects (Non-Interpolating,   */
/*  1 shot players), run through a bunch   */
/*  of one-pole filters.  You can specify  */
/*  the maximum Polyphony (maximum number  */
/*  of simultaneous voices) in a #define   */
/*  in the .h file.  			   */
/*******************************************/

#include "DrumSynt.h"
#include <string.h>

char waveNames[DRUM_NUMWAVES][16] = { 
    "bass.raw",
    "snar.raw",
    "tomlow.raw",
    "tommid.raw",
    "tomhi.raw",
    "hat.raw",
    "ride.raw",
    "crash.raw", 
    "cowbell.raw", 
    "tamb.raw"
};

DrumSynt :: DrumSynt() : Object()
{
    int i;
/*    char temp[64];                            */

/*    for (i=0;i<DRUM_NUMWAVES;i++)  {          */
/*        strcpy(temp,"rawwaves/drumwavs/");    */
/*        strcat(temp,waveNames[i]);            */
/*        waves[i] = new RawWvIn(temp);         */
/*        waves[i]->finish();                   */
/*        waves[i]->normalize(0.2);             */
/*    }                                         */
    
    for (i=0;i<DRUM_POLYPHONY;i++)   {
        filters[i] = new OnePole;
        sounding[i] = -1;
/*        filtSounding[i] = -1;                 */
    }                 
    numSounding = 0;      /*  This counts the number */
                          /*  of sounding voices     */
}  

void DrumSynt :: noteOn(int noteNum, int vel)
{
    int i,notDone;
    char tempString[64];
    RawWvIn *tempWv;
    OnePole *tempFilt;
    
    printf("NoteOn: %s vel=%i\n",waveNames[noteNum],vel);
    
    notDone = -1;
    for (i=0;i<DRUM_POLYPHONY;i++)      {        /* Check first to see     */
        if (sounding[i] == noteNum) notDone = i; /* if there's already     */
    }                                            /* one like this sounding */
    
    if (notDone<0)        {                      /* If not, then           */
        if (numSounding == DRUM_POLYPHONY)   {       /* If we're already   */
            delete waves[0];                         /* at max polyphony,  */
            filters[0]->clear();                     /* then               */
            tempWv = waves[0];
            tempFilt = filters[0];
            for (i=0;i<DRUM_POLYPHONY-1;i++)   {     /* preempt oldest     */
                waves[i] = waves[i+1];               /* oldest voice and   */
                filters[i] = filters[i+1];           /* ripple all down    */
            }
            waves[DRUM_POLYPHONY-1] = tempWv;
            filters[DRUM_POLYPHONY-1] = tempFilt;
        }    
        else    {
            numSounding += 1;                    /* otherwise just add one */
        }

        sounding[numSounding-1] = noteNum;           /* allocate new wave  */
        strcpy(tempString,"rawwaves/drumwavs/");
        strcat(tempString,waveNames[noteNum]);
        waves[numSounding-1] = new RawWvIn(tempString);
        waves[numSounding-1]->normalize(0.2);        
        filters[numSounding-1]->setPole(0.999 - (vel / 32.0));
        filters[numSounding-1]->setGain(vel / 128.0);
    }
    else        {
        waves[notDone]->reset();
        filters[notDone]->setPole(0.999 - (vel / 32.0));
        filters[notDone]->setGain(vel / 128.0);
    }
    
    printf("Number Sounding = %i\n",numSounding);
    for (i=0;i<numSounding;i++) printf(" %i  ",sounding[i]);
    printf("\n");   
}

MY_FLOAT DrumSynt :: tick()
{
    int i, j, notDone;
    MY_FLOAT output;
    OnePole *tempFilt;
    
    i = 0;
    notDone = 1;
    output = 0.0;
    
    if (numSounding == 0) notDone = 0;
    while (notDone && (i < numSounding))     {
        
        output += waves[i]->lastOut();
        
        if (waves[i]->informTick() == 1)     {
  
 printf("Wave %i %i down here\n",i,sounding[i]);
        
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
    
    return output;
}

/**************  Test Main Program  *********************/

#include "miditabl.h"
#include "RawWvOut.h"
#include "Reverb.h"
#include "Noise.h"

int main()
{
    long i,j;
    DrumSynt instrument;    
    RawWvOut output("test.snd");
    Reverb reverb(2137);
    Noise noise;

    for (j=0;j<100;j++) {
        i = (int) (fabs(noise.tick()) * DRUM_NUMWAVES);
	instrument.noteOn(i,127);
        for (i=0;i<2000;i++) output.tick(reverb.tick(instrument.tick()));
    }

    for (i=0;i<22000;i++) output.tick(reverb.tick(instrument.tick()));

}
