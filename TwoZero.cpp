/*******************************************/
/*  Two Zero Filter Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  See books on filters to understand     */
/*  more about how this works.  Nothing    */
/*  out of the ordinary in this version.   */
/*******************************************/

#include "TwoZero.h"

TwoZero :: TwoZero() : Filter()
{
    inputs = (MY_FLOAT *) malloc(2 * MY_FLOAT_SIZE);
    zeroCoeffs[0] = (MY_FLOAT) 0.0;
    zeroCoeffs[1] = (MY_FLOAT) 0.0;
    gain = (MY_FLOAT) 1.0;
    this->clear();
}

TwoZero :: ~TwoZero()
{
    free(inputs);
}

void TwoZero :: clear()
{
    inputs[0] = (MY_FLOAT) 0.0;
    inputs[1] = (MY_FLOAT) 0.0;
    lastOutput = (MY_FLOAT) 0.0;
}

void TwoZero :: setZeroCoeffs(MY_FLOAT *coeffs)
{
    zeroCoeffs[0] = coeffs[0];
    zeroCoeffs[1] = coeffs[1];
}

void TwoZero :: setGain(MY_FLOAT aValue)
{
    gain = aValue;
}

MY_FLOAT TwoZero :: tick(MY_FLOAT sample) /*   Perform Filter Operation            */
{                                         /*  TwoZero is a two zero filter (duh!)  */
                                          /*  Look it up in your favorite DSP text */
    lastOutput = zeroCoeffs[0] * inputs[0];
    lastOutput += zeroCoeffs[1] * inputs[1];
    inputs[1] = inputs[0];
    inputs[0] = gain * sample;
    lastOutput += inputs[0];
    return lastOutput;
}

