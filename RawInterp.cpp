/*******************************************/
/*  Interpolating RawWave Class,           */
/*  by Perry R. Cook, 1995-96              */ 
/*  This object can open a raw 16bit data  */
/*  (signed integers) file, and play back  */
/*  the data once or looping, with linear  */
/*  interpolation on playback.             */
/*                                         */
/*  Made inherited from RawWave            */
/*  by Gary P. Scavone (11/11/98)          */
/*******************************************/

#include "RawInterp.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

RawInterp :: RawInterp(char *fileName)
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
  myData = 1;
  i = 0;
  while (fread(&temp,2,1,fd))   {
#ifdef __LITTLE_ENDIAN__
    temp = SwapShort (temp);
#endif
    data[i] = temp;
    i++;
  }
  data[length] = data[length-1];
  fclose(fd);
  looping = 0;
  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 1.0;
  finished = 0;
}

RawInterp :: RawInterp(MY_FLOAT *someData, long aLength)   
{
    
  length = aLength;
  data = someData;
  myData = 0;
  looping = 0;
  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 1.0;
}

RawInterp :: ~RawInterp()
{
  if (myData) {
    free(data);
    data = 0;
  }
}

void RawInterp :: reset()
{
  time = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0.0;
  finished = 0;
}

void RawInterp :: normalize()
{
  this->normalize((MY_FLOAT) 1.0);
}

void RawInterp :: normalize(MY_FLOAT newPeak)
{
  long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;
  for (i=0;i<=length;i++)
    if (fabs(data[i]) > max) 
	    max = (MY_FLOAT) fabs(data[i]);
  if (max > 0.0)       {
    max = (MY_FLOAT) 1.0 / max;
    max *= newPeak;
    for (i=0;i<=length;i++)
	    data[i] *= max;
  }
}

void RawInterp :: setRate(MY_FLOAT aRate)
{
  rate = aRate;
}

void RawInterp :: setFreq(MY_FLOAT aFreq)
{
  rate = length * (MY_FLOAT) ONE_OVER_SRATE * aFreq;
}

void RawInterp :: addTime(MY_FLOAT aTime)     /* Add an absolute time */
{                                             /*    in samples        */
  time += aTime;
}

void RawInterp :: addPhase(MY_FLOAT anAngle)  /* Add a time in cycles */
{                                             /* Cycles here means    */
  time += length * anAngle;                   /* 1.0 = length         */
}

void RawInterp :: addPhaseOffset(MY_FLOAT anAngle)
{                                           /* Add a phase offset   */
  phaseOffset = length * anAngle;           /* in cycles, where     */
}                                           /* 1.0 = length         */

void RawInterp :: setLooping(int aLoopStatus)
{
  time = (MY_FLOAT) 0;
  looping = aLoopStatus;
  if (looping) data[length] = data[0];
}

long RawInterp :: getLength()
{
  return length;
}

MY_FLOAT* RawInterp :: getData()
{
  return data;
}

MY_FLOAT RawInterp ::  tick()
{
  this->informTick();
  return lastOutput;
}

int RawInterp :: isFinished()
{
  return finished;
}

int RawInterp :: informTick()
{
  long temp;

  MY_FLOAT temp_time, alpha;
    
  time += rate;                   /*  Update current time          */
    
  if (looping)        {
    while (time >= length)        /*  Check for end of sound       */
	    time -= length;             /*  loop back to beginning       */
    while (time < 0.0)            /*  Check for end of sound       */
	    time += length;             /*  loop back to beginning       */
  }
  else {
    if (time >= length)  {        /*  Check for end of sound       */
	    time = length-(MY_FLOAT) 1; /*  stick at end                 */
	    finished = 1;               /*  Information for one-shot use */
    }
    else if (time < 0.0)          /*  Check for end of sound       */
	    time = (MY_FLOAT) 0.0;      /*  stick at beg                 */
  }

  temp_time = time;
    
  if (phaseOffset != 0.0)       {
    temp_time += phaseOffset;     /*  Add phase offset             */
    if (looping)    {
	    while (temp_time >= length) /*  Check for end of sound       */
        temp_time -= length;      /*  loop back to beginning       */
	    while (temp_time < 0.0)     /*  Check for end of sound       */
        temp_time += length;      /*  loop back to beginning       */
    }
    else    {
	    if (temp_time >= length)             /*  Check for end of sound  */
        temp_time = length - (MY_FLOAT) 1; /*  stick at end            */
	    else if (temp_time < 0.0)            /*  check for end of sound  */
        temp_time = (MY_FLOAT) 0.0;        /*  stick at beg            */
    }
  }

  temp = (long) temp_time;             /*  Integer part of time address    */
  alpha = temp_time - (MY_FLOAT) temp; /*  fractional part of time address */
  lastOutput = data[temp];             /*  Do linear interpolation         */
  lastOutput = lastOutput +            /*  same as alpha*data[temp+1]      */
    (alpha * (data[temp+1] -
              lastOutput));            /*  + (1-alpha)data[temp]           */

  return finished;
}

MY_FLOAT RawInterp :: lastOut()
{
    return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
  RawInterp loopWave("rawwaves/sinewave.raw");
  RawInterp oneShot("rawwaves/mandpluk.raw");
  FILE *fd;
  short data;
  long i;
    
  loopWave.setLooping(1);
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
    
  oneShot.setLooping(0);
  for (i=0;i<8192;i++) {
    data = oneShot.tick();
    fwrite(&data,2,1,fd);
  }
  oneShot.reset();
  oneShot.setRate(0.5);
  for (i=0;i<16384;i++) {
    data = oneShot.tick();
    fwrite(&data,2,1,fd);
  }
  fclose(fd);
}
*/
