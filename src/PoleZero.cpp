/*******************************************/
/*  PoleZero (1-pole, 1-zero) Filter Class */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#include "PoleZero.h"

PoleZero :: PoleZero() : Filter()
{
  inputs = (MY_FLOAT *) malloc(sizeof(MY_FLOAT));
  outputs = (MY_FLOAT *) malloc(sizeof(MY_FLOAT));
  b0Coeff = (MY_FLOAT) 1.0;
  b1Coeff = (MY_FLOAT) 0.0;
  a1Coeff = (MY_FLOAT) 0.0;
  gain = (MY_FLOAT) 1.0;
  this->clear();
}

PoleZero :: ~PoleZero()
{
  free(inputs);
  free(outputs);
}

void PoleZero :: clear()
{
  inputs[0] = (MY_FLOAT) 0.0;
  outputs[0] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0.0;
}

void PoleZero :: setA1(MY_FLOAT coeff)
{
  a1Coeff = coeff;
}

void PoleZero :: setB0(MY_FLOAT coeff)
{
  b0Coeff = coeff;
}

void PoleZero :: setB1(MY_FLOAT coeff)
{
  b1Coeff = coeff;
}

void PoleZero :: setGain(MY_FLOAT aValue)
{
  gain = aValue;
}

// PoleZero is one pole, one zero filter
// Look it up in your favorite DSP text
MY_FLOAT PoleZero :: tick(MY_FLOAT sample)
{
  MY_FLOAT in_sample = gain*sample;

  lastOutput = b0Coeff*in_sample + b1Coeff*inputs[0] - a1Coeff*outputs[0];
  inputs[0] = in_sample;
  outputs[0] = lastOutput;

  return lastOutput;
}

