/*******************************************/
/*  Master Class for Simple Instrument     */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains 1 looped      */
/*  wave, 1 noise source, 1 biquad filter  */
/*  1 one-pole filter, and 1 ADSR envelope */
/*******************************************/

#include "Simple.h"

Simple :: Simple()
{
  MY_FLOAT coeffs[2];
  adsr = new ADSR;
  baseFreq = (MY_FLOAT) 440.0;

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  loop = new RawWvIn(strcat(file,"rawwaves/impuls10.raw"),"looping");
  filter = new OnePole;
  noise = new Noise;
  bqpoles = new TwoPole;
  bqzeroes = new TwoZero;
  coeffs[0] = 0;
  coeffs[1] = -1;
  bqzeroes->setZeroCoeffs(coeffs);
  filter->setPole(0.5);
  this->setFreq(baseFreq);
  loopGain = 0.5;
}  

Simple :: ~Simple()
{
  delete adsr;
  delete loop;
  delete filter;
  delete bqzeroes;
  delete bqpoles;
}

void Simple :: keyOn()
{
  adsr->keyOn();
}

void Simple :: keyOff()
{
  adsr->keyOff();
}

void Simple :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->keyOn();
  this->setFreq(freq);
  filter->setGain(amp); 
#if defined(_debug_)        
  printf("Simple : NoteOn: Freq= %lf, Amp=%lf\n",freq, amp);
#endif    
}
void Simple :: noteOff(MY_FLOAT amplitude)
{
  this->keyOff();
#if defined(_debug_)        
  printf("Simple : NoteOff: Amp=%lf\n",amplitude);
#endif    
}

void Simple :: setFreq(MY_FLOAT frequency)
{
#define R 0.98

  MY_FLOAT coeffs[2];
  coeffs[0] = 2 * R * cos(TWO_PI * ONE_OVER_SRATE * frequency);
  coeffs[1] = - R * R;
  bqpoles->setPoleCoeffs(coeffs);
  bqpoles->setGain(1.0 - R);
  loop->setFreq(frequency);
}

MY_FLOAT Simple :: tick()
{
  lastOutput = loopGain * loop->tick();
  bqzeroes->tick(bqpoles->tick(noise->tick()));
  lastOutput += (1.0 - loopGain) * bqzeroes->lastOut();
  lastOutput = filter->tick(lastOutput);
  lastOutput *= adsr->tick();
  return lastOutput;
}

#include "SKINI11.msg"

void Simple :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("Simple : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_Breath_)
    filter->setPole(0.99 * (1.0 - (value * NORM_7 * 2.0)));
  else if (number == __SK_NoiseLevel_)
    loopGain = 1.0 - (NORM_7 * value);
  else if (number == __SK_ModFrequency_)	{
    adsr->setAttackRate(value * NORM_7);
    adsr->setDecayRate(value * NORM_7);
    adsr->setReleaseRate(value * NORM_7);
  }
  else if (number == __SK_ModWheel_)
    printf("Mod Wheel Unimplemented\n");
  else if (number == __SK_AfterTouch_Cont_)
    adsr->setTarget(value * NORM_7);
  else    {        
    printf("Simple : Undefined Control Number!!\n");
  }   
}
