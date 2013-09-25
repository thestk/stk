/********************************************/
/*
  Audio Data Input Base Class
  by Gary P. Scavone, 1999-2000

  This class can handle multi-channel
  input.  Multi-channel input is
  interleaved in the vector "data".
  Actual data input occurs in the
  subclasses of WvIn.

  Currently, STK is only supporting a few data
  types (16-bit integer .snd, .wav, .raw, and
  .aif files and 64-bit double MAT-files).  In
  order to support more formats AND to make the
  writing of subclasses easier, a format ENUM
  could be defined and a generalized getData()
  function written within this WvIn class.  Then,
  most subclasses of WvIn would only have to
  setup the appropriate parameters and all
  other processing would happen here.
*/
/********************************************/

#include "WvIn.h"
#include <stdio.h>

WvIn :: WvIn()
{
  fd = 0;
  chunking = 0;
  readPointer = 0;
  fileSize = 0;
}

WvIn :: ~WvIn()
{
  if (fd) {
    fclose(fd);
    fd = 0;
  }
  if (data) {
    delete [ ] data;
    data = 0;
  }
  if (lastOutput) {
     delete [ ] lastOutput;
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
  /* Do nothing for streamed input ... cannot be normalized */
  if (chunking) return;

  this->normalize((MY_FLOAT) 1.0);
}

// Normalize all channels equally by the greatest magnitude in all of data
void WvIn :: normalize(MY_FLOAT newPeak)
{
  /* Do nothing for streamed input ... cannot be normalized */
  if (chunking) return;

  long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;

  for (i=0;i<channels*bufferSize;i++) {
    if (fabs(data[i]) > max)
      max = (MY_FLOAT) fabs((double) data[i]);
  }
  if (max > 0.0) {
    max = (MY_FLOAT) 1.0 / max;
    max *= newPeak;
    for (i=0;i<=channels*bufferSize;i++)
	    data[i] *= max;
  }
}

void WvIn :: setRate(MY_FLOAT aRate)
{
  rate = aRate;

  // If negative rate and at beginning of sound, move pointer to end of sound.
  if ( (rate < 0) && (time == 0.0) ) time += rate + fileSize;

  if (fmod(rate, 1.0) != 0.0) interpolate = 1;
  else interpolate = 0;
}

void WvIn :: setFreq(MY_FLOAT aFreq)
{
  // This is a looping frequency.
  if (looping) {
    rate = fileSize * (MY_FLOAT) ONE_OVER_SRATE * aFreq;
    if (fmod(rate, 1.0) != 0.0) interpolate = 1;
    else interpolate = 0;
  }
}

void WvIn :: addTime(MY_FLOAT aTime)   
{
  // Add an absolute time in samples 
  time += aTime;

  while (time < 0.0)
    time += fileSize;
  while (time >= fileSize)
    time -= fileSize;
}

void WvIn :: addPhase(MY_FLOAT anAngle)
{
  // Add a time in cycles (one cycle = fileSize) ... for looping.
  if (looping) {
    time += fileSize * anAngle;

    while (time < 0.0)
      time += fileSize;
    while (time >= fileSize)
      time -= fileSize;
  }
}

void WvIn :: addPhaseOffset(MY_FLOAT anAngle)
{
  // Add a phase offset in cycles, where 1.0 = fileSize (looping).
  if (looping)
    phaseOffset = fileSize * anAngle;
}

void WvIn :: setInterpolate(int anInterpStatus)
{
  interpolate = anInterpStatus;
}

void WvIn :: setLooping(int aLoopStatus)
{
  time = (MY_FLOAT) 0.0;
  looping = aLoopStatus;

  if (looping && !chunking) {
    for (int i=0;i<channels;i++)
       data[bufferSize*channels+i] = data[i];
  }
  else {
    for (int i=0;i<channels;i++)
      data[bufferSize*channels+i] = data[(bufferSize-1)*channels+i];
  }
}

long WvIn :: getSize()
{
  return fileSize;
}

void WvIn :: getData(long index)
{
  // implemented in subclasses
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
  static MY_FLOAT temp, alpha;
  static long index;

  if (finished) return finished;

  if (looping) {
    // Check limits of time address ... if necessary, recalculate modulo fileSize.
    while (time < 0.0)
      time += fileSize;
    while (time >= fileSize)
      time -= fileSize;

    if (phaseOffset) {
      temp = time + phaseOffset;
      while (temp < 0.0)
        temp += fileSize;
      while (temp >= fileSize)
        temp -= fileSize;
    }
    else {
      temp = time;
    }
  }
  else {
    // oneshot: if out of bounds, we're done
    if ( (time < 0.0) || (time >= fileSize) ) {
      finished = 1;
      return finished;
    }
    temp = time;
  }

  if (chunking) {
    // check the time address vs. our current buffer limits
    if ( (temp < readPointer) || (temp >= readPointer+bufferSize) )
      this->getData((long) temp);
    // adjust index for current buffer
    temp -= readPointer;
  }

  // integer part of time address
  index = (long) temp;

  if (interpolate) {
    // fractional part of time address
    alpha = temp - (MY_FLOAT) index;
    index *= channels;
    for (int i=0;i<channels;i++) {
      //  Do linear interpolation
      lastOutput[i] = data[index];
      lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
      index++;
    }
  }
  else {
    index *= channels;
    for (int i=0;i<channels;i++) {
      lastOutput[i] = data[index++];
    }
  }

  // increment time, which can be negative
  time += rate;

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
