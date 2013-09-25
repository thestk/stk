/********************************************/
/*  RawWave Abstract Class,                 */
/*  by Gary P. Scavone, 1998                */ 
/********************************************/

#include "RawWave.h"

RawWave :: RawWave()
{
}

RawWave :: ~RawWave()
{
}

void RawWave :: reset()
{
}

void RawWave :: normalize(MY_FLOAT newPeak)
{
}

void RawWave :: setRate(MY_FLOAT aRate)
{
}

int RawWave :: informTick()
{
  return 0;
}

MY_FLOAT RawWave :: tick()
{
  return 0.0;
}

MY_FLOAT RawWave :: lastOut()
{
  return 0.0;
}

