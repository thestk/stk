/*******************************************/
/*  AllPass Interpolating Delay Line       */
/*  Object by Perry R. Cook 1995-96.       */
/*  Revised by Gary P. Scavone, 1999.      */
/*                                         */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */
/*  interpolates fractional length using   */
/*  an all-pass filter.  This version is   */
/*  more efficient for computing static    */
/*  length delay lines (alpha and coeff    */
/*  are computed only when the length      */
/*  is set, there probably is a more       */
/*  efficient computational form if alpha  */
/*  is changed often (each sample)).       */
/*                                         */
/*******************************************/

#include "DLineA.h"

DLineA :: DLineA()
{
  long i;
  // Default max delay length set to 2047.
  length = 2048;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0;
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
}

DLineA :: DLineA(long max_length)
{
  long i;
  length = max_length;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0;
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
}

DLineA :: ~DLineA()
{
  free(inputs);
}

void DLineA :: clear()
{
  long i;
  for (i=0;i<length;i++) inputs[i] = (MY_FLOAT) 0.0;
  lastIn = (MY_FLOAT) 0;
  lastOutput = (MY_FLOAT) 0;
}

void DLineA :: setDelay(MY_FLOAT lag)  
{
  MY_FLOAT outPointer;

  if (lag > length-1) {                   // if delay is too big,
    printf("DLineA: Delay length too big.\n");
    printf("Setting to maximum length of %ld.\n",length-1);
    outPointer = inPoint - 18.0;          // force delay to max_length
  }
  else if (lag < 0.1) {
    printf("DLineA: Delays < 0.1 not possible with current structure.\n");
    printf("Setting delay length to 0.1.\n");
    outPointer = inPoint + 0.8999999999;
  }
  else
    outPointer = inPoint - lag + 1.0;     // outPoint chases inpoint

  if (outPointer < 0)
    outPointer += length;                 // modulo table length
  outPoint = (long) outPointer;           // Integer part of delay
  alpha = 1.0 + outPoint - outPointer;    // fractional part of delay

  if (alpha == 0.0) {  // exact integer delay
    outPoint -= 1;
    if (outPoint < 0) outPoint += length;
  }

  if (alpha<0.1) {                             // Hack to avoid pole/zero
    outPoint += 1;                             // cancellation.  Keeps allpass
    if (outPoint >= length) outPoint -= length;
    alpha += (MY_FLOAT) 1.0;                   // delay in range of .1 to 1.1
  }

  coeff = ((MY_FLOAT) 1.0 - alpha) / 
    ((MY_FLOAT) 1.0 + alpha);         // coefficient for all pass
}

MY_FLOAT DLineA :: tick(MY_FLOAT sample) // Take sample, yield sample
{
  MY_FLOAT temp;
  inputs[inPoint++] = sample;            // Write input sample
  if (inPoint == length)                 // Increment input pointer
    inPoint -= length;                   // modulo length
  temp = inputs[outPoint++];             // filter input
  if (outPoint == length)                // Increment output pointer
    outPoint -= length;                  // modulo length
  lastOutput = -coeff * lastOutput;      // delayed output
  lastOutput += lastIn + (coeff * temp); // input + delayed Input
  lastIn = temp;
  return lastOutput;                     // save output and return
}
