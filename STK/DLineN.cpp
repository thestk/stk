/*******************************************/
/*  Non-Interpolating Delay Line           */
/*  Object by Perry R. Cook 1995-96.       */
/*  Revised by Gary Scavone, 1999.         */
/*                                         */
/*  This one uses either a delay line of   */
/*  maximum length specified on creation   */
/*  or a default length of 2048 samples.   */
/*  A non-interpolating delay line is      */
/*  typically used in non-time varying     */
/*  (reverb) applications.                 */
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
}

DLineN :: ~DLineN()
{
  free(inputs);
}

void DLineN :: clear()
{
  long i;
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0;
}

void DLineN :: setDelay(MY_FLOAT lag)
{
  if (lag > length-1) {                   // if delay is too big,
    printf("DLineN: Delay length too big ... setting to maximum length of %ld.\n",length-1);
    outPoint = inPoint + 1;               // force delay to max_length
  }
  else 
    outPoint = inPoint - (long) lag;      // read chases write
  while (outPoint<0) outPoint += length;  // modulo maximum length
}

MY_FLOAT DLineN :: tick(MY_FLOAT sample)  // Take one, yield one
{
  inputs[inPoint++] = sample;             // Input next sample
  if (inPoint == length)                  // Check for end condition
    inPoint -= length;
  lastOutput = inputs[outPoint++];        // Read out next value
  if (outPoint>=length)                   // Check for end condition
    outPoint -= length;                             
  return lastOutput;
}
