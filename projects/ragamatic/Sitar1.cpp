 /******************************************/  
/*  Karplus-Strong Sitar1 string model   */
/*  by Perry Cook, 1995-96                */
/*					  */
/*  There exist at least two patents,     */
/*  assigned to Stanford, bearing the     */
/*  names of Karplus and/or Strong.       */
/******************************************/

#include "Sitar1.h"

Sitar1 :: Sitar1(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  loopGain = (MY_FLOAT) 0.999;
  loopFilt = new OneZero();
  loopFilt->setCoeff(0.01);
  delayLine = new DLineA(length);
  delay = length/2;
  delayTarg = delay;
  envelope = new ADSR();
  noise = new Noise;
  envelope->setAllTimes(0.001,0.04,0.0,0.5);
  this->clear();
}

Sitar1 :: ~Sitar1()
{
  delete loopFilt;
  delete delayLine;
  delete envelope;
  delete noise;
}

void Sitar1 :: clear()
{
  loopFilt->clear();
  delayLine->clear();
}

void Sitar1 :: setFreq(MY_FLOAT frequency)
{
  delayTarg = (SRATE / frequency);
  delay = delayTarg * (1.0 + (0.05 * noise->tick()));
  delayLine->setDelay(delay);
  loopGain = (MY_FLOAT) 0.995 + (frequency * (MY_FLOAT)  0.000001);
  if (loopGain>1.0) loopGain = (MY_FLOAT) 0.9995;
}

void Sitar1 :: pluck(MY_FLOAT amplitude)
{
  envelope->keyOn();
}

void Sitar1 :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->pluck(amp);
  amPluck = 0.05 * amp;
#if defined(_debug_)        
  printf("Sitar1 : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Sitar1 :: noteOff(MY_FLOAT amp)
{
  loopGain = (MY_FLOAT) 1.0 - amp;
#if defined(_debug_)        
  printf("Sitar1 : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT Sitar1 :: tick()
{
  MY_FLOAT temp;

  temp = delayLine->lastOut();
  if (fabs(temp) > 1.0) {
    loopGain = 0.1;
    this->noteOff(0.9);
    delay = delayTarg;
    delayLine->setDelay(delay);
  }

  temp *= loopGain;

  if (fabs(delayTarg - delay) > 0.001)	{
    if (delayTarg < delay)
      delay *= 0.99999;
    else
      delay *= 1.00001;
    delayLine->setDelay(delay);
  }

  lastOutput = delayLine->tick(loopFilt->tick(temp)
                               + (amPluck * envelope->tick() * noise->tick()));
  
  return lastOutput;
}

