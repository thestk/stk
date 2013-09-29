/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using a first-order
    allpass filter.  A fixed maximum length
    of 4095 and a delay of 0.5 is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    An allpass filter has unity magnitude gain but
    variable phase delay properties, making it useful
    in achieving fractional delays without affecting
    a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay
    response, the minimum delay possible in this
    implementation is limited to a value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "DelayA.h"
#include <iostream.h>

DelayA :: DelayA()
{
  this->setDelay( 0.5 );
  apInput = 0.0;
  doNextOut = true;
}

DelayA :: DelayA(MY_FLOAT theDelay, long maxDelay)
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

DelayA :: ~DelayA()
{
}

void DelayA :: clear()
{
  Delay::clear();
  apInput = 0.0;
}

void DelayA :: setDelay(MY_FLOAT theDelay)  
{
  MY_FLOAT outPointer;

  if (theDelay > length-1) {
    cerr << "DelayA: setDelay(" << theDelay << ") too big!" << endl;
    // Force delay to maxLength
    outPointer = inPoint + 1.0;
    delay = length - 1;
  }
  else if (theDelay < 0.5) {
    cerr << "DelayA: setDelay(" << theDelay << ") less than 0.5 not possible!" << endl;
    outPointer = inPoint + 0.4999999999;
    delay = 0.5;
  }
  else {
    outPointer = inPoint - theDelay + 1.0;     // outPoint chases inpoint
    delay = theDelay;
  }

  if (outPointer < 0)
    outPointer += length;  // modulo maximum length

  outPoint = (long) outPointer;        // integer part
  alpha = 1.0 + outPoint - outPointer; // fractional part

  if (alpha < 0.5) {
    // The optimal range for alpha is about 0.5 - 1.5 in order to
    // achieve the flattest phase delay response.
    outPoint += 1;
    if (outPoint >= length) outPoint -= length;
    alpha += (MY_FLOAT) 1.0;
  }

  coeff = ((MY_FLOAT) 1.0 - alpha) / 
    ((MY_FLOAT) 1.0 + alpha);         // coefficient for all pass
}

MY_FLOAT DelayA :: nextOut(void)
{
  if ( doNextOut ) {
    // Do allpass interpolation delay.
    nextOutput = -coeff * outputs[0];
    nextOutput += apInput + (coeff * inputs[outPoint]);
    doNextOut = false;
  }

  return nextOutput;
}

MY_FLOAT DelayA :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Increment input pointer modulo length.
  if (inPoint == length)
    inPoint -= length;

  outputs[0] = nextOut();
  doNextOut = true;

  // Save the allpass input and increment modulo length.
  apInput = inputs[outPoint++];
  if (outPoint == length)
    outPoint -= length;

  return outputs[0];
}
