/***************************************************/
/*! \class PluckTwo
    \brief STK enhanced plucked string model class.

    This class implements an enhanced two-string,
    plucked physical model, a la Jaffe-Smith,
    Smith, and others.

    PluckTwo is an abstract class, with no excitation
    specified.  Therefore, it can't be directly
    instantiated.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "PluckTwo.h"

PluckTwo :: PluckTwo(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  baseLoopGain = (MY_FLOAT) 0.995;
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA((MY_FLOAT)(length / 2.0), length);
  delayLine2 = new DelayA((MY_FLOAT)(length / 2.0), length);
  combDelay = new DelayL((MY_FLOAT)(length / 2.0), length);
  filter = new OneZero;
  filter2 = new OneZero;
  pluckAmplitude = (MY_FLOAT) 0.3;
  pluckPosition = (MY_FLOAT) 0.4;
  detuning = (MY_FLOAT) 0.995;
  lastFrequency = lowestFrequency * (MY_FLOAT) 2.0;
  lastLength = length * (MY_FLOAT) 0.5;
}

PluckTwo :: ~PluckTwo()
{
  delete delayLine;
  delete delayLine2;
  delete combDelay;
  delete filter;
  delete filter2;
}

void PluckTwo :: clear()
{
  delayLine->clear();
  delayLine2->clear();
  combDelay->clear();
  filter->clear();
  filter2->clear();
}

void PluckTwo :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( lastFrequency <= 0.0 ) {
    cerr << "PluckTwo: setFrequency parameter less than or equal to zero!" << endl;
    lastFrequency = 220.0;
  }

  // Delay = length - approximate filter delay.
  lastLength = ( Stk::sampleRate() / lastFrequency);
  MY_FLOAT delay = (lastLength / detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine->setDelay( delay );

  delay = (lastLength * detuning) - (MY_FLOAT) 0.5;
  if ( delay <= 0.0 ) delay = 0.3;
  else if ( delay > length ) delay = length;
  delayLine2->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 1.0 ) loopGain = (MY_FLOAT) 0.99999;
}

void PluckTwo :: setDetune(MY_FLOAT detune)
{
  detuning = detune;
  if ( detuning <= 0.0 ) {
    cerr << "PluckTwo: setDetune parameter less than or equal to zero!" << endl;
    detuning = 0.1;
  }
  delayLine->setDelay(( lastLength / detuning) - (MY_FLOAT) 0.5);
  delayLine2->setDelay( (lastLength * detuning) - (MY_FLOAT) 0.5);
}

void PluckTwo :: setFreqAndDetune(MY_FLOAT frequency, MY_FLOAT detune)
{
  detuning = detune;
  this->setFrequency(frequency);
}

void PluckTwo :: setPluckPosition(MY_FLOAT position)
{
  pluckPosition = position;
  if ( position < 0.0 ) {
    cerr << "PluckTwo: setPluckPosition parameter is less than zero!" << endl;
    pluckPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    cerr << "PluckTwo: setPluckPosition parameter is greater than 1.0!" << endl;
    pluckPosition = 1.0;
  }
}

void PluckTwo :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * (MY_FLOAT) 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void PluckTwo :: noteOff(MY_FLOAT amplitude)
{
  loopGain =  ((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.5;

#if defined(_STK_DEBUG_)
  cerr << "PluckTwo: NoteOff amplitude = " << amplitude << endl;
#endif
}

