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
#include <string.h>

Modulate :: Modulate()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"rawwaves/sinewave.raw"), TRUE );
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

MY_FLOAT Modulate ::  tick()
{
  // Compute periodic and random modulations.
  lastOutput = vibratoGain * vibrato->tick();
  lastOutput += filter->tick( noise->tick() );
  return lastOutput;                        
}

MY_FLOAT Modulate :: lastOut() const
{
  return lastOutput;
}
