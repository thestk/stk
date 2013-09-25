/******************************************/
/*  Waveguide Brass Instrument Model ala  */
/*  Cook (TBone, HosePlayer)              */
/*  by Perry R. Cook, 1995-96             */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = lipTension   */
/*                CONTROL2 = slideLength  */
/*		  CONTROL3 = vibFreq      */
/*		  MOD_WHEEL= vibAmt       */
/******************************************/

#include "Brass.h"
#include "SKINI11.msg"

Brass :: Brass(MY_FLOAT lowestFreq)
{
  length = (long) (SRATE / lowestFreq + 1);
  delayLine = new DLineA(length);
  lipFilter = new LipFilt;
  dcBlock = new DCBlock;
  adsr = new ADSR;
  adsr->setAllTimes((MY_FLOAT) 0.005, (MY_FLOAT) 0.001, (MY_FLOAT) 1.0, (MY_FLOAT) 0.010);

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibr = new RawWvIn(strcat(file,"rawwaves/sinewave.raw"),"looping");
  this->clear();

  vibr->normalize();
  vibr->setFreq((MY_FLOAT)  6.137);
  vibrGain = (MY_FLOAT)  0.05;            /* breath periodic vibrato component  */

	maxPressure = (MY_FLOAT) 0.0;
}

Brass :: ~Brass()
{
  delete delayLine;
  delete lipFilter;
  delete dcBlock;
  delete adsr;
  delete vibr;
}

void Brass :: clear()
{
  delayLine->clear();
  lipFilter->clear();
  dcBlock->clear();
}

void Brass :: setFreq(MY_FLOAT frequency)
{
  slideTarget = (SRATE / frequency * (MY_FLOAT) 2.0) + (MY_FLOAT) 3.0;
  /* fudge correction for filter delays */
  delayLine->setDelay(slideTarget);   /*  we'll play a harmonic  */
  lipTarget = frequency;
  lipFilter->setFreq(frequency);
}

void Brass :: setLip(MY_FLOAT frequency)
{
  lipFilter->setFreq(frequency);
}

void Brass :: startBlowing(MY_FLOAT amplitude,MY_FLOAT rate)
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

void Brass :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->startBlowing(amp, amp * (MY_FLOAT) 0.001);
#if defined(_debug_)        
  printf("Brass : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Brass :: noteOff(MY_FLOAT amp)
{
  this->stopBlowing(amp * (MY_FLOAT) 0.005);
#if defined(_debug_)        
  printf("Brass : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT Brass :: tick()
{
  MY_FLOAT breathPressure;

  breathPressure = maxPressure * adsr->tick();
  breathPressure += vibrGain * vibr->tick();
  lastOutput = delayLine->tick(                                 /* bore delay  */
               dcBlock->tick(                                   /* block DC    */
               lipFilter->tick((MY_FLOAT) 0.3 * breathPressure, /* mouth input */
                     (MY_FLOAT) 0.85 * delayLine->lastOut()))); /* and bore reflection */
  return lastOutput;
}

void Brass :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT temp;
#if defined(_debug_)        
  printf("Brass : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_LipTension_)	{
    temp = lipTarget * (MY_FLOAT) pow(4.0,(2.0*value*NORM_7) - 1.0);
    this->setLip(temp);
  }
  else if (number == __SK_SlideLength_)
    delayLine->setDelay(slideTarget * ((MY_FLOAT) 0.5 + (value * NORM_7)));  
  else if (number == __SK_ModFrequency_)
		vibr->setFreq((value * NORM_7 * (MY_FLOAT) 12.0));
  else if (number == __SK_ModWheel_ )
		vibrGain = (value * NORM_7 * (MY_FLOAT) 0.4);
  else if (number == __SK_AfterTouch_Cont_)
    adsr->setTarget(value * NORM_7);
  else    {        
    printf("Brass : Undefined Control Number!!\n");
  }  
}
