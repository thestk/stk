/******************************************/
/*  Waveguide Clarinet model ala Smith    */
/*  after McIntyre, Schumacher, Woodhouse */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = reedStiffns  */
/*                CONTROL2 = noiseGain    */
/*                CONTROL3 = vibFreq      */
/*                MOD_WHEEL= vibAmt       */
/******************************************/

#include "Clarinet.h"
#include "SKINI11.msg"

Clarinet :: Clarinet(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  delayLine = new DLineL(length);
  reedTable = new ReedTabl;
  reedTable->setOffset((MY_FLOAT) 0.7);
  reedTable->setSlope((MY_FLOAT) -0.3);
  filter = new OneZero;
  envelope = new Envelope;
  noise = new Noise;
  vibr = new RawLoop("rawwaves/sinewave.raw");
  vibr->normalize();
  vibr->setFreq((MY_FLOAT) 5.735);
  outputGain = (MY_FLOAT) 1.0;
  noiseGain = (MY_FLOAT) 0.2;
  vibrGain = (MY_FLOAT) 0.1;
}

Clarinet :: ~Clarinet()
{
  delete delayLine;
  delete reedTable;
  delete filter;
  delete envelope;
  delete noise;
  delete vibr;
}

void Clarinet :: clear()
{
  delayLine->clear();
  filter->tick((MY_FLOAT) 0.0);
}

void Clarinet :: setFreq(MY_FLOAT frequency)
{
  delayLine->setDelay         /* length - approx filter delay */
    ((SRATE / frequency) * (MY_FLOAT) 0.5 - (MY_FLOAT) 1.5);
}

void Clarinet :: startBlowing(MY_FLOAT amplitude,MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget(amplitude); 
}

void Clarinet :: stopBlowing(MY_FLOAT rate)
{
  envelope->setRate(rate);
  envelope->setTarget((MY_FLOAT) 0.0); 
}

void Clarinet :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->startBlowing((MY_FLOAT) 0.55 + (amp * (MY_FLOAT) 0.30),amp * (MY_FLOAT) 0.005);
  outputGain = amp + (MY_FLOAT) 0.001;
#if defined(_debug_)        
  printf("Clarinet : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Clarinet :: noteOff(MY_FLOAT amp)
{
  this->stopBlowing(amp * (MY_FLOAT) 0.01);
#if defined(_debug_)        
  printf("Clarinet : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT Clarinet :: tick()
{
  MY_FLOAT pressureDiff;
  MY_FLOAT breathPressure;
    
  breathPressure = envelope->tick(); 
  breathPressure += breathPressure * 
    noiseGain * noise->tick();
  breathPressure += breathPressure *
    vibrGain * vibr->tick();
  pressureDiff = filter->tick(delayLine->lastOut());           /* differential pressure  */     
  pressureDiff = (pressureDiff * (MY_FLOAT) -0.95) - breathPressure; /* of reflected and mouth */
  lastOutput = delayLine->tick(breathPressure +                /* perform scattering     */
                               pressureDiff * reedTable->lookup(pressureDiff)); /* in economical way      */
  lastOutput *= outputGain;
  return lastOutput;
}

void Clarinet :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("Clarinet : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_ReedStiffness_)
    reedTable->setSlope((MY_FLOAT) -0.44 + ((MY_FLOAT) 0.26 * value * NORM_7));
  else if (number == __SK_NoiseLevel_)
    noiseGain = (value * NORM_7 * (MY_FLOAT) 0.4);
  else if (number == __SK_ModFrequency_)
    vibr->setFreq((value * NORM_7 * (MY_FLOAT) 12.0));
  else if (number == __SK_ModWheel_)
    vibrGain = (value * NORM_7 * (MY_FLOAT) 0.5);
  else if (number == __SK_AfterTouch_Cont_) {
    envelope->setValue(value * NORM_7);
  }
  else  {
    printf("Clarinet : Undefined Control Number!!\n");
  }
}
