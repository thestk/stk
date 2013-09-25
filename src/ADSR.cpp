/***************************************************/
/*! \class ADSR
    \brief STK ADSR envelope class.

    This Envelope subclass implements a
    traditional ADSR (Attack, Decay,
    Sustain, Release) envelope.  It
    responds to simple keyOn and keyOff
    messages, keeping track of its state.
    The \e state = ADSR::DONE after the
    envelope value reaches 0.0 in the
    ADSR::RELEASE state.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "ADSR.h"
#include <stdio.h>

ADSR :: ADSR() : Envelope()
{
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  attackRate = (MY_FLOAT) 0.001;
  decayRate = (MY_FLOAT) 0.001;
  sustainLevel = (MY_FLOAT) 0.5;
  releaseRate = (MY_FLOAT) 0.01;
  state = ATTACK;
}

ADSR :: ~ADSR()
{
}

void ADSR :: keyOn()
{
  target = (MY_FLOAT) 1.0;
  rate = attackRate;
  state = ATTACK;
}

void ADSR :: keyOff()
{
  target = (MY_FLOAT) 0.0;
  rate = releaseRate;
  state = RELEASE;
}

void ADSR :: setAttackRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("ADSR: negative rates not allowed ... correcting!\n");
    attackRate = -aRate;
  }
  else attackRate = aRate;
}

void ADSR :: setDecayRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("ADSR: negative rates not allowed ... correcting!\n");
    decayRate = -aRate;
  }
  else decayRate = aRate;
}

void ADSR :: setSustainLevel(MY_FLOAT aLevel)
{
  if (aLevel < 0.0 ) {
    printf("ADSR: sustain level out of range ... correcting!\n");
    sustainLevel = (MY_FLOAT)  0.0;
  }
  else sustainLevel = aLevel;
}

void ADSR :: setReleaseRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("ADSR: negative rates not allowed ... correcting!\n");
    releaseRate = -aRate;
  }
  else releaseRate = aRate;
}

void ADSR :: setAttackTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("ADSR: negative rates not allowed ... correcting!\n");
    attackRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else attackRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setDecayTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("ADSR: negative times not allowed ... correcting!\n");
    decayRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else decayRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setReleaseTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("ADSR: negative times not allowed ... correcting!\n");
    releaseRate = 1.0 / ( -aTime * Stk::sampleRate() );
  }
  else releaseRate = 1.0 / ( aTime * Stk::sampleRate() );
}

void ADSR :: setAllTimes(MY_FLOAT aTime, MY_FLOAT dTime, MY_FLOAT sLevel, MY_FLOAT rTime)
{
  this->setAttackTime(aTime);
  this->setDecayTime(dTime);
  this->setSustainLevel(sLevel);
  this->setReleaseTime(rTime);
}

void ADSR :: setTarget(MY_FLOAT aTarget)
{
  target = aTarget;
  if (value < target) {
    state = ATTACK;
    this->setSustainLevel(target);
    rate = attackRate;
  }
  if (value > target) {
    this->setSustainLevel(target);
    state = DECAY;
    rate = decayRate;
  }
}

void ADSR :: setValue(MY_FLOAT aValue)
{
  state = SUSTAIN;
  target = aValue;
  value = aValue;
  this->setSustainLevel(aValue);
  rate = (MY_FLOAT)  0.0;
}

int ADSR :: getState(void) const
{
  return state;
}

MY_FLOAT ADSR :: tick()
{
  switch (state) {

  case ATTACK:
    value += rate;
    if (value >= target) {
      value = target;
      rate = decayRate;
      target = sustainLevel;
	    state = DECAY;
    }
    break;

  case DECAY:
    value -= decayRate;
    if (value <= sustainLevel) {
      value = sustainLevel;
      rate = (MY_FLOAT) 0.0;
      state = SUSTAIN;
    }
    break;

  case RELEASE:
    value -= releaseRate;
    if (value <= 0.0)       {
      value = (MY_FLOAT) 0.0;
      state = DONE;
    }
  }

  return value;
}

MY_FLOAT *ADSR :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}
