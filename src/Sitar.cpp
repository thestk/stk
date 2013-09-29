/***************************************************/
/*! \class Sitar
    \brief STK sitar string model class.

    This class implements a sitar plucked string
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

#include "Sitar.h"
#include <math.h>

Sitar :: Sitar(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DelayA( (MY_FLOAT)(length / 2.0), length );
  delay = length / 2.0;
  targetDelay = delay;

  loopFilter = new OneZero;
  loopFilter->setZero(0.01);

  envelope = new ADSR();
  envelope->setAllTimes(0.001, 0.04, 0.0, 0.5);

  noise = new Noise;
  this->clear();
}

Sitar :: ~Sitar()
{
  delete delayLine;
  delete loopFilter;
  delete noise;
  delete envelope;
}

void Sitar :: clear()
{
  delayLine->clear();
  loopFilter->clear();
}

void Sitar :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Sitar: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  targetDelay = (Stk::sampleRate() / freakency);
  delay = targetDelay * (1.0 + (0.05 * noise->tick()));
  delayLine->setDelay(delay);
  loopGain = 0.995 + (freakency * 0.0000005);
  if (loopGain > 0.9995) loopGain = 0.9995;
}

void Sitar :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void Sitar :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  pluck(amplitude);
  amGain = 0.1 * amplitude;

#if defined(_STK_DEBUG_)
  cerr << "Sitar: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Sitar :: noteOff(MY_FLOAT amplitude)
{
  loopGain = (MY_FLOAT) 1.0 - amplitude;
  if ( loopGain < 0.0 ) {
    cerr << "Plucked: noteOff amplitude greater than 1.0!" << endl;
    loopGain = 0.0;
  }
  else if ( loopGain > 1.0 ) {
    cerr << "Plucked: noteOff amplitude less than or zero!" << endl;
    loopGain = (MY_FLOAT) 0.99999;
  }

#if defined(_STK_DEBUG_)
  cerr << "Plucked: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Sitar :: tick()
{
  if ( fabs(targetDelay - delay) > 0.001 ) {
    if (targetDelay < delay)
      delay *= 0.99999;
    else
      delay *= 1.00001;
    delayLine->setDelay(delay);
  }

  lastOutput = delayLine->tick( loopFilter->tick( delayLine->lastOut() * loopGain ) + 
                                (amGain * envelope->tick() * noise->tick()));
  
  return lastOutput;
}
