/***************************************************/
/*! \class FormSwep
    \brief STK sweepable formant filter class.

    This public BiQuad filter subclass implements
    a formant (resonance) which can be "swept"
    over time from one frequency setting to another.
    It provides methods for controlling the sweep
    rate and target frequency.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "FormSwep.h"

FormSwep :: FormSwep() : BiQuad()
{
  frequency = (MY_FLOAT) 0.0;
  radius = (MY_FLOAT) 0.0;
  targetGain = (MY_FLOAT) 1.0;
  targetFrequency = (MY_FLOAT) 0.0;
  targetRadius = (MY_FLOAT) 0.0;
  deltaGain = (MY_FLOAT) 0.0;
  deltaFrequency = (MY_FLOAT) 0.0;
  deltaRadius = (MY_FLOAT) 0.0;
  sweepState = (MY_FLOAT)  0.0;
  sweepRate = (MY_FLOAT) 0.002;
  dirty = false;
  this->clear();
}

FormSwep :: ~FormSwep()
{
}

void FormSwep :: setResonance(MY_FLOAT aFrequency, MY_FLOAT aRadius)
{
  dirty = false;
  radius = aRadius;
  frequency = aFrequency;

  BiQuad::setResonance( frequency, radius, true );
}

void FormSwep :: setStates(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = false;

  if ( frequency != aFrequency || radius != aRadius )
    BiQuad::setResonance( aFrequency, aRadius, true );

  frequency = aFrequency;
  radius = aRadius;
  gain = aGain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
}

void FormSwep :: setTargets(MY_FLOAT aFrequency, MY_FLOAT aRadius, MY_FLOAT aGain)
{
  dirty = true;
  startFrequency = frequency;
  startRadius = radius;
  startGain = gain;
  targetFrequency = aFrequency;
  targetRadius = aRadius;
  targetGain = aGain;
  deltaFrequency = aFrequency - frequency;
  deltaRadius = aRadius - radius;
  deltaGain = aGain - gain;
  sweepState = (MY_FLOAT) 0.0;
}

void FormSwep :: setSweepRate(MY_FLOAT aRate)
{
  sweepRate = aRate;
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

void FormSwep :: setSweepTime(MY_FLOAT aTime)
{
  sweepRate = 1.0 / ( aTime * Stk::sampleRate() );
  if ( sweepRate > 1.0 ) sweepRate = 1.0;
  if ( sweepRate < 0.0 ) sweepRate = 0.0;
}

MY_FLOAT FormSwep :: tick(MY_FLOAT sample)
{                                     
  if (dirty)  {
    sweepState += sweepRate;
    if ( sweepState >= 1.0 )   {
      sweepState = (MY_FLOAT) 1.0;
      dirty = false;
      radius = targetRadius;
      frequency = targetFrequency;
      gain = targetGain;
    }
    else  {
      radius = startRadius + (deltaRadius * sweepState);
      frequency = startFrequency + (deltaFrequency * sweepState);
      gain = startGain + (deltaGain * sweepState);
    }
    BiQuad::setResonance( frequency, radius, true );
  }

  return BiQuad::tick( sample );
}

MY_FLOAT *FormSwep :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
