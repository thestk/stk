/***************************************************/
/*! \class Simple
    \brief STK wavetable/noise instrument.

    This class combines a looped wave, a
    noise source, a biquad resonance filter,
    a one-pole filter, and an ADSR envelope
    to create some interesting sounds.

    Control Change Numbers: 
       - Filter Pole Position = 2
       - Noise/Pitched Cross-Fade = 4
       - Envelope Rate = 11
       - Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Simple.h"
#include "SKINI.msg"
#include <string.h>

Simple :: Simple()
{
  adsr = new ADSR;
  baseFrequency = (MY_FLOAT) 440.0;

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  loop = new WaveLoop( strcat(file,"impuls10.raw"), TRUE );

  filter = new OnePole(0.5);
  noise = new Noise;
  biquad = new BiQuad();

  setFrequency(baseFrequency);
  loopGain = 0.5;
}  

Simple :: ~Simple()
{
  delete adsr;
  delete loop;
  delete filter;
  delete biquad;
}

void Simple :: keyOn()
{
  adsr->keyOn();
}

void Simple :: keyOff()
{
  adsr->keyOff();
}

void Simple :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  keyOn();
  setFrequency(frequency);
  filter->setGain(amplitude); 

#if defined(_STK_DEBUG_)
  cerr << "Simple: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}
void Simple :: noteOff(MY_FLOAT amplitude)
{
  keyOff();

#if defined(_STK_DEBUG_)
  cerr << "Simple: NoteOff amplitude = " << amplitude << endl;
#endif
}

void Simple :: setFrequency(MY_FLOAT frequency)
{
  biquad->setResonance( frequency, 0.98, true );
  loop->setFrequency(frequency);
}

MY_FLOAT Simple :: tick()
{
  lastOutput = loopGain * loop->tick();
  biquad->tick( noise->tick() );
  lastOutput += (1.0 - loopGain) * biquad->lastOut();
  lastOutput = filter->tick( lastOutput );
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Simple :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Clarinet: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Clarinet: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_Breath_) // 2
    filter->setPole( 0.99 * (1.0 - (norm * 2.0)) );
  else if (number == __SK_NoiseLevel_) // 4
    loopGain = norm;
  else if (number == __SK_ModFrequency_) { // 11
    norm /= 0.2 * Stk::sampleRate();
    adsr->setAttackRate( norm );
    adsr->setDecayRate( norm );
    adsr->setReleaseRate( norm );
  }
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    cerr << "Simple: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Simple: controlChange number = " << number << ", value = " << value << endl;
#endif
}
