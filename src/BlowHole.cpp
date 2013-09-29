/***************************************************/
/*! \class BlowHole
    \brief STK clarinet physical model with one
           register hole and one tonehole.

    This class is based on the clarinet model,
    with the addition of a two-port register hole
    and a three-port dynamic tonehole
    implementation, as discussed by Scavone and
    Cook (1998).

    In this implementation, the distances between
    the reed/register hole and tonehole/bell are
    fixed.  As a result, both the tonehole and
    register hole will have variable influence on
    the playing frequency, which is dependent on
    the length of the air column.  In addition,
    the highest playing freqeuency is limited by
    these fixed lengths.

    This is a digital waveguide model, making its
    use possibly subject to patents held by Stanford
    University, Yamaha, and others.

    Control Change Numbers: 
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "BlowHole.h"
#include "SKINI.msg"
#include <math.h>
#include <string.h>

BlowHole :: BlowHole(MY_FLOAT lowestFrequency)
{
  length = (long) (Stk::sampleRate() / lowestFrequency + 1);
  // delays[0] is the delay line between the reed and the register vent.
  delays[0] = (DelayL *) new DelayL( 5.0 * Stk::sampleRate() / 22050.0, 100 );
  // delays[1] is the delay line between the register vent and the tonehole.
  delays[1] = (DelayL *) new DelayL( length >> 1, length );
  // delays[2] is the delay line between the tonehole and the end of the bore.
  delays[2] = (DelayL *) new DelayL( 4.0 * Stk::sampleRate() / 22050.0, 100 );
  reedTable = new ReedTabl();
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Calculate the initial tonehole three-port scattering coefficient
  double r_b = 0.0075;    // main bore radius
  r_th = 0.003;          // tonehole radius
  scatter = -pow(r_th,2) / ( pow(r_th,2) + 2*pow(r_b,2) );

  // Calculate tonehole coefficients
  MY_FLOAT te = 1.4 * r_th;    // effective length of the open hole
  th_coeff = (te*2*Stk::sampleRate() - 347.23) / (te*2*Stk::sampleRate() + 347.23);
  tonehole = new PoleZero;
  // Start with tonehole open
  tonehole->setA1(-th_coeff);
  tonehole->setB0(th_coeff);
  tonehole->setB1(-1.0);

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    // register vent radius
  te = 1.4 * r_rh;       // effective length of the open hole
  double xi = 0.0;         // series resistance term
  double zeta = 347.23 + 2*PI*pow(r_b,2)*xi/1.1769;
  double psi = 2*PI*pow(r_b,2)*te / (PI*pow(r_rh,2));
  rh_coeff = (zeta - 2 * Stk::sampleRate() * psi) / (zeta + 2 * Stk::sampleRate() * psi);
  rh_gain = -347.23 / (zeta + 2 * Stk::sampleRate() * psi);
  vent = new PoleZero;
  vent->setA1(rh_coeff);
  vent->setB0(1.0);
  vent->setB1(1.0);
  // Start with register vent closed
  vent->setGain(0.0);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibrato = new WaveLoop( strcat(file,"sinewave.raw"), TRUE );
  vibrato->setFrequency((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibratoGain = (MY_FLOAT) 0.01;
}

BlowHole :: ~BlowHole()
{
  delete delays[0];
  delete delays[1];
  delete delays[2];
  delete reedTable;
  delete filter;
  delete tonehole;
  delete vent;
  delete envelope;
  delete noise;
  delete vibrato;
}

void BlowHole :: clear()
{
  delays[0]->clear();
  delays[1]->clear();
  delays[2]->clear();
  filter->tick((MY_FLOAT) 0.0);
  tonehole->tick((MY_FLOAT) 0.0);
  vent->tick((MY_FLOAT) 0.0);
}

void BlowHole :: setFrequency(MY_FLOAT frequency)
{
  MY_FLOAT freakency = frequency;
  if ( frequency <= 0.0 ) {
    cerr << "BlowHole: setFrequency parameter is less than or equal to zero!" << endl;
    freakency = 220.0;
  }

  // Delay = length - approximate filter delay.
  MY_FLOAT delay = (Stk::sampleRate() / freakency) * 0.5 - 3.5;
  delay -= delays[0]->getDelay() + delays[2]->getDelay();

  if (delay <= 0.0) delay = 0.3;
  else if (delay > length) delay = length;
  delays[1]->setDelay(delay);
}

void BlowHole :: setVent(MY_FLOAT newValue)
{
  // This method allows setting of the register vent "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).

  MY_FLOAT gain;

  if (newValue <= 0.0) gain = 0.0;
  else if (newValue >= 1.0) gain = rh_gain;
  else gain = newValue * rh_gain;
  vent->setGain(gain);
}

void BlowHole :: setTonehole(MY_FLOAT newValue)
{
  // This method allows setting of the tonehole "open-ness" at
  // any point between "Open" (newValue = 1) and "Closed"
  // (newValue = 0).
  MY_FLOAT new_coeff;

  if (newValue <= 0.0) new_coeff = 0.9995;
  else if (newValue >= 1.0) new_coeff = th_coeff;
  else new_coeff = (newValue * (th_coeff - 0.9995)) + 0.9995;
  tonehole->setA1(-new_coeff);
  tonehole->setB0(new_coeff);
}

void BlowHole :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void BlowHole :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void BlowHole :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  setFrequency(frequency);
  startBlowing((MY_FLOAT) 0.55 + (amplitude * 0.30), amplitude * 0.005);
  outputGain = amplitude + 0.001;

#if defined(_STK_DEBUG_)
  cerr << "BlowHole: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void BlowHole :: noteOff(MY_FLOAT amplitude)
{
  this->stopBlowing(amplitude * 0.01);

#if defined(_STK_DEBUG_)
  cerr << "BlowHole: NoteOff amplitude = " << amplitude << endl;
#endif
}

MY_FLOAT BlowHole :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrato)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibratoGain * vibrato->tick();

  // Calculate the differential pressure = reflected - mouthpiece pressures
  pressureDiff = delays[0]->lastOut() - breathPressure;

  // Do two-port junction scattering for register vent
  MY_FLOAT pa = breathPressure + pressureDiff * reedTable->tick(pressureDiff);
  MY_FLOAT pb = delays[1]->lastOut();
  vent->tick(pa+pb);

  lastOutput = delays[0]->tick(vent->lastOut()+pb);
  lastOutput *= outputGain;

  // Do three-port junction scattering (under tonehole)
  pa += vent->lastOut();
  pb = delays[2]->lastOut();
  MY_FLOAT pth = tonehole->lastOut();
  temp = scatter * (pa + pb - 2 * pth);

  delays[2]->tick(filter->tick(pa + temp) * -0.95);
  delays[1]->tick(pb + temp);
  tonehole->tick(pa + pb - pth + temp);

  return lastOutput;
}

void BlowHole :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "BlowHole: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "BlowHole: Control value greater than 128.0!" << endl;
  }

  if (number == __SK_ReedStiffness_) // 2
    reedTable->setSlope( -0.44 + (0.26 * norm) );
  else if (number == __SK_NoiseLevel_) // 4
    noiseGain = ( norm * 0.4);
  else if (number == __SK_ModFrequency_) // 11
    this->setTonehole( norm );
  else if (number == __SK_ModWheel_) // 1
    this->setVent( norm );
  else if (number == __SK_AfterTouch_Cont_) // 128
    envelope->setValue( norm );
  else
    cerr << "BlowHole: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "BlowHole: controlChange number = " << number << ", value = " << value << endl;
#endif
}
