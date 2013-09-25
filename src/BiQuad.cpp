/*******************************************/
/*
   BiQuad (2-pole, 2-zero) Filter Class,
   by Perry R. Cook, 1995-96.
   Modified by Julius Smith, 2000:
     setA1,setA2,setB1,setB2

   See books on filters to understand
   more about how this works.  Nothing
   out of the ordinary in this version.
*/
/*******************************************/

#include "BiQuad.h"

BiQuad :: BiQuad() : Filter()
{
  inputs = (MY_FLOAT *) malloc(2 * sizeof(MY_FLOAT));
  zeroCoeffs[0] = (MY_FLOAT) 0.0;
  zeroCoeffs[1] = (MY_FLOAT) 0.0;
  poleCoeffs[0] = (MY_FLOAT) 0.0;
  poleCoeffs[1] = (MY_FLOAT) 0.0;
  gain = (MY_FLOAT) 1.0;
  this->clear();
}

BiQuad :: ~BiQuad()
{
  free(inputs);
}

void BiQuad :: clear()
{
  inputs[0] = (MY_FLOAT) 0.0;
  inputs[1] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0.0;
}

void BiQuad :: setA1(MY_FLOAT a1)
{
  poleCoeffs[0] = -a1;
}  

void BiQuad :: setA2(MY_FLOAT a2)
{
  poleCoeffs[1] = -a2;
}  

void BiQuad :: setB1(MY_FLOAT b1)
{
  zeroCoeffs[0] = b1;
}  

void BiQuad :: setB2(MY_FLOAT b2)
{
  zeroCoeffs[1] = b2;
}  

void BiQuad :: setPoleCoeffs(MY_FLOAT *coeffs)
{
  poleCoeffs[0] = coeffs[0];
  poleCoeffs[1] = coeffs[1];
}

void BiQuad :: setZeroCoeffs(MY_FLOAT *coeffs)
{
  zeroCoeffs[0] = coeffs[0];
  zeroCoeffs[1] = coeffs[1];
}

void BiQuad :: setFreqAndReson(MY_FLOAT freq, MY_FLOAT reson)
{
  poleCoeffs[1] = - (reson * reson);
  poleCoeffs[0] = (MY_FLOAT) 2.0 * reson * (MY_FLOAT)  cos(TWO_PI * freq / SRATE);
}

void BiQuad :: setEqualGainZeroes()
{
  zeroCoeffs[1] = (MY_FLOAT) -1.0;
  zeroCoeffs[0] = (MY_FLOAT) 0.0;
}

void BiQuad :: setGain(MY_FLOAT aValue)
{
  gain = aValue;
}

/* Perform Filter Operation */
MY_FLOAT BiQuad :: tick(MY_FLOAT sample)
{
  /*
     Biquad is two pole, two zero filter. Look it up
     in your favorite DSP text.  This version implements
     only 2 state variables.  It takes 5 multiplies, 
     4 adds, and 3 moves.
  */
  MY_FLOAT temp;

  temp = sample * gain;
  temp += inputs[0] * poleCoeffs[0];
  temp += inputs[1] * poleCoeffs[1];
    
  lastOutput = temp;
  lastOutput += (inputs[0] * zeroCoeffs[0]);
  lastOutput += (inputs[1] * zeroCoeffs[1]);
  inputs[1] = inputs[0];
  inputs[0] = temp;

  return lastOutput;

}

