/***************************************************/
/*! \class StifKarp
    \brief STK plucked stiff string instrument.

    This class implements a simple plucked string
    algorithm (Karplus Strong) with enhancements
    (Jaffe-Smith, Smith, and others), including
    string stiffness and pluck position controls.
    The stiffness is modeled with allpass filters.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "StifKarp.h"
#include "SKINI.msg"
#include <string.h>
#include <math.h>

StifKarp :: StifKarp(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA(0.5 * length, length);
  combDelay = new DelayL( 0.2 * length, length);

  filter = new OneZero();
  noise = new Noise();
  biQuad[0] = new BiQuad();
  biQuad[1] = new BiQuad();
  biQuad[2] = new BiQuad();
  biQuad[3] = new BiQuad();

  pluckAmplitude = 0.3;
  pickupPosition = (MY_FLOAT) 0.4;
  lastFrequency = lowestFrequency * 2.0;
  lastLength = length * 0.5;
  stretching = 0.9999;
  baseLoopGain = 0.995;
  loopGain = 0.999;

  clear();
}

StifKarp :: ~StifKarp()
{
  delete delayLine;
  delete combDelay;
  delete filter;
  delete noise;
  delete biQuad[0];
  delete biQuad[1];
  delete biQuad[2];
  delete biQuad[3];
}

void StifKarp :: clear()
{
  delayLine->clear();
  combDelay->clear();
  filter->clear();
}

void StifKarp :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency; 
  if ( frequency <= 0.0 ) {
    cerr << "StifKarp: setFrequency parameter is less than or equal to zero!" << endl;
    lastFrequency = 220.0;
  }

  lastLength = Stk::sampleRate() / lastFrequency;
  MY_FLOAT delay = lastLength - 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay( delay );

  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if (loopGain >= 1.0) loopGain = (MY_FLOAT) 0.99999;

  setStretch(stretching);

  combDelay->setDelay((MY_FLOAT) 0.5 * pickupPosition * lastLength); 
}

void StifKarp :: setStretch(MY_FLOAT stretch)
{
  stretching = stretch;
  MY_FLOAT coefficient;
  MY_FLOAT freq = lastFrequency * 2.0;
  MY_FLOAT dFreq = ( (0.5 * Stk::sampleRate()) - freq ) * 0.25;
  MY_FLOAT temp = 0.5 + (stretch * 0.5);
  if (temp > 0.9999) temp = 0.9999;
  for (int i=0; i<4; i++)	{
    coefficient = temp * temp;
    biQuad[i]->setA2( coefficient );
    biQuad[i]->setB0( coefficient );
    biQuad[i]->setB2( 1.0 );

    coefficient = -2.0 * temp * cos(TWO_PI * freq / Stk::sampleRate());
    biQuad[i]->setA1( coefficient );
    biQuad[i]->setB1( coefficient );

    freq += dFreq;
  }
}

void StifKarp :: setPickupPosition(MY_FLOAT position) {
  pickupPosition = position;
  if ( position < 0.0 ) {
    cerr << "StifKarp: setPickupPosition parameter is less than zero!" << endl;
    pickupPosition = 0.0;
  }
  else if ( position > 1.0 ) {
    cerr << "StifKarp: setPickupPosition parameter is greater than 1.0!" << endl;
    pickupPosition = 1.0;
  }

  // Set the pick position, which puts zeroes at position * length.
  combDelay->setDelay(0.5 * pickupPosition * lastLength); 
}

void StifKarp :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFrequency * 0.000005);
  if ( loopGain > 0.99999 ) loopGain = (MY_FLOAT) 0.99999;
}

void StifKarp :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    cerr << "StifKarp: pluck amplitude greater than 1.0!" << endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    cerr << "StifKarp: pluck amplitude less than zero!" << endl;
    gain = 0.0;
  }

  pluckAmplitude = amplitude;
  for (long i=0; i<length; i++)  {
    // Fill delay with noise additively with current contents.
    delayLine->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude);
    //delayLine->tick( combDelay->tick((delayLine->lastOut() * 0.6) + 0.4 * noise->tick() * pluckAmplitude));
  }
}

void StifKarp :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  cerr << "StifKarp: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void StifKarp :: noteOff(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    cerr << "StifKarp: noteOff amplitude greater than 1.0!" << endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    cerr << "StifKarp: noteOff amplitude less than zero!" << endl;
    gain = 0.0;
  }
  loopGain =  (1.0 - gain) * 0.5;

#if defined(_STK_DEBUG_)
  cerr << "StifPluck: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT StifKarp :: tick()
{
  MY_FLOAT temp = delayLine->lastOut() * loopGain;

  // Calculate allpass stretching.
  for (int i=0; i<4; i++)
    temp = biQuad[i]->tick(temp);

  // Moving average filter.
  temp = filter->tick(temp);

  lastOutput = delayLine->tick(temp);
  lastOutput = lastOutput - combDelay->tick(lastOutput);
  return lastOutput;
}

void StifKarp :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "StifKarp: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "StifKarp: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_PickPosition_) // 4
    setPickupPosition( norm );
  else if (number == __SK_StringDamping_) // 11
    setBaseLoopGain( 0.97 + (norm * 0.03) );
  else if (number == __SK_StringDetune_) // 1
    setStretch( 0.9 + (0.1 * (1.0 - norm)) );
  else
    cerr << "StifKarp: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "StifKarp: controlChange number = " << number << ", value = " << value << endl;
#endif
}

