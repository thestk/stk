/********************************************/  
/*  Commuted Mandolin Subclass of enhanced  */
/*  dual plucked-string model               */
/*  by Perry Cook, 1995-96                  */
/*   Controls:    CONTROL1 = bodySize       */
/*                CONTROL2 = pluckPosition  */
/*                CONTROL3 = loopGain       */
/*                MOD_WHEEL= deTuning       */
/*                                          */
/*  Note: Commuted Synthesis, as with many  */
/*  other WaveGuide techniques, is covered  */
/*  by patents, granted, pending, and/or    */
/*  applied-for.  Many are assigned to the  */
/*  Board of Trustees, Stanford University. */
/*  For information, contact the Office of  */ 
/*  Technology Licensing, Stanford U.       */
/********************************************/  

#include "Mandolin.h"
#include "SKINI11.msg"

Mandolin :: Mandolin(MY_FLOAT lowestFreq) : Plucked2(lowestFreq)
{
  int i;

  soundfile[0] = new RawInterp("rawwaves/mand1.raw");
  soundfile[1] = new RawInterp("rawwaves/mand2.raw");
  soundfile[2] = new RawInterp("rawwaves/mand3.raw");
  soundfile[3] = new RawInterp("rawwaves/mand4.raw");
  soundfile[4] = new RawInterp("rawwaves/mand5.raw");
  soundfile[5] = new RawInterp("rawwaves/mand6.raw");
  soundfile[6] = new RawInterp("rawwaves/mand7.raw");
  soundfile[7] = new RawInterp("rawwaves/mand8.raw");
  soundfile[8] = new RawInterp("rawwaves/mand9.raw");
  soundfile[9] = new RawInterp("rawwaves/mand10.raw");
  soundfile[10] = new RawInterp("rawwaves/mand11.raw");
  soundfile[11] = new RawInterp("rawwaves/mand12.raw");
  for (i=0;i<12;i++)	{
    //        soundfile[i]->normalize((MY_FLOAT) 0.1); /*  Empirical hack here  */
    soundfile[i]->setLooping(0);
  }
  directBody = 1.0;
  mic = 0;
  dampTime = 0;
  waveDone = 1;
}

Mandolin :: ~Mandolin()
{
}

void Mandolin :: pluck(MY_FLOAT amplitude)
{ /* this function gets interesting here, */
  /* because pluck may be longer than     */
  /* string length, so we just reset the  */
  /* soundfile and add in the pluck in    */
  /* the tick method.                     */
  soundfile[mic]->reset();
  pluckAmp = amplitude;
  /* Set Pick Position which puts zeroes at pos*length  */
  combDelay->setDelay((MY_FLOAT) 0.5 * pluckPos * lastLength); 
  dampTime = (long) lastLength;      /* See tick method below                */
  waveDone = 0;
}

void Mandolin :: pluck(MY_FLOAT amplitude, MY_FLOAT position)
{ 
  pluckPos = position;     /* pluck position is zeroes at pos*length  */
  this->pluck(amplitude);
}

void Mandolin :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  this->setFreq(freq);
  this->pluck(amp);
#if defined(_debug_)        
	printf("Mandolin : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Mandolin :: setBodySize(MY_FLOAT size)
{
  int i;
  for (i=0;i<12;i++)	{
    soundfile[i]->setRate(size);
  }
}

MY_FLOAT Mandolin :: tick()
{
  MY_FLOAT temp = (MY_FLOAT) 0;
  if (!waveDone)      {
    waveDone = soundfile[mic]->informTick();     /* as long as it goes . . .        */
    temp = soundfile[mic]->lastOut() * pluckAmp; /* scaled pluck excitation         */
    temp = temp - combDelay->tick(temp);         /* with comb filtering             */
  }                                            
  if (dampTime>=0) {                             /* Damping hack to help avoid      */
    dampTime -= 1;                               /* overflow on replucking          */
    lastOutput = delayLine->tick(                /* Calculate 1st delay             */
                filter->tick(                    /* filterered reflection           */
                temp +                           /* plus pluck excitation           */
         (delayLine->lastOut() * (MY_FLOAT) 0.7)));  
    lastOutput += delayLine2->tick(              /* and 2nd delay                   */
                  filter2->tick(                 /* just like the 1st               */
                  temp +                         
        (delayLine2->lastOut() * (MY_FLOAT) 0.7))); /* that's the whole thing!!     */
  }
  else { /*  No damping hack after 1 period */
    lastOutput = delayLine->tick(              /* Calculate 1st delay             */
                 filter->tick(                 /* filtered reflection             */
                 temp +                        /* plus pluck excitation           */
        (delayLine->lastOut() * loopGain)));   
    lastOutput += delayLine2->tick(            /* and 2nd delay                   */
                  filter2->tick(               /* just like the 1st               */
                  temp +                        
                  (delayLine2->lastOut() * loopGain))); 
  }
  lastOutput *= (MY_FLOAT) 0.3 / 32768.0;
  return lastOutput;
}

void Mandolin :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("Mandolin : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_BodySize_)
    this->setBodySize(value * (MY_FLOAT) NORM_7 * (MY_FLOAT) 2.0);
  else if (number == __SK_PickPosition_)
    this->setPluckPos(value * (MY_FLOAT) NORM_7);
  else if (number == __SK_StringDamping_)
    this->setBaseLoopGain((MY_FLOAT) 0.97 + (value * (MY_FLOAT) NORM_7 * (MY_FLOAT) 0.03));
  else if (number == __SK_StringDetune_)
    this->setDetune((MY_FLOAT) 1.0 - (value * (MY_FLOAT) NORM_7 * (MY_FLOAT) 0.1));
  else if (number == __SK_AfterTouch_)
    this->pluck(value * (MY_FLOAT) NORM_7);
  else if (number == 411)	{
    mic = (int) value % 12;
  }
  else    {        
    printf("Mandolin : Undefined Control Number!! %i\n",number);
  }   
}
