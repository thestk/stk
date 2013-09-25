/*******************************************/
/*  Linearly Interpolating Delay Line      */
/*  Object by Perry R. Cook 1995-96        */
/*  This one uses a delay line of maximum  */
/*  length specified on creation, and      */ 
/*  linearly interpolates fractional       */
/*  length.  It is designed to be more     */
/*  efficient if the delay length is not   */
/*  changed very often.                    */
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
}

DLineL :: DLineL(long max_length)
{
  length = max_length;
  inputs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  this->clear();
  inPoint = 0;
  outPoint = length >> 1;
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

  if (lag > length-1) {               // if delay is too big,
    printf("DLineL: Delay length too big.\n");
    printf("Setting to maximum length of %ld.\n",length-1);
    outPointer = inPoint + 1;         // force delay to max_length
  }
  else
    outPointer = inPoint - lag;       // read chases write
  while (outPointer<0)
    outPointer += length;             // modulo maximum length
  outPoint = (long) outPointer;       // integer part
  alpha = outPointer - outPoint;      // fractional part
  omAlpha = (MY_FLOAT) 1.0 - alpha;   // 1.0 - fractional part (more efficient)
}

MY_FLOAT DLineL :: tick(MY_FLOAT sample)     // Take one, yield one
{
  inputs[inPoint++] = sample;                // Input next sample
  if (inPoint == length)                     // Check for end condition
    inPoint -= length;
  lastOutput = inputs[outPoint++] * omAlpha; // first 1/2 of interpolation
  if (outPoint<length)        {              // Check for end condition
    lastOutput += inputs[outPoint] * alpha;  // second 1/2 of interpolation
  }                                          
  else        {                              // if at end . . .
    lastOutput += inputs[0] * alpha;         // second 1/2 of interpolation
    outPoint -= length;                             
  }
  return lastOutput;
}
