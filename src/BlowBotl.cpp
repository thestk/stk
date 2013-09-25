/***************************************************/
/*! \class BlowBotl
    \brief STK blown bottle instrument class.

    This class implements a helmholtz resonator
    (biquad filter) with a polynomial jet
    excitation (a la Cook).

    Control Change Numbers: 
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "BlowBotl.h"
#include "SKINI.msg"
#include <string.h>

#define __BOTTLE_RADIUS_ 0.999

BlowBotl :: BlowBotl()
{
  jetTable = new JetTabl();

  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"rawwaves/sinewave.raw"), TRUE );
  vibrato->setFrequency( 5.925 );
  vibratoGain = 0.0;

  resonator = new BiQuad();
  resonator->setResonance(500.0, __BOTTLE_RADIUS_, true);

  adsr = new ADSR();
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010);

  noise = new Noise();
  noiseGain = 20.0;

	maxPressure = (MY_FLOAT) 0.0;
}

BlowBotl :: ~BlowBotl()
{
  delete jetTable;
  delete resonator;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void BlowBotl :: clear()
{
  resonator->clear();
}

void BlowBotl :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "BlowBotl: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  resonator->setResonance( freakency, __BOTTLE_RADIUS_, true );
}

void BlowBotl :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void BlowBotl :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void BlowBotl :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  cerr << "BlowBotl: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void BlowBotl :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  cerr << "BlowBotl: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT BlowBotl :: tick()
{
  MY_FLOAT breathPressure;
  MY_FLOAT randPressure;
  MY_FLOAT pressureDiff;

  // Calculate the breath pressure (envelope + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  pressureDiff = breathPressure - resonator->lastOut();

  randPressure = noiseGain * noise->tick();
  randPressure *= breathPressure;
  randPressure *= (1.0 + pressureDiff);

  resonator->tick( breathPressure + randPressure - ( jetTable->tick( pressureDiff ) * pressureDiff ) );
  lastOutput = 0.2 * outputGain * dcBlock->tick( pressureDiff );

  return lastOutput;
}

void BlowBotl :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "BlowBotl: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "BlowBotl: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_NoiseLevel_) // 4
    noiseGain = norm * 30.0;
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = norm * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    cerr << "BlowBotl: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "BlowBotl: controlChange number = " << number << ", value = " << value << endl;
#endif
}
