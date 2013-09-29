/***************************************************/
/*! \class Modulate
    \brief STK periodic/random modulator.

    This class combines random and periodic
    modulations to give a nice, natural human
    modulation function.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Modulate.h"

Modulate :: Modulate()
{
  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE );
  vibrato->setFrequency( 6.0 );
  vibratoGain = 0.04;

  noise = new SubNoise(330);
  randomGain = 0.05;

  filter = new OnePole( 0.999 );
  filter->setGain( randomGain );
}

Modulate :: ~Modulate()
{
  delete vibrato;
  delete noise;
  delete filter;
}

void Modulate :: reset()
{
  lastOutput = (MY_FLOAT)  0.0;
}

void Modulate :: setVibratoRate(MY_FLOAT aRate)
{
  vibrato->setFrequency( aRate );
}

void Modulate :: setVibratoGain(MY_FLOAT aGain)
{
  vibratoGain = aGain;
}

void Modulate :: setRandomGain(MY_FLOAT aGain)
{
  randomGain = aGain;
  filter->setGain( randomGain );
}

MY_FLOAT Modulate :: tick()
{
  // Compute periodic and random modulations.
  lastOutput = vibratoGain * vibrato->tick();
  lastOutput += filter->tick( noise->tick() );
  return lastOutput;                        
}

MY_FLOAT *Modulate :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

MY_FLOAT Modulate :: lastOut() const
{
  return lastOutput;
}
