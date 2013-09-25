/*********************************************/
/*  Bowed Bar model                          */
/*  by Georg Essl, 1999                      */
/*  For details refer to:                    */
/*    G.Essl, P.R.Cook: "Banded Waveguides:  */
/*    Towards Physical Modelling of Bar      */
/*    Percussion Instruments", ICMC'99       */
/*********************************************/

#include "BowedBar.h"
#include "SKINI11.msg"
#include "Noise.h"

/* Contructor */
BowedBar :: BowedBar()
{
  long i;
  doPluck = 1;

  /* Number of banded waveguide modes */
  NR_MODES = 4;

  modes[0] = (MY_FLOAT) 1.0;
  modes[1] = (MY_FLOAT) 2.756;
  modes[2] = (MY_FLOAT) 5.404;
  modes[3] = (MY_FLOAT) 8.933;

  for (i=0;i<4;i++)	{
    gains[i] = (MY_FLOAT) pow(0.9,(double) i);
  }
  
  bowTabl = new BowTabl;
  adsr = new ADSR;
  bandpass = new BiQuad[NR_MODES];
  
  bowTabl->setSlope((MY_FLOAT) 3.0);
  adsr->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.005,(MY_FLOAT) 0.9,(MY_FLOAT) 0.01);
    
  freq = SRATE / 100;
  length = 100;
  
  bowPos = 0;
  lastBowPos = 0;

  for(i = 0; i<NR_MODES; i++) {
    delay[i].setDelay((int)(length/modes[i]));
    delay[i].clear();
    
    bandpass[i].clear();
    Zs[i][1] = 0.0;
    Zs[i][2] = 0.0;
    filtOut[i] = 0.0;
    filtIn[i] = 0.0;
  }
  
  R = (MY_FLOAT) 0.97;
  GAIN = (MY_FLOAT) 0.999;
  
  slope = (MY_FLOAT) 3.0;
  
  this->tuneBandPasses();
}

BowedBar :: ~BowedBar()
{
  delete bowTabl;
  delete adsr;
  delete [] bandpass;
}

void BowedBar :: clear()
{
  long i;
  
  for(i = 0; i<NR_MODES; i++) {
    delay[i].clear();
    
    bandpass[i].clear();
    Zs[i][1] = 0.0;
    Zs[i][2] = 0.0;
    filtOut[i] = 0.0;
    filtIn[i] = 0.0;
  }
}

void BowedBar :: setFreq(MY_FLOAT frequency)
{
  int i;
  freq = frequency;

  if(freq > 1568.0) freq = 1568.0;

  length = (int)(SRATE/freq);
  
  NR_MODES = 4;
  for(i = 0; i<NR_MODES; i++) {
    if((int)(length/modes[i]) > 4)
      delay[i].setDelay((int)(length/modes[i]));
    else	{
      NR_MODES = i;
      break;
    }
    /*   FIX THIS BETTER!!!!! */
    delay[i].clear();
    
    bandpass[i].clear();
    Zs[i][1] = 0.0;
    Zs[i][2] = 0.0;
    filtOut[i] = 0.0;
    filtIn[i] = 0.0;
  }
  
  tuneBandPasses();
}

void BowedBar :: setStrikePosition(MY_FLOAT position)
{
  MY_FLOAT temp2;
  temp2 = position * PI;
  gains[0] = fabs(sin(temp2 / 2) * pow(0.9,0));                                       
  gains[1] = fabs(sin(temp2) * pow(0.9,1));
  gains[2] = fabs(sin(temp2 * 3 / 2) * pow(0.9,2));
  gains[3] = fabs(sin(temp2 * 2) * pow(0.9,3));
}

void BowedBar :: tuneBandPasses()
{
  long i;
  
  for(i=0; i<NR_MODES; i++) {
    R = 1 - 6.28318530718 * freq * modes[i] / SRATE / 2.0;
    bandpass[i].setFreqAndReson(freq * modes[i], R);
    bandpass[i].setEqualGainZeroes();
    bandpass[i].setGain((1.0-R*R)/2.0);
    filtGain[i] = (MY_FLOAT) (1.0 - (R*R))/2.0;
    coeffs[i][1] = -R * R;
    coeffs[i][0] = 2.0 * R * cos(6.28318530718 * freq * modes[i] / SRATE);
    
    delay[i].clear(); //(rand()) - 16384;
  }
}

void BowedBar :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOn();
  maxVelocity = (MY_FLOAT) 0.03 + ((MY_FLOAT) 0.2 * amplitude); 
  maxVelocity = (MY_FLOAT) 0.03 + ((MY_FLOAT) 0.5 * amplitude); 
}

void BowedBar :: stopBowing(MY_FLOAT rate)
{
  adsr->setRate(rate);
  adsr->keyOff();
}

void BowedBar :: pluck(MY_FLOAT amplitude)
{
  long i,j;
  int pluckLen;
  MY_FLOAT temp;
  Noise noise;
  
  pluckLen = (int)(length/modes[NR_MODES-1]);
  for (j=1;j<pluckLen/2;j++)	{
    temp = amplitude*2.0*noise.tick();
    for(i=0; i<NR_MODES; i++)
      delay[i].tick(temp*j/pluckLen*gains[i]);
  }
  for (j=pluckLen/2;j>0;j--)	{
    temp = amplitude*2.0*noise.tick();
    for(i=0; i<NR_MODES; i++)
      delay[i].tick(temp*j/pluckLen*gains[i]);
  }
}

void BowedBar :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  if(!doPluck) {
    for(int i=0; i<NR_MODES ; i++)
      bandpass[i].clear();
  
    this->startBowing(amp,amp * (MY_FLOAT) 0.001);
    this->setFreq(freq);
    doPluck = 0;
#if defined(_debug_)        
    printf("BowedBar : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
  }
  else {
    for(int i=0; i<NR_MODES ; i++)
      bandpass[i].clear();
    this->setFreq(freq);
    this->pluck(amp);
    doPluck = 1;
  }
}

void BowedBar :: noteOff(MY_FLOAT amp)
{
  if(!doPluck) {
    this->stopBowing(((MY_FLOAT) 1.0 - amp) * (MY_FLOAT) 0.005);
  }
#if defined(_debug_)        
  printf("BowedBar : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT BowedBar :: tick()
{
  long k;
  MY_FLOAT input;
  MY_FLOAT data;
  
  data = 0.0;
      
  input = 0.0;
  
  if(integration_const == 0.0)
    velinput = 0.0;
  else
    velinput = integration_const * velinput;
  
  for(k=0; k<NR_MODES; k++)
    {
      velinput += GAIN * delay[k].lastOut();
    }
  
  if(trackVel)  {
    bowvel *= 0.9995;
    bowvel += bowTarg;
    bowTarg *= 0.995;
  }
  else
    {
      bowvel = adsr->tick()*maxVelocity;
    }

  if(doPluck)
    {
      input = 0.0;
    }
  else
    {
      input = bowvel - velinput;
  
      input = input * bowTabl->lookup(input);
  
      input = input/(MY_FLOAT)NR_MODES;
    }
  
  for(k=0; k<NR_MODES; k++)
    {
      bandpass[k].tick(input*gains[k] + GAIN * delay[k].lastOut());
      delay[k].tick(bandpass[k].lastOut());
      data += bandpass[k].lastOut();
    }
  
  lastOutput = data * 4.0;
  return lastOutput;
}

void BowedBar :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("BowedBar : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_BowPressure_)
    {
      bowTabl->setSlope((MY_FLOAT) 10.0 - ((MY_FLOAT) 9.0 * value * NORM_7));
      slope = (MY_FLOAT) 10.0 - ((MY_FLOAT) 9.0 * value * NORM_7);
    }
  else if (number == __SK_BowPosition_)
    {
      this->setStrikePosition(value*NORM_7);
    }
  else if (number == __SK_Balance_)	{
    bowPos = value * NORM_7;
    bowTarg += 0.02 * (bowPos - lastBowPos);
    lastBowPos = bowPos;
    adsr->setTarget(bowPos);
  }
  else if (number == __SK_AfterTouch_Cont_)
    {
      bowPos = value * NORM_7;
      bowTarg += 0.02 * (bowPos - lastBowPos);
      lastBowPos = bowPos;
      adsr->setTarget(bowPos);
    }      
  else if (number == __SK_ModWheel_)
    {
      GAIN = 0.809 + (0.2 * (value * NORM_7));
    }
  else if(number == __SK_ModFrequency_)
    {
      integration_const = value * NORM_7;
      
    }
  else if(number == __SK_Sustain_)	{
    if(value < 65)
      doPluck = 1;
    else
      doPluck = 0;
  }
  else if(number == __SK_Portamento_)	{
    if(value < 65)
      trackVel = 0;
    else
      trackVel = 1;
  }
  else    {        
    printf("BowedBar : Undefined Control Number!!\n");
  }    
}
