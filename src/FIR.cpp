/********************************************/  
/*
   General Finite-Impulse-Response (FIR)
   Digital Filter Class
   by Julius Smith, 1997
*/
/********************************************/  

#include "FIR.h"

FIR :: FIR(int theLength) : Object()
{
  length = theLength;
  coeffs = (MY_FLOAT *) malloc(length * sizeof(MY_FLOAT));
  pastInputs = (MY_FLOAT *) calloc(2*length, sizeof(MY_FLOAT));
  piOffset = length;
}

FIR :: ~FIR()
{
  free(pastInputs);
  free(coeffs);
}

void FIR :: clear()
{
  int i;
  for (i=0; i < 2*length; i++) {
    pastInputs[i] = 0;
  }
  piOffset = length;
}

void FIR :: setCoeffs(MY_FLOAT *theCoeffs)
{
  int i;
  for (i=0; i < length; i++) {
    coeffs[i] = theCoeffs[i];
  }
}

MY_FLOAT FIR :: tick(MY_FLOAT input) {
  int i;
  lastOutput = input*coeffs[0];
  for (i=1; i<length; i++)
    lastOutput += coeffs[i] * pastInputs[piOffset-i]; // sample 0 unused
  pastInputs[piOffset++] = input;
  if (piOffset >= 2*length) { // sample 2*length-1 unused
    piOffset = length;
    for (i=0; i<length; i++)
      pastInputs[i] = pastInputs[length+i];
  }
  return lastOutput;
}


MY_FLOAT FIR :: getDelay(MY_FLOAT freq)
{
  return delay;
}

int FIR :: getLength(void)
{
  return length;
}


