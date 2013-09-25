/***************************************************/
/*! \class Clarinet
    \brief STK clarinet physical model class.

    This class implements a simple clarinet
    physical model, as discussed by Smith (1986),
    McIntyre, Schumacher, Woodhouse (1983), and
    others.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Clarinet.h"
#include "SKINI.msg"
#include <string.h>

Clarinet :: Clarinet(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayL( (MY_FLOAT)(length / 2.0), length);
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char path[128];
  strcpy(path, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(path,"rawwaves/sinewave.raw"), TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;
}

Clarinet :: ~Clarinet()
{
  delete delayLine;
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Clarinet :: clear()
{
  delayLine->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Clarinet :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Clarinet: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 1.5;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delayLine->setDelay(delay);
}

void Clarinet :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Clarinet :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void Clarinet :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->setFrequency(frequency);
  this->startBlowing((MY_FLOAT) 0.55 + (amplitude * (MY_FLOAT) 0.30), amplitude * (MY_FLOAT) 0.005);
  outputGain = amplitude + (MY_FLOAT) 0.001;

#if defined(_STK_DEBUG_)
  cerr << "Clarinet: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Clarinet :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * (MY_FLOAT) 0.01);

#if defined(_STK_DEBUG_)
  cerr << "Clarinet: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Clarinet :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Perform commuted loss filtering.
  pressureDiff = -0.95 * filter->tick(delayLine->lastOut());

  // Calculate pressure difference of reflected and mouthpiece pressures.
  pressureDiff = pressureDiff - breathPressure;

  // Perform non-linear scattering using pressure difference in reed function.
  lastOutput = delayLine->tick(breathPressure + pressureDiff * reedTable->tick(pressureDiff));

  // Apply output gain.
  lastOutput *= outputGain;

  return lastOutput;
}

void Clarinet :: controlChange(int number, MY_FLOAT value)
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

  if (number == __SK_ReedStiffness_) // 2
    reedTable->setSlope((MY_FLOAT) -0.44 + ( (MY_FLOAT) 0.26 * norm ));
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = (norm * (MY_FLOAT) 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency((norm * (MY_FLOAT) 12.0));
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = (norm * (MY_FLOAT) 0.5);
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue(norm);
  else
    cerr << "Clarinet: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Clarinet: controlChange number = " << number << ", value = " << value << endl;
#endif
}
