/******************************************/
/*  Test Sampler Subclass of              */                   
/*  Sampling Synthesizer Class            */
/*  by Perry R. Cook, 1995-96             */ 
/*                                        */
/*   Controls:    CONTROL1 = filterQ      */
/*                CONTROL2 = filterRate   */
/*                CONTROL3 = vibFreq      */
/*                MOD_WHEEL= vibAmt       */
/******************************************/

#include "Moog1.h"
#include "SKINI11.msg"

Moog1 :: Moog1() : SamplFlt()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char temp[128];
  char file[128];
  strcpy(temp, RAWWAVE_PATH);
  strcpy(file,temp);
  attacks[0] = new RawWvIn(strcat(file,"rawwaves/mandpluk.raw"),"oneshot");
  strcpy(file,temp);
  loops[0] = new RawWvIn(strcat(file,"rawwaves/impuls20.raw"),"looping");
  strcpy(file,temp);
  loops[1] = new RawWvIn(strcat(file,"rawwaves/sinewave.raw"),"looping"); /* Steal one for vibrato */
  attacks[0]->normalize();
  loops[0]->normalize();
  loops[1]->normalize();
  loops[1]->setFreq((MY_FLOAT) 6.122);
  adsr->setAllTimes((MY_FLOAT) 0.001,(MY_FLOAT) 1.5,(MY_FLOAT) 0.6,(MY_FLOAT) 0.250);
  filterQ = (MY_FLOAT) 0.85;
  filterRate = (MY_FLOAT) 0.0001;
  modDepth = (MY_FLOAT) 0.0;
}  

Moog1 :: ~Moog1()
{
  delete attacks[0];
  delete loops[0];
  delete loops[1];
}

void Moog1 :: setFreq(MY_FLOAT frequency)
{    
  baseFreq = frequency;
  attacks[0]->setFreq(baseFreq * (MY_FLOAT) 0.01);
  loops[0]->setFreq(baseFreq);
}

void Moog1 :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  MY_FLOAT temp;
    
  this->setFreq(freq);
  this->keyOn();
  attackGain = amp * (MY_FLOAT) 0.5;
  loopGain = amp;

  temp = filterQ + (MY_FLOAT) 0.05;
  filters[0]->setStates((MY_FLOAT) 2000.0,temp,(MY_FLOAT) 2.0 * ((MY_FLOAT) 1.0 - temp));
  filters[1]->setStates((MY_FLOAT) 2000.0,temp,(MY_FLOAT) 2.0 * ((MY_FLOAT) 1.0 - temp));
  temp = filterQ + (MY_FLOAT) 0.099;
  filters[0]->setTargets((MY_FLOAT) freq,temp,(MY_FLOAT) 2.0 * ((MY_FLOAT) 1.0 - temp));
  filters[1]->setTargets((MY_FLOAT) freq,temp,(MY_FLOAT) 2.0 * ((MY_FLOAT) 1.0 - temp));
  filters[0]->setSweepRate(filterRate * (MY_FLOAT)  22050.0 / SRATE);
  filters[1]->setSweepRate(filterRate * (MY_FLOAT)  22050.0 / SRATE);
#if defined(_debug_)        
  printf("Moog1 : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Moog1 :: setModulationSpeed(MY_FLOAT mSpeed)
{
  loops[1]->setFreq(mSpeed);
}

void Moog1 :: setModulationDepth(MY_FLOAT mDepth)
{
  modDepth = mDepth * (MY_FLOAT) 0.5;
}

void Moog1 :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("Moog1 : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_FilterQ_)
    filterQ = (MY_FLOAT) 0.80 + ((MY_FLOAT) 0.1 * value * NORM_7);
  else if (number == __SK_FilterSweepRate_)
    filterRate = (value * NORM_7 * (MY_FLOAT) 0.0002);
  else if (number == __SK_ModFrequency_)
    this->setModulationSpeed(value * NORM_7 * (MY_FLOAT) 12.0);
  else if (number == __SK_ModWheel_)
    this->setModulationDepth(value * NORM_7);
  else if (number == __SK_AfterTouch_Cont_)
    adsr->setTarget(value * NORM_7);
  else    {
    printf("Moog1 : Undefined Control Number!!\n");
  }    
}

MY_FLOAT Moog1 :: tick()
{
  MY_FLOAT temp;

  if (modDepth!=0.0) {
    temp = loops[1]->tick() * modDepth;    
    loops[0]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp));
  }
  lastOutput = SamplFlt :: tick();
  return lastOutput;
}

