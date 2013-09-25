/********************************************/
/*  Data Input Base Class                   */
/*  by Gary P. Scavone, 1999                */
/*                                          */
/*  This class can handle multi-channel     */
/*  input.  Multi-channel input is          */
/*  interleaved in the vector "data".       */
/*  Actual data input occurs in the         */
/*  subclasses of WvIn.                     */
/********************************************/

#include "WvIn.h"
#include <stdio.h>

WvIn :: WvIn()
{
}

WvIn :: ~WvIn()
{
  if (data) {
    delete [ ] data;
    data = 0;
  }
  if (lastOutput) {
     free(lastOutput);
     lastOutput = 0;
  }
}

void WvIn :: reset()
{
  finished = 0;
  time = (MY_FLOAT) 0.0;
  for (int i=0;i<channels;i++) {
    lastOutput[i] = (MY_FLOAT) 0.0;
  }
}

void WvIn :: normalize()
{
  this->normalize((MY_FLOAT) 1.0);
}

// Normalize all channels equally by the greatest magnitude in all of data
void WvIn :: normalize(MY_FLOAT newPeak)
{
  long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;

  for (i=0;i<channels*length;i++) {
    if (fabs(data[i]) > max)
      max = (MY_FLOAT) fabs((double) data[i]);
  }
  if (max > 0.0) {
    max = (MY_FLOAT) 1.0 / max;
    max *= newPeak;
    for (i=0;i<=channels*length;i++)
	    data[i] *= max;
  }
}

void WvIn :: setRate(MY_FLOAT aRate)
{
  rate = aRate;
  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
}

void WvIn :: setFreq(MY_FLOAT aFreq)
{
  rate = length * (MY_FLOAT) ONE_OVER_SRATE * aFreq;
  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
}

void WvIn :: addTime(MY_FLOAT aTime)          /* Add an absolute time */
{                                             /* in samples           */
  time += aTime;
}

void WvIn :: addPhase(MY_FLOAT anAngle)       /* Add a time in cycles */
{                                             /* Cycles here means    */
  time += length * anAngle;                   /* 1.0 = length         */
}

void WvIn :: addPhaseOffset(MY_FLOAT anAngle)
{                                             /* Add a phase offset  */
  phaseOffset = length * anAngle;             /* in cycles, where    */
}                                             /* 1.0 = length        */

void WvIn :: setInterpolate(int anInterpStatus)
{
  interpolate = anInterpStatus;
}

void WvIn :: setLooping(int aLoopStatus)
{
  time = (MY_FLOAT) 0.0;
  looping = aLoopStatus;

  if (looping) {
    for (int i=0;i<channels;i++)
       data[length*channels+i] = data[i];
  }
  else {
    for (int i=0;i<channels;i++)
      data[length*channels+i] = data[(length-1)*channels+i];
  }
}

long WvIn :: getLength()
{
  return length;
}

int WvIn :: isFinished()
{
  return finished;
}

MY_FLOAT WvIn :: tick()
{
  this->informTick();
  if (channels > 1) {
    MY_FLOAT tempout = 0.0;
    for (int i=0;i<channels;i++)
      tempout += lastOutput[i];
    tempout /= channels;
    return tempout;
  }
  else
    return *lastOutput;
}

MY_MULTI WvIn :: mtick()
{
  this->informTick();
  return lastOutput;
}

int WvIn :: informTick()
{
  static MY_FLOAT temp_time, alpha;
  static long temp;

  if (!finished) {

    temp_time = time;
    
    if (phaseOffset != 0.0) {
      temp_time += phaseOffset;              /*  Add phase offset        */
      if (looping) {
        while (temp_time >= length)          /*  Check for end of sound  */
          temp_time -= length;               /*  loop back to beginning  */
        while (temp_time < 0.0)              /*  Check for end of sound  */
          temp_time += length;               /*  loop back to beginning  */
      }
      else {
        if (temp_time >= length)             /*  Check for end of sound  */
          temp_time = length - (MY_FLOAT) 1; /*  stick at end            */
        else if (temp_time < 0.0)            /*  check for end of sound  */
          temp_time = (MY_FLOAT) 0.0;        /*  stick at beginning      */
      }
    }

    temp = (long) temp_time;                /*  Integer part of time address    */

    if (interpolate) {
      alpha = temp_time - (MY_FLOAT) temp;  /*  fractional part of time address */
      temp *= channels;
      for (int i=0;i<channels;i++) {
        /*  Do linear interpolation  */
        lastOutput[i] = data[temp];
        lastOutput[i] = lastOutput[i] + 
          (alpha*(data[temp+channels] - lastOutput[i]));
        temp++;
      }
    }
    else {
      temp *= channels;
      for (int i=0;i<channels;i++)
        lastOutput[i] = data[temp++];
    }

    time += rate;                   /*  Increment time               */
    if (looping)        {
      while (time >= length)        /*  Check for end of sound       */
        time -= length;             /*  loop back to beginning       */
      while (time < 0.0)            /*  Check for end of sound       */
        time += length;             /*  loop back to beginning       */
    }
    else {                          /*  OneShot                      */
      if (time >= length)  {        /*  Check for end of sound       */
        time = length-(MY_FLOAT) 1; /*  stick at end                 */
        finished = 1;               /*  Information for one-shot use */
      }
      else if (time < 0.0)          /*  Check for end of sound       */
        time = (MY_FLOAT) 0.0;      /*  stick at beginning           */
    }
  }

  return finished;
}

MY_FLOAT WvIn :: lastOut()
{
  if (channels > 1) {
    MY_FLOAT tempout = 0.0;
    for (int i=0;i<channels;i++)
      tempout += lastOutput[i];
    tempout /= channels;
    return tempout;
  }
  else
    return *lastOutput;
}

MY_MULTI WvIn :: mlastOut()
{
  return lastOutput;
}
