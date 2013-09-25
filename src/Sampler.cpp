/***************************************************/
/*! \class Sampler
    \brief STK sampling synthesis abstract base class.

    This instrument contains up to 5 attack waves,
    5 looped waves, and an ADSR envelope.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Sampler.h"

Sampler :: Sampler()
{
  // We don't make the waves here yet, because
  // we don't know what they will be.
  adsr = new ADSR;
  baseFrequency = 440.0;
  filter = new OnePole;
  attackGain = 0.25;
  loopGain = 0.25;
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

#if defined(_STK_DEBUG_)
  cerr << "Sampler: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Sampler :: tick()
{
  lastOutput = attackGain * attacks[whichOne]->tick();
  lastOutput += loopGain * loops[whichOne]->tick();
  lastOutput = filter->tick(lastOutput);
  lastOutput *= adsr->tick();
  return lastOutput;
}
