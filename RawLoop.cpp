
/*******************************************/
/*  Raw Looped Soundfile Class,            */
/*  by Perry R. Cook, 1995-96              */ 
/*  This Object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data, looping only, with linear    */
/*  interpolation on playback.             */
/*******************************************/

#include "RawLoop.h"

#include "swapstuf.h"

RawLoop :: RawLoop(char *fileName)
{
    long i;
    short temp;
    FILE *fd;
    fd = fopen(fileName,"rb");
    if (!fd)   {
	printf("Couldn't find soundfile %s  !!!!!!!!\n",fileName);
	exit(0);
    }
    i = 0;
    while (fread(&temp,2,1,fd)) i++;
    length = i;
    fseek(fd,0,0);
    data = (MY_FLOAT *) malloc(MY_FLOAT_SIZE * (length + 1));
    i = 0;
    while (fread(&temp,2,1,fd))   {
        #ifdef __LITTLE_ENDIAN__
            temp = SwapShort (temp);
        #endif
	data[i] = temp;
	i++;
    }
    data[length] = data[0];
    fclose(fd);
    time = (MY_FLOAT) 0.0;
    phaseOffset = (MY_FLOAT) 0.0;
    rate = (MY_FLOAT) 1.0;
}

RawLoop :: ~RawLoop()
{
    free(data);
}

void RawLoop :: reset()
{
    time = (MY_FLOAT) 0.0;
    lastOutput = (MY_FLOAT) 0.0;
}

void RawLoop :: normalize()
{
    this->normalize((MY_FLOAT) 1.0);
}

void RawLoop :: normalize(MY_FLOAT newPeak)
{
    long i;
    MY_FLOAT max = (MY_FLOAT) 0.0;
    for (i=0;i<=length;i++)
	if (fabs(data[i]) > max) 
	    max = (MY_FLOAT) fabs((double) data[i]);
    if (max > 0.0)       {
	max = (MY_FLOAT) 1.0 / max;
	max *= newPeak;
	for (i=0;i<=length;i++)
	    data[i] *= max;
    }
}

void RawLoop :: setRate(MY_FLOAT aRate)
{
    rate = aRate;
}

void RawLoop :: setFreq(MY_FLOAT aFreq)
{
    rate = length * ONE_OVER_SRATE * aFreq;
}

void RawLoop :: addTime(MY_FLOAT aTime)    /* Add an absolute time */
{                                          /* in samples           */
    time += aTime;
}

void RawLoop :: addPhase(MY_FLOAT anAngle) /* Add a time in cycles */
{                                          /* Cycles here means    */
    time += length * anAngle;              /* 1.0 = length         */
}

void RawLoop :: addPhaseOffset(MY_FLOAT anAngle)
{                                          /* Add a phase offset   */
    phaseOffset = length * anAngle;        /* in cycles, where     */
}                                          /* 1.0 = length         */

MY_FLOAT RawLoop :: tick()
{
    long temp;
    
    MY_FLOAT temp_time, alpha;
    
    time += rate;                          /*  Update current time    */
    
    while (time >= length)                 /*  Check for end of sound */
	time -= length;                    /*  loop back to beginning */
    while (time < 0.0)                     /*  Check for end of sound */
	time += length;                    /*  loop back to beginning */
    
    temp_time = time;
    
    if (phaseOffset != 0.0)       {
	temp_time += phaseOffset;          /*  Add phase offset       */
	while (temp_time >= length)        /*  Check for end of sound */
	    temp_time -= length;           /*  loop back to beginning */
	while (temp_time < 0.0)            /*  Check for end of sound */
	    temp_time += length;           /*  loop back to beginning */
    }

    temp = (long) temp_time;               /*  Integer part of time address    */
    alpha = temp_time - (MY_FLOAT) temp;   /*  fractional part of time address */
    lastOutput = data[temp];               /*  Do linear interpolation         */
    lastOutput = lastOutput +              /*  same as alpha*data[temp+1]      */
	(alpha * (data[temp+1] 
		- lastOutput));            /*  + (1-alpha)data[temp]           */
    
    return lastOutput;                        
}

MY_FLOAT RawLoop :: lastOut()
{
    return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    RawLoop loopWave("rawwaves/sinewave.raw");
    FILE *fd;
    short data;
    long i;
    
    loopWave.setFreq(5500);
    fd = fopen("test.raw","wb");
    for (i=0;i<4096;i++) {
	data = loopWave.tick();
	fwrite(&data,2,1,fd);
    }
    loopWave.setFreq(2750);
    for (i=0;i<4096;i++) {
	data = loopWave.tick();
	fwrite(&data,2,1,fd);
    }
    fclose(fd);
}
*/
