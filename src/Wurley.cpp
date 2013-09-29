/***************************************************/
/*! \class Wurley
    \brief STK Wurlitzer electric piano FM
           synthesis instrument.

    This class implements two simple FM Pairs
    summed together, also referred to as algorithm
    5 of the TX81Z.

    \code
    Algorithm 5 is :  4->3--\
                             + --> Out
                      2->1--/
    \endcode

    Control Change Numbers: 
       - Modulator Index One = 2
       - Crossfade of Outputs = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

    The basic Chowning/Stanford FM patent expired
    in 1995, but there exist follow-on patents,
    mostly assigned to Yamaha.  If you are of the
    type who should worry about this (making
    money) worry away.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Wurley.h"
#include <string.h>

Wurley :: Wurley()
  : FM()
{
  int i;
  char files[4][128];

  // Concatenate the STK RAWWAVE_PATH to the rawwave file.
  for ( i=0; i<4; i++ )
    strcpy( files[i], RAWWAVE_PATH);

  strcat(files[0], "sinewave.raw");
  strcat(files[1], "sinewave.raw");
  strcat(files[2], "sinewave.raw");
  strcat(files[3], "fwavblnk.raw");

  for ( i=0; i<4; i++ )
    waves[i] = new WaveLoop( files[i], TRUE );

  this->setRatio(0, 1.0);
  this->setRatio(1, 4.0);
  this->setRatio(2, -510.0);
  this->setRatio(3, -510.0);

  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[82];
  gains[2] = __FM_gains[92];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[1]->setAllTimes( 0.001, 1.50, 0.0, 0.04);
  adsr[2]->setAllTimes( 0.001, 0.25, 0.0, 0.04);
  adsr[3]->setAllTimes( 0.001, 0.15, 0.0, 0.04);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 8.0 );
}  

Wurley :: ~Wurley()
{
}

void Wurley :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;
  waves[0]->setFrequency(baseFrequency * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ratios[1]);
  waves[2]->setFrequency(ratios[2]);	// Note here a 'fixed resonance'.
  waves[3]->setFrequency(ratios[3]);
}

void Wurley :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99];
  gains[1] = amplitude * __FM_gains[82];
  gains[2] = amplitude * __FM_gains[82];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "Wurley: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Wurley :: tick()
{
  MY_FLOAT temp, temp2;

  temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  waves[2]->addPhaseOffset(temp);
  temp = ( 1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
  temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();

  // Calculate amplitude modulation and apply it to output.
  temp2 = vibrato->tick() * modDepth;
  temp = temp * (1.0 + temp2);
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
