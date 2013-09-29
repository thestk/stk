/***************************************************/
/*! \class Fluate
    \brief STK flute physical model class.

    This class implements a simple flute
    physical model, as discussed by Karjalainen,
    Smith, Waryznyk, etc.  The jet model uses
    a polynomial, a la Cook.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Jet Delay = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Flute.h"
#include "SKINI.msg"
#include <string.h>

Flute :: Flute(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  boreDelay = new DelayL( 100.0, length );
  length >>= 1;
  jetDelay = new DelayL( 49.0, length );
  jetTable = new JetTabl();
  filter = new OnePole();
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();
  noise = new Noise();
  adsr = new ADSR();

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"sinewave.raw"), TRUE );
  vibrato->setFrequency( 5.925 );

  this->clear();

  filter->setPole( 0.7 - ((MY_FLOAT) 0.1 * 22050.0 / Stk::sampleRate() ) );
  filter->setGain( -1.0 );
  adsr->setAllTimes( 0.005, 0.01, 0.8, 0.010);
  endReflection = (MY_FLOAT) 0.5;
  jetReflection = (MY_FLOAT) 0.5;
  noiseGain =  0.15;             // Breath pressure random component.
  vibratoGain = (MY_FLOAT) 0.05; // Breath periodic vibrato component.
  jetRatio = (MY_FLOAT) 0.32;

	maxPressure = (MY_FLOAT) 0.0;
  lastFrequency = 220.0;
}

Flute :: ~Flute()
{
  delete jetDelay;
  delete boreDelay;
  delete jetTable;
  delete filter;
  delete dcBlock;
  delete noise;
  delete adsr;
  delete vibrato;
}

void Flute :: clear()
{
  jetDelay->clear();
  boreDelay->clear();
  filter->clear();
  dcBlock->clear();
}

void Flute :: setFrequency(MY_FLOAT frequency)
{
  lastFrequency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Flute: setFrequency parameter is less than or equal to zero!" << endl;
    lastFrequency = 220.0;
  }

  // We're overblowing here.
  lastFrequency *= 0.66666;
  // Delay = length - approximate filter delay.
  MY_FLOAT delay = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  boreDelay->setDelay(delay);
  jetDelay->setDelay(delay * jetRatio);
}

void Flute :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude / (MY_FLOAT) 0.8;
  adsr->keyOn();
}

void Flute :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Flute :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing( 1.1 + (amplitude * 0.20), amplitude * 0.02);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  cerr << "Flute: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Flute :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.02);

#if defined(_STK_DEBUG_)
  cerr << "Flute: NoteOff amplitude = " << amplitude << endl;
#endif
}

void Flute :: setJetReflection(MY_FLOAT coefficient)
{
  jetReflection = coefficient;
}

void Flute :: setEndReflection(MY_FLOAT coefficient)
{         
  endReflection = coefficient;
}               

void Flute :: setJetDelay(MY_FLOAT aRatio)
{
  // Delay = length - approximate filter delay.
  MY_FLOAT temp = Stk::sampleRate() / lastFrequency - (MY_FLOAT) 2.0;
  jetRatio = aRatio;
  jetDelay->setDelay(temp * aRatio); // Scaled by ratio.
}

MY_FLOAT Flute :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = maxPressure * adsr->tick();
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  MY_FLOAT temp = filter->tick( boreDelay->lastOut() );
  temp = dcBlock->tick(temp); // Block DC on reflection.

  pressureDiff = breathPressure - (jetReflection * temp);
  pressureDiff = jetDelay->tick( pressureDiff );
  pressureDiff = jetTable->tick( pressureDiff ) + (endReflection * temp);
  lastOutput = (MY_FLOAT) 0.3 * boreDelay->tick( pressureDiff );

  lastOutput *= outputGain;
  return lastOutput;
}

void Flute :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Flute: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Flute: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_JetDelay_) // 2
    this->setJetDelay( (MY_FLOAT) (0.08 + (0.48 * norm)) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0);
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    cerr << "Flute: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Flute: controlChange number = " << number << ", value = " << value << endl;
#endif
}
