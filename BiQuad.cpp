/*******************************************/
/*  BiQuad (2-pole, 2-zero) Filter Class,  */
/*  by Perry R. Cook, 1995-96              */ 
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#include "BiQuad.h"

BiQuad :: BiQuad() : Filter()
{
  inputs = (MY_FLOAT *) malloc(2 * MY_FLOAT_SIZE);
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

MY_FLOAT BiQuad :: tick(MY_FLOAT sample)    /*  Perform Filter Operation             */
{                                           /*  Biquad is two pole, two zero filter  */
  MY_FLOAT temp;                            /*  Look it up in your favorite DSP text */

  temp = sample * gain;                      /* Here's the math for the  */
  temp += inputs[0] * poleCoeffs[0];         /* version which implements */
  temp += inputs[1] * poleCoeffs[1];         /* only 2 state variables.  */
    
  lastOutput = temp;                          /* This form takes          */
  lastOutput += (inputs[0] * zeroCoeffs[0]);  /* 5 multiplies and         */
  lastOutput += (inputs[1] * zeroCoeffs[1]);  /* 4 adds                   */
  inputs[1] = inputs[0];                      /* and 3 moves              */
  inputs[0] = temp;                           /* like the 2 state-var form*/

  return lastOutput;

}

