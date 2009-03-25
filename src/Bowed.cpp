/***************************************************/
/*! \class Bowed
    \brief STK bowed string instrument class.

    This class implements a bowed string model, a
    la Smith (1986), after McIntyre, Schumacher,
    Woodhouse (1983).

    This is a digital waveguide model, making its
    use possibly subject to patents held by
    Stanford University, Yamaha, and others.

    Control Change Numbers: 
       - Bow Pressure = 2
       - Bow Position = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Volume = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Bowed.h"
#include "SKINI.msg"
#include <string.h>

Bowed :: Bowed(MY_FLOAT lowestFrequency)
{
  long length;
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  neckDelay = new DelayL(100.0, length);
  length >>= 1;
  bridgeDelay = new DelayL(29.0, length);

  bowTable = new BowTabl;
  bowTable->setSlope((MY_FLOAT) 3.0);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"sinewave.raw"), TRUE );
  vibrato->setFrequency((MY_FLOAT) 6.12723);
  vibratoGain = (MY_FLOAT) 0.0;

  stringFilter = new OnePole;
  stringFilter->setPole((MY_FLOAT) (0.6 - (0.1 * 22050.0 / Stk::sampleRate() ) ) );
  stringFilter->setGain((MY_FLOAT) 0.95);

  bodyFilter = new BiQuad;
  bodyFilter->setResonance( 500.0, 0.85, TRUE );
  bodyFilter->setGain((MY_FLOAT) 0.2);

  adsr = new ADSR;
  adsr->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.005,(MY_FLOAT) 0.9,(MY_FLOAT) 0.01);
    
  betaRatio = (MY_FLOAT) 0.127236;

  // Necessary to initialize internal variables.
  setFrequency( 220.0 );
}

Bowed :: ~Bowed()
{
  delete neckDelay;
  delete bridgeDelay;
  delete bowTable;
  delete stringFilter;
  delete bodyFilter;
  delete vibrato;
  delete adsr;
}

void Bowed :: clear()
{
  neckDelay->clear();
  bridgeDelay->clear();
}

void Bowed :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "Bowed: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  baseDelay = Stk::sampleRate() / freakency - (MY_FLOAT) 4.0;
  if ( baseDelay <= 0.0 ) baseDelay = 0.3;
  bridgeDelay->setDelay(baseDelay * betaRatio); 	               // bow to bridge length
  neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio)); // bow to nut (finger) length
}

void Bowed :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = (MY_FLOAT) 0.03 + ((MY_FLOAT) 0.2 * amplitude); 
}

void Bowed :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void Bowed :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  this->startBowing(amplitude, amplitude * 0.001);
  this->setFrequency(frequency);

#if defined(_STK_DEBUG_)
  cerr << "Bowed: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Bowed :: noteOff(MY_FLOAT amplitude)
{
  this->stopBowing(((MY_FLOAT) 1.0 - amplitude) * (MY_FLOAT) 0.005);

#if defined(_STK_DEBUG_)
  cerr << "Bowed: NoteOff amplitude = " << amplitude << endl;
#endif
}

void Bowed :: setVibrato(MY_FLOAT gain)
{
  vibratoGain = gain;
}

MY_FLOAT Bowed :: tick()
{
  MY_FLOAT bowVelocity;
  MY_FLOAT bridgeRefl;
  MY_FLOAT nutRefl;
  MY_FLOAT newVel;
  MY_FLOAT velDiff;
  MY_FLOAT stringVel;
    
  bowVelocity = maxVelocity * adsr->tick();

  bridgeRefl = -stringFilter->tick( bridgeDelay->lastOut() );
  nutRefl = -neckDelay->lastOut();
  stringVel = bridgeRefl + nutRefl;               // Sum is String Velocity
  velDiff = bowVelocity - stringVel;              // Differential Velocity
  newVel = velDiff * bowTable->tick( velDiff );   // Non-Linear Bow Function
  neckDelay->tick(bridgeRefl + newVel);           // Do string propagations
  bridgeDelay->tick(nutRefl + newVel);
    
  if (vibratoGain > 0.0)  {
    neckDelay->setDelay((baseDelay * ((MY_FLOAT) 1.0 - betaRatio)) + 
                        (baseDelay * vibratoGain * vibrato->tick()));
  }

  lastOutput = bodyFilter->tick(bridgeDelay->lastOut());                 

  return lastOutput;
}

void Bowed :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Bowed: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Bowed: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_BowPressure_) // 2
		bowTable->setSlope( 5.0 - (4.0 * norm) );
  else if (number == __SK_BowPosition_) { // 4
		betaRatio = 0.027236 + (0.2 * norm);
    bridgeDelay->setDelay(baseDelay * betaRatio);
    neckDelay->setDelay(baseDelay * ((MY_FLOAT) 1.0 - betaRatio));
  }
  else if (number == __SK_ModFrequency_) // 11
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.4 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget(norm);
  else
    cerr << "Bowed: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Bowed: controlChange number = " << number << ", value = " << value << endl;
#endif
}
