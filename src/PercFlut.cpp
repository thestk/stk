/***************************************************/
/*! \class PercFlut
    \brief STK percussive flute FM synthesis instrument.

    This class implements algorithm 4 of the TX81Z.

    \code
    Algorithm 4 is :   4->3--\
                          2-- + -->1-->Out
    \endcode

    Control Change Numbers: 
       - Total Modulator Index = 2
       - Modulator Crossfade = 4
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

#include "PercFlut.h"
#include <string.h>

PercFlut :: PercFlut()
  : FM()
{
  int i;
  char files[4][128];

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  for ( i=0; i<4; i++ )
    strcpy( files[i], RAWWAVE_PATH);

  strcat(files[0], "rawwaves/sinewave.raw");
  strcat(files[1], "rawwaves/sinewave.raw");
  strcat(files[2], "rawwaves/sinewave.raw");
  strcat(files[3], "rawwaves/fwavblnk.raw");

  for ( i=0; i<4; i++ )
    waves[i] = new WaveLoop( files[i], TRUE );

  this->setRatio(0, 1.50 * 1.000);
  this->setRatio(1, 3.00 * 0.995);
  this->setRatio(2, 2.99 * 1.005);
  this->setRatio(3, 6.00 * 0.997);
  gains[0] = __FM_gains[99];
  gains[1] = __FM_gains[71];
  gains[2] = __FM_gains[93];
  gains[3] = __FM_gains[85];

  adsr[0]->setAllTimes( 0.05, 0.05, __FM_susLevels[14], 0.05);
  adsr[1]->setAllTimes( 0.02, 0.50, __FM_susLevels[13], 0.5);
  adsr[2]->setAllTimes( 0.02, 0.30, __FM_susLevels[11], 0.05);
  adsr[3]->setAllTimes( 0.02, 0.05, __FM_susLevels[13], 0.01);

  twozero->setGain( 0.0 );
  modDepth = 0.005;
}  

PercFlut :: ~PercFlut()
{
}

void PercFlut :: setFrequency(MY_FLOAT frequency)
{    
  baseFrequency = frequency;
}

void PercFlut :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[99] * 0.5;
  gains[1] = amplitude * __FM_gains[71] * 0.5;
  gains[2] = amplitude * __FM_gains[93] * 0.5;
  gains[3] = amplitude * __FM_gains[85] * 0.5;
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "PercFlut: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT PercFlut :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * (MY_FLOAT) 0.2;    
  waves[0]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * ((MY_FLOAT) 1.0 + temp) * ratios[3]);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = gains[3] * adsr[3]->tick() * waves[3]->tick();

  twozero->tick(temp);
  waves[2]->addPhaseOffset(temp);
  temp = (1.0 - (control2 * 0.5)) * gains[2] * adsr[2]->tick() * waves[2]->tick();

  temp += control2 * 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;

  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * (MY_FLOAT) 0.5;
  return lastOutput;
}
