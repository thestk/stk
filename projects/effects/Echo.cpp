/******************************************/  
/*  Echo Effect                           */
/*  by Perry Cook, 1996                   */
/******************************************/

#include "Echo.h"

Echo :: Echo(MY_FLOAT longestDelay)
{
  length = (long) longestDelay + 2;
  delayLine = new DLineN(length);
  effectMix = 0.5;
  this->clear();
  this->setDelay(longestDelay);
}

Echo :: ~Echo()
{
  delete delayLine;
}

void Echo :: clear()
{
  delayLine->clear();
  lastOut = 0.0;
}

void Echo :: setDelay(MY_FLOAT delay)
{
  delayLine->setDelay(delay);
}

void Echo :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT Echo :: tick(MY_FLOAT input)
{
  lastOut = effectMix * delayLine->tick(input);
  lastOut += input * (1.0 - effectMix);
  return lastOut;
}

