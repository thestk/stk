/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements a echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Echo.h"
#include <iostream>

Echo :: Echo(MY_FLOAT longestDelay)
{
  length = (long) longestDelay + 2;
  delayLine = new Delay(length>>1, length);
  effectMix = 0.5;
  this->clear();
}

Echo :: ~Echo()
{
  delete delayLine;
}

void Echo :: clear()
{
  delayLine->clear();
  lastOutput = 0.0;
}

void Echo :: setDelay(MY_FLOAT delay)
{
  MY_FLOAT size = delay;
  if ( delay < 0.0 ) {
    std::cerr << "Echo: setDelay parameter is less than zero!" << std::endl;
    size = 0.0;
  }
  else if ( delay > length ) {
    std::cerr << "Echo: setDelay parameter is greater than delay length!" << std::endl;
    size = length;
  }

  delayLine->setDelay((long)size);
}

void Echo :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    std::cerr << "Echo: setEffectMix parameter is less than zero!" << std::endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    std::cerr << "Echo: setEffectMix parameter is greater than 1.0!" << std::endl;
    effectMix = 1.0;
  }
}

MY_FLOAT Echo :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Echo :: tick(MY_FLOAT input)
{
  lastOutput = effectMix * delayLine->tick(input);
  lastOutput += input * (1.0 - effectMix);
  return lastOutput;
}

MY_FLOAT *Echo :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
