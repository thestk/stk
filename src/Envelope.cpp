/***************************************************/
/*! \class Envelope
    \brief STK envelope base class.

    This class implements a simple envelope
    generator which is capable of ramping to
    a target value by a specified \e rate.
    It also responds to simple \e keyOn and
    \e keyOff messages, ramping to 1.0 on
    keyOn and to 0.0 on keyOff.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Envelope.h"
#include <stdio.h>

Envelope :: Envelope(void) : Stk()
{    
  target = (MY_FLOAT) 0.0;
  value = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 0.001;
  state = 0;
}

Envelope :: ~Envelope(void)
{    
}

void Envelope :: keyOn(void)
{
  target = (MY_FLOAT) 1.0;
  if (value != target) state = 1;
}

void Envelope :: keyOff(void)
{
  target = (MY_FLOAT) 0.0;
  if (value != target) state = 1;
}

void Envelope :: setRate(MY_FLOAT aRate)
{
  if (aRate < 0.0) {
    printf("Envelope: negative rates not allowed ... correcting!\n");
    rate = -aRate;
  }
  else
    rate = aRate;
}

void Envelope :: setTime(MY_FLOAT aTime)
{
  if (aTime < 0.0) {
    printf("Envelope: negative times not allowed ... correcting!\n");
    rate = 1.0 / (-aTime * Stk::sampleRate());
  }
  else
    rate = 1.0 / (aTime * Stk::sampleRate());
}

void Envelope :: setTarget(MY_FLOAT aTarget)
{
  target = aTarget;
  if (value != target) state = 1;
}

void Envelope :: setValue(MY_FLOAT aValue)
{
  state = 0;
  target = aValue;
  value = aValue;
}

int Envelope :: getState(void) const
{
  return state;
}

MY_FLOAT Envelope :: tick(void)
{
  if (state) {
    if (target > value) {
      value += rate;
      if (value >= target) {
        value = target;
        state = 0;
      }
    }
    else {
      value -= rate;
      if (value <= target) {
        value = target;
        state = 0;
      }
    }
  }
  return value;
}

MY_FLOAT *Envelope :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Envelope :: lastOut(void) const
{
  return value;
}

