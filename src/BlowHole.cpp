/***********************************************/
/*
   Waveguide "reed" instrument model with a
   register hole and one tonehole

   by Gary P. Scavone, 2000.
*/
/***********************************************/

#include "BlowHole.h"
#include "SKINI11.msg"

BlowHole :: BlowHole(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  delays = (DLineL *) new DLineL[3];
  // delays[0] is the delay line between the reed and the register vent
  delays[0].setDelay(5.0);
  // delays[1] is the delay line between the register vent and the tonehole
  delays[1].setDelay(length >> 1);
  // delays[2] is the delay line between the tonehole and the end of the bore
  delays[2].setDelay(4.0);
  reedTable = new ReedTabl;
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;

  // Calculate the initial tonehole three-port scattering coefficient
  double r_b = 0.0075;    /* main bore radius */
  r_th = 0.003;          /* tonehole radius */
  scatter = -pow(r_th,2) / ( pow(r_th,2) + 2*pow(r_b,2) );

  // Calculate tonehole coefficients
  MY_FLOAT te = 1.4 * r_th;    /* effective length of the open hole */
  th_coeff = (te*2*SRATE - 347.23) / (te*2*SRATE + 347.23);
  tonehole = new PoleZero;
  // Start with tonehole open
  tonehole->setA1(-th_coeff);
  tonehole->setB0(th_coeff);
  tonehole->setB1(-1.0);

  // Calculate register hole filter coefficients
  double r_rh = 0.0015;    /* register vent radius */
  te = 1.4 * r_rh;       /* effective length of the open hole */
  double xi = 0.0;         /* series resistance term */
  double zeta = 347.23 + 2*PI*pow(r_b,2)*xi/1.1769;
  double psi = 2*PI*pow(r_b,2)*te / (PI*pow(r_rh,2));
  rh_coeff = (zeta - 2*SRATE*psi) / (zeta + 2*SRATE*psi);
  rh_gain = -347.23 / (zeta + 2*SRATE*psi);
  vent = new PoleZero;
  vent->setA1(rh_coeff);
  vent->setB0(1.0);
  vent->setB1(1.0);
  // Start with register vent closed
  vent->setGain(0.0);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibr = new RawWvIn(strcat(file,"rawwaves/sinewave.raw"),"looping");
  vibr->setFreq((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibrGain = (MY_FLOAT) 0.01;
}

BlowHole :: ~BlowHole()
{
  delete [] delays;
  delete reedTable;
  delete filter;
  delete tonehole;
  delete vent;
  delete envelope;
  delete noise;
  delete vibr;
}

void BlowHole :: clear()
{
  delays[0].clear();
  delays[1].clear();
  filter->tick((MY_FLOAT) 0.0);
  tonehole->tick((MY_FLOAT) 0.0);
  vent->tick((MY_FLOAT) 0.0);
}

void BlowHole :: setFreq(MY_FLOAT frequency)
{
  MY_FLOAT new_length = (SRATE / frequency) * (MY_FLOAT) 0.5 - (MY_FLOAT) 1.5;
  new_length -= 9;

  if (new_length <= 1.0) new_length = 1.0;
  else if (new_length >= length) new_length = length;
  delays[1].setDelay(new_length);
}

void BlowHole :: setVent(MY_FLOAT newValue)
{
  /*
    This method allows setting of the register vent "open-ness" at
    any point between "Open" (newValue = 1) and "Closed"
    (newValue = 0).
  */

  MY_FLOAT gain;

  if (newValue <= 0.0) gain = 0.0;
  else if (newValue >= 1.0) gain = rh_gain;
  else gain = newValue * rh_gain;
  vent->setGain(gain);
}

void BlowHole :: setTonehole(MY_FLOAT newValue)
{
  /*
    This method allows setting of the tonehole "open-ness" at
    any point between "Open" (newValue = 1) and "Closed"
    (newValue = 0).
  */
  MY_FLOAT new_coeff;

  if (newValue <= 0.0) new_coeff = 0.9995;
  else if (newValue >= 1.0) new_coeff = th_coeff;
  else new_coeff = (newValue * (th_coeff - 0.9995)) + 0.9995;
  tonehole->setA1(-new_coeff);
  tonehole->setB0(new_coeff);
}

void BlowHole :: startBlowing(MY_FLOAT amplitude,MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void BlowHole :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void BlowHole :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->startBlowing((MY_FLOAT) 0.55 + (amp * (MY_FLOAT) 0.30),amp * (MY_FLOAT) 0.005);
  outputGain = amp + (MY_FLOAT) 0.001;
}

void BlowHole :: noteOff(MY_FLOAT amp)
{
  this->stopBlowing(amp * (MY_FLOAT) 0.01);
}

MY_FLOAT BlowHole :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
  MY_FLOAT temp;

  // Calculate the breath pressure (envelope + noise + vibrator)
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * noiseGain * noise->tick();
  breathPressure += breathPressure * vibrGain * vibr->tick();

  // Calculate the differential pressure = reflected - mouthpiece pressures
  pressureDiff = delays[0].lastOut() - breathPressure;

  // Do two-port junction scattering for register vent
  MY_FLOAT pa = breathPressure + pressureDiff * reedTable->lookup(pressureDiff);
  MY_FLOAT pb = delays[1].lastOut();
  vent->tick(pa+pb);

  lastOutput = delays[0].tick(vent->lastOut()+pb);
  lastOutput *= outputGain;

  // Do three-port junction scattering (under tonehole)
  pa += vent->lastOut();
  pb = delays[2].lastOut();
  MY_FLOAT pth = tonehole->lastOut();
  temp = scatter * (pa + pb - 2 * pth);

  delays[2].tick(filter->tick(pa + temp) * -0.95);
  delays[1].tick(pb + temp);
  tonehole->tick(pa + pb - pth + temp);

  return lastOutput;
}

void BlowHole :: controlChange(int number, MY_FLOAT value)
{
  if (number == __SK_ReedStiffness_)
    reedTable->setSlope((MY_FLOAT) -0.44 + ((MY_FLOAT) 0.26 * value * NORM_7));
  else if (number == __SK_NoiseLevel_)
    noiseGain = (value * NORM_7 * (MY_FLOAT) 0.4);
  else if (number == __SK_ModFrequency_)
    //vibr->setFreq((value * NORM_7 * (MY_FLOAT) 12.0));
    this->setTonehole(value * NORM_7);
  else if (number == __SK_ModWheel_)
    //vibrGain = (value * NORM_7 * (MY_FLOAT) 0.5);
    this->setVent(value * NORM_7);
  else if (number == __SK_AfterTouch_Cont_) {
    envelope->setValue(value * NORM_7);
  }
  else  {
    printf("BlowHole : Undefined Control Number!!\n");
  }
}
