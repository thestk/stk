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

Brass :: Brass(MY_FLOAT lowestFreq)
{
    length = (long) (SRATE / lowestFreq + 1);
    delayLine = new DLineA(length);
    lipFilter = new LipFilt;
    dcBlock = new DCBlock;
    adsr = new ADSR;
    adsr->setAll(0.02, 0.05, 1.0, 0.001);
    vibr = new RawLoop("rawwaves/sinewave.raw");
    this->clear();

    vibr->normalize();
    vibr->setFreq(6.137);
    vibrGain = 0.05;            /* breath periodic vibrato component  */
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
/*    adsr->reset();     */
}

void Brass :: setFreq(MY_FLOAT frequency)
{
    slideTarget = ((MY_FLOAT) SRATE / frequency * 2.0) + 3.0;
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
    this->startBlowing(amp, amp * 0.001);
#if defined(_debug_)        
    printf("Brass : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Brass :: noteOff(MY_FLOAT amp)
{
    this->stopBlowing(amp * 0.005);
#if defined(_debug_)        
    printf("Brass : NoteOff: Amp=%lf\n",amp);
#endif    
}

MY_FLOAT Brass :: tick()
{
    MY_FLOAT breathPressure;

    breathPressure = maxPressure * adsr->tick();
    breathPressure += vibrGain * vibr->tick();
    lastOutput = delayLine->tick(                       /* bore delay  */
		dcBlock->tick(                          /* block DC    */
		  lipFilter->tick(0.3 * breathPressure, /* mouth input */
			0.85 * delayLine->lastOut()))); /* and bore reflection */
    return lastOutput;
}

void Brass :: controlChange(int number, MY_FLOAT value)
{
    MY_FLOAT temp;
#if defined(_debug_)        
    printf("Brass : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)	{
        temp = lipTarget * pow(4.0,(2.0*value*NORM_7) - 1.0);
	this->setLip(temp);
    }
    else if (number == MIDI_control2)
        delayLine->setDelay(slideTarget * (0.5 + (value * NORM_7)));  
    else if (number == MIDI_control3)
	vibr->setFreq((value * NORM_7 * 12.0));
    else if (number == MIDI_mod_wheel)
	vibrGain = (value * NORM_7 * 0.4);
    else if (number == MIDI_after_touch)
        adsr->setTarget(value * NORM_7);
    else    {        
        printf("Brass : Undefined Control Number!!\n");
    }  
}
