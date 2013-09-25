/******************************************/  
/*  Enhanced (Jaffe-Smith, Smith, others) */
/*  Karplus-Strong plucked model          */
/*  by Perry Cook, 1995-96                */
/*  This is the super-class, with no      */
/*  excitation specified.  So this one by */
/*  itself doesn't make any sound.        */
/******************************************/

#include "Plucked2.h"

Plucked2 :: Plucked2(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  baseLoopGain = (MY_FLOAT) 0.995;
  loopGain = (MY_FLOAT) 0.999;
  delayLine = new DLineA(length);
  delayLine2 = new DLineA(length);
  combDelay = new DLineL(length);
  filter = new OneZero;
  filter2 = new OneZero;
  pluckAmp = (MY_FLOAT) 0.3;
  pluckPos = (MY_FLOAT) 0.4;
  detuning = (MY_FLOAT) 0.995;
  lastFreq = lowestFreq * (MY_FLOAT) 2.0;
  lastLength = length * (MY_FLOAT) 0.5;
}

Plucked2 :: ~Plucked2()
{
  delete delayLine;
  delete delayLine2;
  delete combDelay;
  delete filter;
  delete filter2;
}

void Plucked2 :: clear()
{
  delayLine->clear();
  delayLine2->clear();
  combDelay->clear();
  filter->clear();
  filter2->clear();
}

void Plucked2 :: setFreq(MY_FLOAT frequency)
{
  lastFreq = frequency;
  lastLength = ((MY_FLOAT) SRATE / lastFreq);        /* length - delays */
  delayLine->setDelay((lastLength / detuning) - (MY_FLOAT) 0.5);
  delayLine2->setDelay((lastLength * detuning) - (MY_FLOAT) 0.5);
  loopGain = baseLoopGain + (frequency * (MY_FLOAT) 0.000005);
  if (loopGain>1.0) loopGain = (MY_FLOAT) 0.99999;
}

void Plucked2 :: setDetune(MY_FLOAT detune)
{
  detuning = detune;
  delayLine->setDelay((lastLength / detuning) - (MY_FLOAT) 0.5);
  delayLine2->setDelay((lastLength * detuning) - (MY_FLOAT) 0.5);
}

void Plucked2 :: setFreqAndDetune(MY_FLOAT frequency,MY_FLOAT detune)
{
  lastFreq = frequency;
  detuning = detune;
  this->setFreq(frequency);
}

void Plucked2 :: setPluckPos(MY_FLOAT position)
{
  pluckPos = position;
}

void Plucked2 :: setBaseLoopGain(MY_FLOAT aGain)
{
  baseLoopGain = aGain;
  loopGain = baseLoopGain + (lastFreq * (MY_FLOAT) 0.000005);
  if (loopGain>1.0) loopGain = (MY_FLOAT) 0.99999;
}

void Plucked2 :: noteOff(MY_FLOAT amp)
{
  loopGain =  ((MY_FLOAT) 1.0 - amp) * (MY_FLOAT) 0.5;
#if defined(_debug_)        
  printf("Plucked2 : NoteOff: Amp=%lf\n",amp);
#endif    
}

