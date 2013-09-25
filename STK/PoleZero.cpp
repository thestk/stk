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
  zeroCoeff = (MY_FLOAT) 0.0;
  poleCoeff = (MY_FLOAT) 0.0;
  gain = (MY_FLOAT) 1.0;
  this->clear();
}

PoleZero :: ~PoleZero()
{
  free(inputs);
}

void PoleZero :: clear()
{
  inputs[0] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0.0;
}

void PoleZero :: setPoleCoeff(MY_FLOAT coeff)
{
  poleCoeff = coeff;
}

void PoleZero :: setZeroCoeff(MY_FLOAT coeff)
{
  zeroCoeff = coeff;
}

void PoleZero :: setGain(MY_FLOAT aValue)
{
  gain = aValue;
}

// PoleZero is one pole, one zero filter
// Look it up in your favorite DSP text
MY_FLOAT PoleZero :: tick(MY_FLOAT sample)
{
  MY_FLOAT temp;

  // Direct Form II Implementation - only 1 state variable
  temp = sample * gain;
  temp += inputs[0] * poleCoeff;

  lastOutput = temp;
  lastOutput += (inputs[0] * zeroCoeff);
  inputs[0] = temp;

  return lastOutput;
}

