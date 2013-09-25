 /******************************************/  
/*  Karplus-Strong StrDrone string model   */
/*  by Perry Cook, 1995-96                */
/*					  */
/*  There exist at least two patents,     */
/*  assigned to Stanford, bearing the     */
/*  names of Karplus and/or Strong.       */
/******************************************/

#include "StrDrone.h"

StrDrone :: StrDrone(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  loopGain = (MY_FLOAT) 0.999;
  loopFilt = new OneZero();
  delayLine = new DLineA(length);
  envelope = new ADSR();
  noise = new Noise;
  envelope->setAllTimes(2.0,0.5,0.0,0.5);
  this->clear();
}

StrDrone :: ~StrDrone()
{
  delete loopFilt;
  delete delayLine;
  delete envelope;
  delete noise;
}

void StrDrone :: clear()
{
  loopFilt->clear();
  delayLine->clear();
}

void StrDrone :: setFreq(MY_FLOAT frequency)
{
  MY_FLOAT delay;
  delay = (SRATE / frequency);
  delayLine->setDelay(delay - 0.5);
  loopGain = (MY_FLOAT) 0.997 + (frequency * (MY_FLOAT)  0.000002);
  if (loopGain>1.0) loopGain = (MY_FLOAT) 0.99999;
}

void StrDrone :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void StrDrone :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->pluck(amp);
#if defined(_debug_)        
  printf("StrDrone : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void StrDrone :: noteOff(MY_FLOAT amp)
{
  loopGain = (MY_FLOAT) 1.0 - amp;
#if defined(_debug_)        
  printf("StrDrone : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT StrDrone :: tick()
{
  /* check this out */
  /* here's the whole inner loop of the instrument!!  */
  lastOutput = delayLine->tick(loopFilt->tick((delayLine->lastOut() * loopGain))
		+ (0.005 * envelope->tick() * noise->tick())); 
  return lastOutput;
}

