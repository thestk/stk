/***************************************************/
/*! \class Brass
    \brief STK simple brass instrument class.

    This class implements a simple brass instrument
    waveguide model, a la Cook (TBone, HosePlayer).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Lip Tension = 2
       - Slide Length = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Brass.h"
#include "SKINI.msg"
#include <string.h>
#include <math.h>

Brass :: Brass(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  delayLine = new DelayA( 0.5 * length, length );

  lipFilter = new BiQuad();
  lipFilter->setGain( 0.03 );
  dcBlock = new PoleZero();
  dcBlock->setBlockZero();

  adsr = new ADSR;
  adsr->setAllTimes( 0.005, 0.001, 1.0, 0.010);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"sinewave.raw"), TRUE );
  vibrato->setFrequency( 6.137 );
  vibratoGain = 0.0;

  this->clear();
	maxPressure = (MY_FLOAT) 0.0;
  lipTarget = 0.0;

  // Necessary to initialize variables.
  setFrequency( 220.0 );
}

Brass :: ~Brass()
{
  delete delayLine;
  delete lipFilter;
  delete dcBlock;
  delete adsr;
  delete vibrato;
}

void Brass :: clear()
{
  delayLine->clear();
  lipFilter->clear();
  dcBlock->clear();
}

void Brass :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Brass: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  // Fudge correction for filter delays.
  slideTarget = (Stk::sampleRate() / freakency * 2.0) + 3.0;
  delayLine->setDelay(slideTarget); // play a harmonic

  lipTarget = freakency;
  lipFilter->setResonance( freakency, 0.997 );
}

void Brass :: setLip(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Brass: setLip parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  lipFilter->setResonance( freakency, 0.997 );
}

void Brass :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setAttackRate(rate);
  maxPressure = amplitude;
  adsr->keyOn();
}

void Brass :: stopBlowing(MY_FLOAT rate)
{
  adsr->setReleaseRate(rate);
  adsr->keyOff();
}

void Brass :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  this->startBlowing(amplitude, amplitude * 0.001);

#if defined(_STK_DEBUG_)
  cerr << "Brass: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Brass :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.005);

#if defined(_STK_DEBUG_)
  cerr << "Brass: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT Brass :: tick()
{
  MY_FLOAT breathPressure = maxPressure * adsr->tick();
  breathPressure += vibratoGain * vibrato->tick();

  MY_FLOAT mouthPressure = 0.3 * breathPressure;
  MY_FLOAT borePressure = 0.85 * delayLine->lastOut();
  MY_FLOAT deltaPressure = mouthPressure - borePressure; // Differential pressure.
  deltaPressure = lipFilter->tick( deltaPressure );      // Force - > position.
  deltaPressure *= deltaPressure;                        // Basic position to area mapping.
  if ( deltaPressure > 1.0 ) deltaPressure = 1.0;         // Non-linear saturation.
  // The following input scattering assumes the mouthPressure = area.
  lastOutput = deltaPressure * mouthPressure + ( 1.0 - deltaPressure) * borePressure;
  lastOutput = delayLine->tick( dcBlock->tick( lastOutput ) );

  return lastOutput;
}

void Brass :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Brass: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Brass: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_LipTension_)	{ // 2
    MY_FLOAT temp = lipTarget * pow( 4.0, (2.0 * norm) - 1.0 );
    this->setLip(temp);
  }
  else if (number == __SK_SlideLength_) // 4
    delayLine->setDelay( slideTarget * (0.5 + norm) );
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_ ) // 1
    vibratoGain = norm * 0.4;
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    cerr << "Brass: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Brass: controlChange number = " << number << ", value = " << value << endl;
#endif
}
