/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using first-order
    linear interpolation.  A fixed maximum length
    of 4095 and a delay of zero is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    Linear interpolation is an efficient technique
    for achieving fractional delay lengths, though
    it does introduce high-frequency signal
    attenuation to varying degrees depending on the
    fractional delay setting.  The use of higher
    order Lagrange interpolators can typically
    improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "DelayL.h"
#include <iostream.h>

DelayL :: DelayL()
{
  doNextOut = true;
}

DelayL :: DelayL(MY_FLOAT theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  length = maxDelay+1;

  if ( length > 4096 ) {
    // We need to delete the previously allocated inputs.
    delete [] inputs;
    inputs = new MY_FLOAT[length];
    this->clear();
  }

  inPoint = 0;
  this->setDelay(theDelay);
  doNextOut = true;
}

DelayL :: ~DelayL()
{
}

void DelayL :: setDelay(MY_FLOAT theDelay)
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    cerr << "DelayL: setDelay(" << theDelay << ") too big!" << endl;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    cerr << "DelayL: setDelay(" << theDelay << ") less than zero!" << endl;
    outPointer = inPoint;
    delay = 0;
  }
  else {
    outPointer = inPoint - theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPointer < 0)
    outPointer += length; // modulo maximum length

  outPoint = (long) outPointer;  // integer part
  alpha = outPointer - outPoint; // fractional part
  omAlpha = (MY_FLOAT) 1.0 - alpha;
}

MY_FLOAT DelayL :: getDelay(void) const
{
  return delay;
}

MY_FLOAT DelayL :: nextOut(void)
{
  if ( doNextOut ) {
    // First 1/2 of interpolation
    nextOutput = inputs[outPoint] * omAlpha;
    // Second 1/2 of interpolation
    if (outPoint+1 < length)
      nextOutput += inputs[outPoint+1] * alpha;
    else
      nextOutput += inputs[0] * alpha;
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayL :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Increment input pointer modulo length.
  if (inPoint == length)
    inPoint -= length;

  outputs[0] = nextOut();
  doNextOut = true;

  // Increment output pointer modulo length.
  if (++outPoint >= length)
    outPoint -= length;

  return outputs[0];
}
