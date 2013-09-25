/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "PitShift.h"
#include <iostream.h>
#include <math.h>

PitShift :: PitShift()
{
  delay[0] = 12;
  delay[1] = 512;
  delayLine[0] = new DelayL(delay[0], (long) 1024);
  delayLine[1] = new DelayL(delay[1], (long) 1024);
  effectMix = (MY_FLOAT) 0.5;
  rate = 1.0;
}

PitShift :: ~PitShift()
{
  delete delayLine[0];
  delete delayLine[1];
}

void PitShift :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    cerr << "PitShift: setEffectMix parameter is less than zero!" << endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    cerr << "PitShift: setEffectMix parameter is greater than 1.0!" << endl;
    effectMix = 1.0;
  }
}

void PitShift :: setShift(MY_FLOAT shift)
{
  if (shift < 1.0)    {
    rate = 1.0 - shift; 
  }
  else if (shift > 1.0)       {
    rate = 1.0 - shift;
  }
  else {
    rate = 0.0;
    delay[0] = 512;
  }
}

MY_FLOAT PitShift :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT PitShift :: tick(MY_FLOAT input)
{
  delay[0] = delay[0] + rate;
  while (delay[0] > 1012) delay[0] -= 1000;
  while (delay[0] < 12) delay[0] += 1000;
  delay[1] = delay[0] + 500;
  while (delay[1] > 1012) delay[1] -= 1000;
  while (delay[1] < 12) delay[1] += 1000;
  delayLine[0]->setDelay((long)delay[0]);
  delayLine[1]->setDelay((long)delay[1]);
  env[1] = fabs(delay[0] - 512) * 0.002;
  env[0] = 1.0 - env[1];
  lastOutput =  env[0] * delayLine[0]->tick(input);
  lastOutput += env[1] * delayLine[1]->tick(input);
  lastOutput *= effectMix;
  lastOutput += (1.0 - effectMix) * input;
  return lastOutput;
}

MY_FLOAT *PitShift :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

