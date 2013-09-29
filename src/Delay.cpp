/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This protected Filter subclass implements
    a non-interpolating digital delay-line.
    A fixed maximum length of 4095 and a delay
    of zero is set using the default constructor.
    Alternatively, the delay and maximum length
    can be set during instantiation with an
    overloaded constructor.
    
    A non-interpolating delay line is typically
    used in fixed delay-length applications, such
    as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Delay.h"
#include <iostream.h>

Delay :: Delay()
{
  // Default max delay length set to 4095.
  length = 4096;
  delete [] inputs;
  inputs = new MY_FLOAT[length];
  this->clear();

  inPoint = 0;
  outPoint = 0;
  delay = 0;
}

Delay :: Delay(long theDelay, long maxDelay)
{
  // Writing before reading allows delays from 0 to length-1. 
  // If we want to allow a delay of maxDelay, we need a
  // delay-line of length = maxDelay+1.
  length = maxDelay+1;

  // We need to delete the previously allocated inputs.
  delete [] inputs;
  inputs = new MY_FLOAT[length];
  this->clear();

  inPoint = 0;
  this->setDelay(theDelay);
}

Delay :: ~Delay()
{
}

void Delay :: clear(void)
{
  long i;
  for (i=0;i<length;i++) inputs[i] = 0.0;
  outputs[0] = 0.0;
}

void Delay :: setDelay(long theDelay)
{
  if (theDelay > length-1) { // The value is too big.
    cerr << "Delay: setDelay(" << theDelay << ") too big!" << endl;
    // Force delay to maxLength.
    outPoint = inPoint + 1;
    delay = length - 1;
  }
  else if (theDelay < 0 ) {
    cerr << "Delay: setDelay(" << theDelay << ") less than zero!" << endl;
    outPoint = inPoint;
    delay = 0;
  }
  else {
    outPoint = inPoint - (long) theDelay;  // read chases write
    delay = theDelay;
  }

  while (outPoint < 0) outPoint += length;  // modulo maximum length
}

long Delay :: getDelay(void) const
{
  return (long)delay;
}

MY_FLOAT Delay :: energy(void) const
{
  int i;
  register MY_FLOAT e = 0;
  if (inPoint >= outPoint) {
    for (i=outPoint; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  } else {
    for (i=outPoint; i<length; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
    for (i=0; i<inPoint; i++) {
      register MY_FLOAT t = inputs[i];
      e += t*t;
    }
  }
  return e;
}

MY_FLOAT Delay :: contentsAt(long tapDelay) const
{
  long i = tapDelay;
  if (i < 0) {
    cerr << "Delay: contentsAt(" << tapDelay << ") too small!" << endl;
    i = 0;
  }
  else if (i > delay) {
    cerr << "Delay: contentsAt(" << tapDelay << ") too big!" << endl;
    i = (long) delay;
  }

  long tap = inPoint - i;
  if (tap < 0) // Check for wraparound.
    tap += length;

  return inputs[tap];
}

MY_FLOAT Delay :: lastOut(void) const
{
  return Filter::lastOut();
}

MY_FLOAT Delay :: nextOut(void) const
{
  return inputs[outPoint];
}

MY_FLOAT Delay :: tick(MY_FLOAT sample)
{
  inputs[inPoint++] = sample;

  // Check for end condition
  if (inPoint == length)
    inPoint -= length;

  // Read out next value
  outputs[0] = inputs[outPoint++];

  if (outPoint>=length)
    outPoint -= length;

  return outputs[0];
}

MY_FLOAT *Delay :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
