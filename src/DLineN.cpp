/*******************************************/
/*
   Non-Interpolating Delay Line
   Object by Perry R. Cook 1995-96.
   Revised by Gary Scavone, 1999.
   Added methods by Julius Smith, 2000.

   This one uses either a delay line of
   maximum length specified on creation
   or a default length of 2047 samples.
   A non-interpolating delay line is
   typically used in non-time varying
   (reverb) applications.
 */
/*******************************************/

#include "DLineN.h"

DLineN :: DLineN()
{
  // Default max delay length set to 2047.
  length = 2048;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
  currentDelay = outPoint;
}

DLineN :: DLineN(long max_length)
{
  // Writing before reading allows delays from 0 to length-1.
  // Thus, if we want to allow a delay of max_length, we need
  // a delay-line of length = max_length+1.
  length = max_length+1;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
  currentDelay = outPoint;
}

DLineN :: ~DLineN()
{
  free(inputs);
}

void DLineN :: clear(void)
{
  long i;
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0;
}

void DLineN :: setDelay(MY_FLOAT lag)
{
  if (lag > length-1) {
    // delay is too big
    printf("DLineN: Delay length too big ... setting to maximum length of %ld.\n",length-1);
    // force delay to max_length
    outPoint = inPoint + 1;
    currentDelay = length - 1;
  }
  else {
    outPoint = inPoint - (long) lag;      // read chases write
    currentDelay = lag;
  }

  while (outPoint<0) outPoint += length;  // modulo maximum length
}

MY_FLOAT DLineN :: delay(void)
{
  return currentDelay;
}

MY_FLOAT DLineN :: energy(void)
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


long DLineN :: currentInPoint(void)
{
  return inPoint;
}

long DLineN :: currentOutPoint(void)
{
  return outPoint;
}

MY_FLOAT DLineN :: contentsAt(int n)
{
  int i = n;
  if (i<0) i=0;
  if (i>= length) i = length-1;
  if (i != n) {
    fprintf(stderr,
            "DLineN: contentsAt(%d) overflows length %ld delay line\n",
            n, length);
  }
  return inputs[i];
}

MY_FLOAT DLineN :: contentsAtNowMinus(int n)
{
  /* "Now" is always where inPoint points which is not yet written. */
  /* outPoint points to "now - delay".  Thus, valid values for n are 1 to delay. */

  int i = n;
  if (i<1) i=1;
  if (i>length) i = length;
  if (i != n) {
    fprintf(stderr,
            "DLineN: contentsAtNowMinus(%d) overflows length %ld delay line\n"
            "Clipped\n", n, length);
  }
  int ndx = inPoint-i;
  if (ndx < 0) { /* Check for wraparound */
    ndx += length;
    if (ndx < 0 || ndx >= length)
      fprintf(stderr,"DLineN: contentsAtNowMinus(): can't happen\n");
  }   
  return inputs[ndx];
}

MY_FLOAT DLineN :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Check for end condition
  if (inPoint == length)
    inPoint -= length;
  // Read out next value
  lastOutput = inputs[outPoint++];

  if (outPoint>=length)
    outPoint -= length;                             
  return lastOutput;
}
