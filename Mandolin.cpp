/********************************************/  
/*  Commuted Mandolin Subclass of enhanced  */
/*  dual plucked-string model               */
/*  by Perry Cook, 1995-96                  */
/*   Controls:    CONTROL1 = bodySize       */
/*                CONTROL2 = pluckPosition  */
/*		  CONTROL3 = loopGain       */
/*		  MOD_WHEEL= deTuning       */
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

Mandolin :: Mandolin(MY_FLOAT lowestFreq) : Plucked2(lowestFreq)
{
    soundfile = new RawWave("rawwaves/mandpluk.raw");
    soundfile->normalize(0.05);       /*  Empirical hack here  */
    soundfile->setLooping(0);
    dampTime = 0;
    waveDone = 1;
}

void Mandolin :: pluck(MY_FLOAT amplitude)
{                               /* this function gets interesting here, */
    soundfile->reset();         /* because pluck may be longer than     */
    pluckAmp = amplitude;       /* string length, so we just reset the  */
				/* soundfile and add in the pluck in    */
				/* the tick method.                     */
    combDelay->setDelay(
	0.5 * pluckPos * lastLength);  /* Set Pick Position                    */
				       /*   which puts zeroes at pos*length    */
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
    soundfile->setRate(size);
}

MY_FLOAT Mandolin :: tick()
{
    MY_FLOAT temp = 0;
    if (!waveDone)      {
	waveDone = soundfile->informTick();        /* as long as it goes . . .        */
	temp = soundfile->lastOut() * pluckAmp;    /* scaled pluck excitation         */
	temp = temp - combDelay->tick(temp);       /* with comb filtering             */
    }                                            
    if (dampTime>=0) {                             /* Damping hack to help avoid      */
	dampTime -= 1;                             /*   overflow on replucking        */
	lastOutput = delayLine->tick(              /* Calculate 1st delay             */
		  filter->tick(                    /*  filterered reflection          */
		   temp +                          /*  plus pluck excitation          */
		   (delayLine->lastOut() * 0.7)));  
	lastOutput += delayLine2->tick(            /* and 2nd delay                   */
		  filter2->tick(                   /*  just like the 1st              */
		     temp +                         
		     (delayLine2->lastOut() * 0.7))); /* that's the whole thing!!     */
    }
    else {                                         /*  No damping hack after 1 period */
	lastOutput = delayLine->tick(              /* Calculate 1st delay             */
		  filter->tick(                    /*  filtered reflection            */
		   temp +                          /*  plus pluck excitation          */
		   (delayLine->lastOut() 
			* loopGain)));   
	lastOutput += delayLine2->tick(            /* and 2nd delay                   */
		  filter2->tick(                   /*  just like the 1st              */
		   temp +                        
		   (delayLine2->lastOut() 
		   * loopGain))); 
    }
    lastOutput *= 2.0;
    return lastOutput;
}

void Mandolin :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Mandolin : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
	this->setBodySize(value * NORM_7 * 2.0);
    else if (number == MIDI_control2)
	this->setPluckPos(value * NORM_7);
    else if (number == MIDI_control3)
	this->setBaseLoopGain(0.97 + (value * NORM_7 * 0.03));
    else if (number == MIDI_mod_wheel)
	this->setDetune(1.0 - (value * NORM_7 * 0.1));
    else if (number == MIDI_after_touch)
        this->pluck(value * NORM_7);
    else    {        
        printf("Mandolin : Undefined Control Number!!\n");
    }   
}
