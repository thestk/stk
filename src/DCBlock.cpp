/*******************************************/
/*  DC Blocking Filter                     */ 
/*  by Perry R. Cook, 1995-96              */ 
/*  This guy is very helpful in, uh,       */
/*  blocking DC.  Needed because a simple  */
/*  low-pass reflection filter allows DC   */
/*  to build up inside recursive           */ 
/*  structures.                            */
/*******************************************/

#include "DCBlock.h"

DCBlock :: DCBlock()
{
  inputs = (MY_FLOAT *) malloc(sizeof(MY_FLOAT));
  outputs = (MY_FLOAT *) malloc(sizeof(MY_FLOAT));
  this->clear();
}

DCBlock :: ~DCBlock()
{
  free(inputs);
  free(outputs);
}

void DCBlock :: clear()
{
  outputs[0] = (MY_FLOAT) 0.0;
  inputs[0] = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT) 0.0;
}

MY_FLOAT DCBlock :: tick(MY_FLOAT sample)    
{
  outputs[0] = sample - inputs[0] + ((MY_FLOAT) 0.99 * outputs[0]);
  inputs[0] = sample;
  lastOutput = outputs[0];
  return lastOutput;
}

