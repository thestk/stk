/*******************************************/
/*
   Linearly Interpolating Delay Line
   Object by Perry R. Cook 1995-96.
   Added methods by Julius Smith, 2000.

   This one uses a delay line of maximum
   length specified on creation, and
   linearly interpolates fractional
   length.  It is designed to be more
   efficient if the delay length is not
   changed very often.
*/
/*******************************************/

#include "DLineL.h"

DLineL :: DLineL()
{
  // Default max delay length set to 2047.
  length = 2048;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
  currentDelay = outPoint;
}

DLineL :: DLineL(long max_length)
{
  length = max_length;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
  currentDelay = outPoint;
}

DLineL :: ~DLineL()
{
  free(inputs);
}

void DLineL :: clear()
{
  long i;
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0;
}

void DLineL :: setDelay(MY_FLOAT lag)
{
  MY_FLOAT outPointer;

  if (lag > length-1) {
    // delay is too big,
    printf("DLineL: Delay length too big.\n");
    printf("Setting to maximum length of %ld.\n",length-1);
    // force delay to max_length
    outPointer = inPoint + 1;
    currentDelay = length - 1;
  }
  else {
    // read chases write
    outPointer = inPoint - lag;
    currentDelay = lag;
  }

  while (outPointer<0)
    // modulo maximum length
    outPointer += length;

  // integer part
  outPoint = (long) outPointer;
  // fractional part
  alpha = outPointer - outPoint;
  omAlpha = (MY_FLOAT) 1.0 - alpha;
}

MY_FLOAT DLineL :: delay(void)
{
  return currentDelay;
}

MY_FLOAT DLineL :: energy(void)
{
  int i;
  register MY_FLOAT e = 0;
  if (inPoint>=outPoint) {
    for (i=outPoint;i<inPoint;i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  } else {
    for (i=outPoint;i<length;i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
    for (i=0;i<inPoint;i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  }
  return e;
}

long DLineL :: currentInPoint(void)
{
  return inPoint;
}

long DLineL :: currentOutPoint(void)
{
  return outPoint;
}

MY_FLOAT DLineL :: contentsAt(int n)
{
  int i = n;
  if (i<0) i=0;
  if (i>= length) i = length-1;
  if (i != n) {
    fprintf(stderr,
            "DLineL: contentsAt(%d) overflows length %ld delay line\n",
            n, length);
  }
  return inputs[i];
}

MY_FLOAT DLineL :: contentsAtNowMinus(int n)
{
  /* "Now" is always where inPoint points which is not yet written. */
  /* outPoint points to "now - delay".  Thus, valid values for n are 1 to delay. */
  int i = n;
  if (i<1) i=1;
  if (i>length) i = length;
  if (i != n) {
    fprintf(stderr,
            "DLineL: contentsAtNowMinus(%d) overflows length %ld delay line\n"
            "Clipped\n", n,length);
  }
  int ndx = inPoint-i;
  if (ndx < 0) { /* Check for wraparound */
    ndx += length;
    if (ndx < 0 || ndx >= length)
      fprintf(stderr,"DLineL: contentsAtNowMinus(): can't happen\n");
  }   
  return inputs[ndx];
}

MY_FLOAT DLineL :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Check for end condition
  if (inPoint == length)
    inPoint -= length;

  // First 1/2 of interpolation
  lastOutput = inputs[outPoint++] * omAlpha;
  // Check for end condition
  if (outPoint<length) {
    // Second 1/2 of interpolation
    lastOutput += inputs[outPoint] * alpha;
  }                                          
  else { // if at end ...
    // Second 1/2 of interpolation
    lastOutput += inputs[0] * alpha;
    outPoint -= length;                             
  }
  return lastOutput;
}
