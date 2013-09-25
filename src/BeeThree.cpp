/***************************************************/
/*! \class BeeThree
    \brief STK Hammond-oid organ FM synthesis instrument.

    This class implements a simple 4 operator
    topology, also referred to as algorithm 8 of
    the TX81Z.

    \code
    Algorithm 8 is :
                     1 --.
                     2 -\|
                         +-> Out
                     3 -/|
                     4 --
    \endcode

    Control Change Numbers: 
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
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

#include "BeeThree.h"
#include <string.h>

BeeThree :: BeeThree()
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

  this->setRatio(0, 0.999);
  this->setRatio(1, 1.997);
  this->setRatio(2, 3.006);
  this->setRatio(3, 6.009);

  gains[0] = __FM_gains[95];
  gains[1] = __FM_gains[95];
  gains[2] = __FM_gains[99];
  gains[3] = __FM_gains[95];

  adsr[0]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[2]->setAllTimes( 0.005, 0.003, 1.0, 0.01);
  adsr[3]->setAllTimes( 0.005, 0.001, 0.4, 0.03);

  twozero->setGain( 0.1 );
}  

BeeThree :: ~BeeThree()
{
}

void BeeThree :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[95];
  gains[1] = amplitude * __FM_gains[95];
  gains[2] = amplitude * __FM_gains[99];
  gains[3] = amplitude * __FM_gains[95];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "BeeThree: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT BeeThree :: tick()
{
  register MY_FLOAT temp;

  if (modDepth > 0.0)	{
    temp = 1.0 + (modDepth * vibrato->tick() * 0.1);
    waves[0]->setFrequency(baseFrequency * temp * ratios[0]);
    waves[1]->setFrequency(baseFrequency * temp * ratios[1]);
    waves[2]->setFrequency(baseFrequency * temp * ratios[2]);
    waves[3]->setFrequency(baseFrequency * temp * ratios[3]);
  }

  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = control1 * 2.0 * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);

  temp += control2 * 2.0 * gains[2] * adsr[2]->tick() * waves[2]->tick();
  temp += gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp += gains[0] * adsr[0]->tick() * waves[0]->tick();

  lastOutput = temp * 0.125;
  return lastOutput;
}
