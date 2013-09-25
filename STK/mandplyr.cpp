/********************************************/  
/*  MandPlyr Player Expert Object to control*/
/*  commuted dual plucked-string model      */
/*  by Perry Cook, 1995-96                  */
/********************************************/  

#include "Instrmnt.h"
#include "Mandolin.h"
#include "mandplyr.h"
#include "VoicMang.h"
#include "Noise.h"
#include "SKINI11.msg"

MandPlyr :: MandPlyr() : Instrmnt()
{
    short i;
    strings = new VoicMang(NUM_STRINGS,"Mandolin");
    noise = new Noise;
    strumming = 0;
    strumRate = (long) (SRATE * 0.1);            // ( 1/(0.1) = 10Hz)
    strumCount = 0;
    skill = (MY_FLOAT) 0.90;
    for (i=0;i<NUM_STRINGS;i++) {
	nums[i] = 0;
	amps[i]  = (MY_FLOAT) 0;
	tags[i]  = -1;
    }
}

MandPlyr :: ~MandPlyr()
{
    delete strings;
    delete noise;
}

void MandPlyr :: noteOnN(short num, MY_FLOAT amp)
{
    long temp;
    short i,notDone;
    MY_FLOAT isErr,errDir;
    short error = 0;
    
    errDir = noise->tick();
    if (errDir < 0) 
	isErr = -errDir;
    else 
	isErr = errDir;
    if (isErr < (0.4 * (1.0 - skill)))  {   /* Should we err here? */
	    error = 1;
	    if (strumming < 1) {
		strumming = 2;
	        strumCount = strumRate;
 	    }
	    if (errDir < 0.0) error = -1;  /* random error +/- 1/2 step */
    }
    
    
    i = 0;
    notDone = 1;
    while (notDone && i<NUM_STRINGS) {
	    if (tags[i] < 0) {
	        nums[i] = num;
	        amps[i] = amp;
	        tags[i] = strings->noteOnN((MY_FLOAT) num + error,amp);
		notDone = 0;
	        if (tags[i]<0) 	{
		    printf("Must Reallocate a note\n");
		    notDone = 1;
		}
	    }
	    i++;
    }
    if (notDone)    {
	    temp = strings->oldestVoice();
	    i = 0;
	    while (notDone && i<NUM_STRINGS) {
	        if (tags[i] < 0) {
		        strings->kill(temp);
		        nums[i] = num;
		        amps[i] = amp;
		        tags[i] = strings->noteOnN((MY_FLOAT) num + error,amp);
		        notDone = 0;
	        }
	        i++;
	    }
    }
    if (notDone) printf("Can't allocate a new note\n");
}

void MandPlyr :: noteOffN(short num,MY_FLOAT amp)
{
    long temp;
    short i;
    temp = strings->noteOffN(num,amp);
    if (temp>0)	{				// Usual Case
	for (i=0;i<NUM_STRINGS;i++)      {
	    if (tags[i] == temp)	{
	    	tags[i] = -1;
	    	nums[i] = 0;
	    	amps[i] = (MY_FLOAT) 0.0;
	    }
        }
    }
    else	{			   // In case note already off 
	for (i=0;i<NUM_STRINGS;i++)      { //    from strumming
	    if (nums[i] == num)	{		// Just clear its entry
	    	strings->noteOffT(num,amp);
	    	tags[i] = -1;
	    	nums[i] = 0;
	    	amps[i] = (MY_FLOAT) 0.0;
	    }
        }
    }
    if (strumming == 2)	strumming = 0;
}

MY_FLOAT MandPlyr :: tick()
{
    short i;
    MY_FLOAT temp2;
    if (strumming > 0) {
	strumCount -= 1;
	if (strumCount <= 0) {
	    for (i=0;i<NUM_STRINGS;i++)   {
		if (tags[i] > 0) {
		    strings->kill(tags[i]);
		    temp2 = (MY_FLOAT) fabs(noise->tick());
		    if (temp2 > 0.2 * (1.0 - skill))
			tags[i] = strings->noteOnN(nums[i],amps[i]);
	        }
	    }
	    strumCount = strumRate;
	    strumCount = strumCount +
	        (long) (strumCount * (noise->tick() * (1.0 - skill)));
	    if (strumming == 2) strumming = 0;
	}
    }
    for (i=0;i<NUM_STRINGS;i++)	{
        if (tags[i] < -1) {
	    tags[i] += 1;
	    if (tags[i] == -1)	{
		tags[i] = strings->noteOnN(nums[i],amps[i]);
	    }
	}
    }
    lastOutput = strings->tick();
    return lastOutput;
}


/*    Some very basic Mandolin Chords   */
/* Base Strings:	G  D  A  E			*/
/* G Chord:			G  D  B  G			*/
/* C Chord:			G  E  C  G			*/
/* D Chord:			A  D  A  F#			*/
/* E Chord:			G# E  B  G#			*/
/* A Chord:			A  E  C# A			*/
/* B Chord:			B  D# B  F#			*/

#define NUM_CHORDS 6

void MandPlyr :: playChord(MY_FLOAT amp, char *chordString)
{
    short i;
	char chordStr[NUM_CHORDS+1] = {"GCDEAB"};
	short chords[NUM_CHORDS][4] = {
		{55,62,71,79},
		{55,64,72,79},
		{57,62,69,78},
		{56,64,71,80},
		{57,64,73,81},
		{59,63,71,78}				};
    short whichChord = -1;

    for (i=0;i<4;i++)
		if (tags[i] >= 0) strings->noteOffT(tags[i],amp);

    for (i=0;i<NUM_CHORDS;i++)	{
		if (chordString[0]==chordStr[i])
			whichChord = i;
    }

    if (chordString[0] == 0)	{
    }
    else	{
	if (whichChord == -1) {
            printf("I don't know this chord: %s\n",chordString);
        }
    	else {
	    for (i=0;i<4;i++)	{
	    	nums[i] = chords[whichChord][i];
	    	tags[i] = -2 - (i * 10 * (200 - (int) amp));
	    	amps[i] = amp;
	    }
    	}
    }
}

void MandPlyr :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("MandPlyr : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    
    if (number == __SK_BodySize_)
	    strings->controlChange(number,value);
    else if (number == __SK_PickPosition_)
	    strings->controlChange(number,value);
    else if (number == __SK_StringDamping_)
	    strings->controlChange(number,value);
    else if (number == __SK_StringDetune_)
	    strings->controlChange(number,value);
    
    else if (number == __SK_Strumming_)   {
	    if (value < 0.5) 
	        strumming = 0;
	    else
	        strumming = 1;
    }
    else if (number == __SK_NotStrumming_)
	    strumming = 0;
    else if (number == __SK_PlayerSkill_)
	    skill = value * (MY_FLOAT) NORM_7;
    else if (number == __SK_StrumSpeed_)
        strumRate = (long) (SRATE * value * NORM_7); // (0 - 1Hz.)
    else    {        
	    printf("MandPlyr : Undefined Control Number!!\n");
    }   
}
