/***************************************************/
/*! \class HevyMetl
    \brief STK heavy metal FM synthesis instrument.

    This class implements 3 cascade operators with
    feedback modulation, also referred to as
    algorithm 3 of the TX81Z.

    Algorithm 3 is :     4--\
                    3-->2-- + -->1-->Out

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

#include "HevyMetl.h"

HevyMetl :: HevyMetl()
  : FM()
{
  // Concatenate the STK rawwave path to the rawwave files
  for ( int i=0; i<3; i++ )
    waves[i] = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE );
  waves[3] = new WaveLoop( (Stk::rawwavePath() + "fwavblnk.raw").c_str(), TRUE );

  this->setRatio(0, 1.0 * 1.000);
  this->setRatio(1, 4.0 * 0.999);
  this->setRatio(2, 3.0 * 1.001);
  this->setRatio(3, 0.5 * 1.002);

  gains[0] = __FM_gains[92];
  gains[1] = __FM_gains[76];
  gains[2] = __FM_gains[91];
  gains[3] = __FM_gains[68];

  adsr[0]->setAllTimes( 0.001, 0.001, 1.0, 0.01);
  adsr[1]->setAllTimes( 0.001, 0.010, 1.0, 0.50);
  adsr[2]->setAllTimes( 0.010, 0.005, 1.0, 0.20);
  adsr[3]->setAllTimes( 0.030, 0.010, 0.2, 0.20);

  twozero->setGain( 2.0 );
  vibrato->setFrequency( 5.5 );
  modDepth = 0.0;
}  

HevyMetl :: ~HevyMetl()
{
}

void HevyMetl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  gains[0] = amplitude * __FM_gains[92];
  gains[1] = amplitude * __FM_gains[76];
  gains[2] = amplitude * __FM_gains[91];
  gains[3] = amplitude * __FM_gains[68];
  this->setFrequency(frequency);
  this->keyOn();

#if defined(_STK_DEBUG_)
  cerr << "HevyMetl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT HevyMetl :: tick()
{
  register MY_FLOAT temp;

  temp = vibrato->tick() * modDepth * 0.2;    
  waves[0]->setFrequency(baseFrequency * (1.0 + temp) * ratios[0]);
  waves[1]->setFrequency(baseFrequency * (1.0 + temp) * ratios[1]);
  waves[2]->setFrequency(baseFrequency * (1.0 + temp) * ratios[2]);
  waves[3]->setFrequency(baseFrequency * (1.0 + temp) * ratios[3]);
    
  temp = gains[2] * adsr[2]->tick() * waves[2]->tick();
  waves[1]->addPhaseOffset(temp);
    
  waves[3]->addPhaseOffset(twozero->lastOut());
  temp = (1.0 - (control2 * 0.5)) * gains[3] * adsr[3]->tick() * waves[3]->tick();
  twozero->tick(temp);
    
  temp += control2 * (MY_FLOAT) 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
  temp = temp * control1;
    
  waves[0]->addPhaseOffset(temp);
  temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
  lastOutput = temp * 0.5;
  return lastOutput;
}
