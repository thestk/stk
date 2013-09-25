/***************************************************/
/*! \class Drone
    \brief STK "drone" plucked string model.

    This class implements a simple plucked string
    physical model based on the Karplus-Strong
    algorithm.

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.
    There exist at least two patents, assigned to
    Stanford, bearing the names of Karplus and/or
    Strong.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Drone.h"

Drone :: Drone(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  loopFilter = new OneZero;
  noise = new Noise;
  envelope = new ADSR();
  envelope->setAllTimes(2.0, 0.5, 0.0, 0.5);
  this->clear();
}

Drone :: ~Drone()
{
  delete delayLine;
  delete loopFilter;
  delete envelope;
  delete noise;
}

void Drone :: clear()
{
  delayLine->clear();
  loopFilter->clear();
}

void Drone :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Drone: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
  loopGain = 0.997 + (freakency * 0.000002);
  if ( loopGain >= 1.0 ) loopGain = (MY_FLOAT) 0.99999;
}

void Drone :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void Drone :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  cerr << "Drone: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Drone :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    cerr << "Drone: noteOff amplitude greater than 1.0!" << endl;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    cerr << "Drone: noteOff amplitude less than or zero!" << endl;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  cerr << "Drone: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Drone :: tick()
{
  // Here's the whole inner loop of the instrument!!
  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) + (0.005 * envelope->tick() * noise->tick())); 
  return lastOutput;
}
