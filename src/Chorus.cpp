/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Chorus.h"
#include <iostream>

Chorus :: Chorus(MY_FLOAT baseDelay)
{
  delayLine[0] = new DelayL((long) baseDelay, (long) (baseDelay * 1.414) + 2);
  delayLine[1] = new DelayL((long) (baseDelay), (long) baseDelay + 2);
  baseLength = baseDelay;

  // Concatenate the STK rawwave path to the rawwave file
  mods[0] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE );
  mods[1] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE );
  mods[0]->setFrequency(0.2);
  mods[1]->setFrequency(0.222222);
  modDepth = 0.05;
  effectMix = 0.5;
  this->clear();
}

Chorus :: ~Chorus()
{
  delete delayLine[0];
  delete delayLine[1];
  delete mods[0];
  delete mods[1];
}

void Chorus :: clear()
{
  delayLine[0]->clear();
  delayLine[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

void Chorus :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
  if ( mix < 0.0 ) {
    std::cerr << "Chorus: setEffectMix parameter is less than zero!" << std::endl;
    effectMix = 0.0;
  }
  else if ( mix > 1.0 ) {
    std::cerr << "Chorus: setEffectMix parameter is greater than 1.0!" << std::endl;
    effectMix = 1.0;
  }
}

void Chorus :: setModDepth(MY_FLOAT depth)
{
  modDepth = depth;
}

void Chorus :: setModFrequency(MY_FLOAT frequency)
{
  mods[0]->setFrequency(frequency);
  mods[1]->setFrequency(frequency * 1.1111);
}

MY_FLOAT Chorus :: lastOut() const
{
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
}

MY_FLOAT Chorus :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Chorus :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT Chorus :: tick(MY_FLOAT input)
{
  delayLine[0]->setDelay(baseLength * 0.707 * (1.0 + mods[0]->tick()));
  delayLine[1]->setDelay(baseLength  * 0.5 *  (1.0 - mods[1]->tick()));
  lastOutput[0] = input * (1.0 - effectMix);
  lastOutput[0] += effectMix * delayLine[0]->tick(input);
  lastOutput[1] = input * (1.0 - effectMix);
  lastOutput[1] += effectMix * delayLine[1]->tick(input);
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;
}

MY_FLOAT *Chorus :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
