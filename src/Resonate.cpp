/***************************************************/
/*! \class Resonate
    \brief STK noise driven formant filter.

    This instrument contains a noise source, which
    excites a biquad resonance filter, with volume
    controlled by an ADSR.

    Control Change Numbers:
       - Resonance Frequency (0-Nyquist) = 2
       - Pole Radii = 4
       - Notch Frequency (0-Nyquist) = 11
       - Zero Radii = 1
       - Envelope Gain = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Resonate.h"
#include "SKINI.msg"

Resonate :: Resonate()
{
  adsr = new ADSR;
  noise = new Noise;

  filter = new BiQuad;
  poleFrequency = 4000.0;
  poleRadius = 0.95;
  // Set the filter parameters.
  filter->setResonance( poleFrequency, poleRadius, TRUE );
  zeroFrequency = 0.0;
  zeroRadius = 0.0;
}  

Resonate :: ~Resonate()
{
  delete adsr;
  delete filter;
  delete noise;
}

void Resonate :: keyOn()
{
  adsr->keyOn();
}

void Resonate :: keyOff()
{
  adsr->keyOff();
}

void Resonate :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  adsr->setTarget( amplitude );
  this->keyOn();
  this->setResonance(frequency, poleRadius);

#if defined(_STK_DEBUG_)
  std::cerr << "Resonate: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << std::endl;
#endif
}
void Resonate :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();

#if defined(_STK_DEBUG_)
  std::cerr << "Resonate: NoteOff amplitude = " << amplitude << std::endl;
#endif
}

void Resonate :: setResonance(MY_FLOAT frequency, MY_FLOAT radius)
{
  poleFrequency = frequency;
  if ( frequency < 0.0 ) {
    std::cerr << "Resonate: setResonance frequency parameter is less than zero!" << std::endl;
    poleFrequency = 0.0;
  }

  poleRadius = radius;
  if ( radius < 0.0 ) {
    std::cerr << "Resonate: setResonance radius parameter is less than 0.0!" << std::endl;
    poleRadius = 0.0;
  }
  else if ( radius >= 1.0 ) {
    std::cerr << "Resonate: setResonance radius parameter is greater than or equal to 1.0, which is unstable!" << std::endl;
    poleRadius = 0.9999;
  }
  filter->setResonance( poleFrequency, poleRadius, TRUE );
}

void Resonate :: setNotch(MY_FLOAT frequency, MY_FLOAT radius)
{
  zeroFrequency = frequency;
  if ( frequency < 0.0 ) {
    std::cerr << "Resonate: setNotch frequency parameter is less than zero!" << std::endl;
    zeroFrequency = 0.0;
  }

  zeroRadius = radius;
  if ( radius < 0.0 ) {
    std::cerr << "Resonate: setNotch radius parameter is less than 0.0!" << std::endl;
    zeroRadius = 0.0;
  }
  
  filter->setNotch( zeroFrequency, zeroRadius );
}

void Resonate :: setEqualGainZeroes()
{
  filter->setEqualGainZeroes();
}

MY_FLOAT Resonate :: tick()
{
  lastOutput = filter->tick(noise->tick());
  lastOutput *= adsr->tick();
  return lastOutput;
}

void Resonate :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    std::cerr << "Resonate: Control value less than zero!" << std::endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    std::cerr << "Resonate: Control value greater than 128.0!" << std::endl;
  }

  if (number == 2) // 2
    setResonance( norm * Stk::sampleRate() * 0.5, poleRadius );
  else if (number == 4) // 4
    setResonance( poleFrequency, norm*0.9999 );
  else if (number == 11) // 11
    this->setNotch( norm * Stk::sampleRate() * 0.5, zeroRadius );
  else if (number == 1)
    this->setNotch( zeroFrequency, norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    adsr->setTarget( norm );
  else
    std::cerr << "Resonate: Undefined Control Number (" << number << ")!!" << std::endl;

#if defined(_STK_DEBUG_)
  std::cerr << "Resonate: controlChange number = " << number << ", value = " << value << std::endl;
#endif
}
