/*******************************************/
/*  1st order allpass filter.              */ 
/*  by Perry R. Cook, 1995-96              */ 
/*  A special case of the one pole         */
/*  one zero filter.                       */
/*******************************************/

#include "AllPass1.h"

AllPass1 :: AllPass1()
{
    inputs = (MY_FLOAT *) malloc(MY_FLOAT_SIZE);
    outputs = (MY_FLOAT *) malloc(MY_FLOAT_SIZE);
    this->clear();

}

AllPass1 :: ~AllPass1()
{
    free(inputs);
    free(outputs);
}

void AllPass1 :: clear()
{
    outputs[0] = 0.0;
    inputs[0] = 0.0;
    lastOutput = 0.0;
}

MY_FLOAT AllPass1 :: tick(MY_FLOAT sample)    
{
    outputs[0] = sample - inputs[0] + (0.99 * outputs[0]);
    inputs[0] = sample;
    lastOutput = outputs[0];
    return lastOutput;
}


