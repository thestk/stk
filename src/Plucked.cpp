/***************************************************/
/*! \class Plucked
    \brief STK plucked string model class.

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

#include "Plucked.h"

Plucked :: Plucked(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  loopFilter = new OneZero;
  pickFilter = new OnePole;
  noise = new Noise;
  this->clear();
}

Plucked :: ~Plucked()
{
  delete delayLine;
  delete loopFilter;
  delete pickFilter;
  delete noise;
}

void Plucked :: clear()
{
  delayLine->clear();
  loopFilter->clear();
  pickFilter->clear();
}

void Plucked :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "Plucked: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 0.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.000005);
  if ( loopGain >= 1.0 ) loopGain = (MY_FLOAT) 0.99999;
}

void Plucked :: pluck(MY_FLOAT amplitude)
{
  MY_FLOAT gain = amplitude;
  if ( gain > 1.0 ) {
    std::cerr << "Plucked: pluck amplitude greater than 1.0!" << std::endl;
    gain = 1.0;
  }
  else if ( gain < 0.0 ) {
    std::cerr << "Plucked: pluck amplitude less than zero!" << std::endl;
    gain = 0.0;
  }

  pickFilter->setPole((MY_FLOAT) 0.999 - (gain * (MY_FLOAT) 0.15));
  pickFilter->setGain(gain * (MY_FLOAT) 0.5);
  for (long i=0; i<length; i++)
    // Fill delay with noise additively with current contents.
    delayLine->tick( 0.6 * delayLine->lastOut() + pickFilter->tick( noise->tick() ) );
}

void Plucked :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->pluck(amplitude);

#if defined(_STK_DEBUG_)
  std::cerr << "Plucked: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Plucked :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    std::cerr << "Plucked: noteOff amplitude greater than 1.0!" << std::endl;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    std::cerr << "Plucked: noteOff amplitude less than or zero!" << std::endl;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  std::cerr << "Plucked: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Plucked :: tick()
{
  // Here's the whole inner loop of the instrument!!
  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) ); 
  lastOutput *= (MY_FLOAT) 3.0;
  return lastOutput;
}
