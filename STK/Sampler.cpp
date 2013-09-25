/*******************************************/
/*  Master Class for Sampling Synthesizer  */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains up to 5       */
/*  attack waves, 5 looped waves, and      */
/*  an ADSR envelope.                      */
/*******************************************/

#include "Sampler.h"

Sampler :: Sampler()
{
  adsr = new ADSR;
  /*  We don't make the waves here yet, because */
  /*  we don't know what they will be.          */
  baseFreq = (MY_FLOAT) 440.0;
  filter = new OnePole;
  attackGain = (MY_FLOAT) 0.25;
  loopGain = (MY_FLOAT) 0.25;
  whichOne = 0;
}  

Sampler :: ~Sampler()
{
  delete adsr;
  delete filter;
}

void Sampler :: keyOn()
{
  adsr->keyOn();
  attacks[0]->reset();
}

void Sampler :: keyOff()
{
  adsr->keyOff();
}

void Sampler :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();
#if defined(_debug_)        
  printf("Sampler : NoteOff: Amp=%lf\n",amplitude);
#endif    
}

void Sampler :: setFreq(MY_FLOAT frequency)
{
}

MY_FLOAT Sampler :: tick()
{
  lastOutput = attackGain * attacks[whichOne]->tick();
  lastOutput += loopGain * loops[whichOne]->tick();
  lastOutput = filter->tick(lastOutput);
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Sampler :: controlChange(int number, MY_FLOAT value)
{
}
