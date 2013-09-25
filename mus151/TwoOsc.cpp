/********************************************/
/*  Two oscillator instrument for use       */
/*  with MUS151 Psychoacoustics course.     */
/*  by Gary P. Scavone & Oded Ben-Tal, 1998 */
/*  CCRMA, Stanford Unviversity             */
/********************************************/

#include "TwoOsc.h"

TwoOsc :: TwoOsc()
{
  int i;

  envelope = new Envelope;  /* Envelope to avoid clicks */

  for (i=0; i<2; i++) {
    osc[i] = new RawLoop("../rawwaves/sinewave.raw");
    osc[i]->normalize();
    osc[i]->setFreq((MY_FLOAT) 200.0);
    amps[i] = 0.2;
  }
  lastOutput = 0;
}

TwoOsc :: ~TwoOsc()
{
  int i;
  for (i=0; i<2; i++) delete osc[i];
  delete envelope;
}

void TwoOsc :: setFreq(int oscnum, MY_FLOAT frequency)
{
  if (oscnum == 1) osc[0]->setFreq(frequency);
  else if (oscnum == 2) osc[1]->setFreq(frequency);
}

void TwoOsc :: setAmp(int oscnum, MY_FLOAT amp)
{
  if (oscnum == 1) amps[0] = amp;
  else if (oscnum == 2) amps[1] = amp;
}

void TwoOsc :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  envelope->keyOn();
}

void TwoOsc :: noteOff(MY_FLOAT amp)
{
  envelope->keyOff();
}

MY_FLOAT TwoOsc :: tick()
{
  lastOutput = 0.5 * envelope->tick() * (amps[0]*osc[0]->tick() + amps[1]*osc[1]->tick());
  return lastOutput;
}

void TwoOsc :: controlChange(int number, MY_FLOAT value)
{
  switch (number) {
  case 20:
    osc[0]->setFreq(value);
    break;
  case 21:
    osc[1]->setFreq(value);
    break;		
  case 22:
    amps[0] = value * NORM_7;
    break;
  case 23:
    amps[1] = value * NORM_7;
    break;
  }	
}
