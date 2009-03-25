/***************************************************/
/*! \class Saxofony
    \brief STK faux conical bore reed instrument class.

    This class implements a "hybrid" digital
    waveguide instrument that can generate a
    variety of wind-like sounds.  It has also been
    referred to as the "blowed string" model.  The
    waveguide section is essentially that of a
    string, with one rigid and one lossy
    termination.  The non-linear function is a
    reed table.  The string can be "blown" at any
    point between the terminations, though just as
    with strings, it is impossible to excite the
    system at either end.  If the excitation is
    placed at the string mid-point, the sound is
    that of a clarinet.  At points closer to the
    "bridge", the sound is closer to that of a
    saxophone.  See Scavone (2002) for more details.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Saxofony.h"
#include "SKINI.msg"

Saxofony :: Saxofony(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // Initialize blowing position to 0.2 of length / 2.
  position = 0.2;
  delays[0] = (DelayL *) new DelayL( (1.0-position) * (length >> 1), length );
  delays[1] = (DelayL *) new DelayL( position * (length >> 1), length );

  reedTable = new ReedTabl;
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) 0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Concatenate the STK rawwave path to the rawwave file
  vibrato = new WaveLoop( (Stk::rawwavePath() + "sinewave.raw").c_str(), TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);

  outputGain = (MY_FLOAT) 0.3;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.1;
}

Saxofony :: ~Saxofony()
{
  delete delays[0];
  delete delays[1];
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibrato;
}

void Saxofony :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Saxofony :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    std::cerr << "Saxofony: setFrequency parameter is less than or equal to zero!" << std::endl;
    freakency = 220.0;
  }

  MY_FLOAT delay = (Stk::sampleRate() / freakency) - (MY_FLOAT) 3.0;
  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;

  delays[0]->setDelay((1.0-position) * delay);
  delays[1]->setDelay(position * delay);
}

void Saxofony :: setBlowPosition(MY_FLOAT aPosition)
{
  if (position == aPosition) return;

  if (aPosition < 0.0) position = 0.0;
  else if (aPosition > 1.0) position = 1.0;
  else position = aPosition;

  MY_FLOAT total_delay = delays[0]->getDelay();
  total_delay += delays[1]->getDelay();

  delays[0]->setDelay((1.0-position) * total_delay);
  delays[1]->setDelay(position * total_delay);
}

void Saxofony :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Saxofony :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0);
}

void Saxofony :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  std::cerr << "Saxofony: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}

void Saxofony :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  std::cerr << "Saxofony: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

MY_FLOAT Saxofony :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  temp = -0.95 * filter->tick( delays[0]->lastOut() );
  lastOutput = temp - delays[1]->lastOut();
  pressureDiff = breathPressure - lastOutput;
  delays[1]->tick(temp);
  delays[0]->tick(breathPressure - (pressureDiff * reedTable->tick(pressureDiff)) - temp);

  lastOutput *= outputGain;
  return lastOutput;
}

void Saxofony :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "Saxofony: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "Saxofony: Control value greater than 128.0!" << std::endl;
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable->setSlope( 0.1 + (0.4 * norm) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4 );
  else if (number == 29) // 29
    vibrato->setFrequency( norm * 12.0 );
  else if (number == __SK_ModWheel_) // 1
    vibratoGain = ( norm * 0.5 );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue( norm );
  else if (number == 11) // 11
    this->setBlowPosition( norm );
  else if (number == 26) // reed table offset
    reedTable->setOffset(0.4 + ( norm * 0.6));
  else
    std::cerr << "Saxofony: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "Saxofony: controlChange number = " << number << ", value = " << value << std::endl;
#endif

}
