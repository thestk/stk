/*******************************************/
/*  One Zero Filter Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  The parameter gain is an additional    */
/*  gain parameter applied to the filter   */  
/*  on top of the normalization that takes */
/*  place automatically.  So the net max   */
/*  gain through the system equals the     */
/*  value of gain.  sgain is the combina-  */
/*  tion of gain and the normalization     */
/*  parameter, so if you set the poleCoeff */
/*  to alpha, sgain is always set to       */
/*  gain / (1.0 - fabs(alpha)).            */
/*******************************************/

#include "OneZero.h"

OneZero :: OneZero()
{
    gain = 1.0;
    zeroCoeff = 1.0;
    sgain = 0.5;
    inputs = (MY_FLOAT *) malloc(MY_FLOAT_SIZE);
    this->clear();
}

OneZero :: ~OneZero()
{
    free(inputs);
}

void OneZero :: clear()
{
    inputs[0] = 0.0;
    lastOutput = 0.0;
}

void OneZero :: setGain(MY_FLOAT aValue)
{
    gain = aValue;
    if (zeroCoeff > 0.0)                  /*  Normalize gain to 1.0 max  */
        sgain = gain / (1.0 + zeroCoeff);
    else
        sgain = gain / (1.0 - zeroCoeff);
}

void OneZero :: setCoeff(MY_FLOAT aValue)
{
    zeroCoeff = aValue;
    if (zeroCoeff > 0.0)                  /*  Normalize gain to 1.0 max  */
        sgain = gain / (1.0 + zeroCoeff);
    else
        sgain = gain / (1.0 - zeroCoeff);
}

MY_FLOAT OneZero :: tick(MY_FLOAT sample) /*   Perform Filter Operation  */
{
    MY_FLOAT temp;
    temp = sgain * sample;
    lastOutput = (inputs[0] * zeroCoeff) + temp;
    inputs[0] = temp;
    return lastOutput;
}

