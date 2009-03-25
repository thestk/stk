/******************************************/
/*  WaveGuide Flute ala Karjalainen,      */
/*  Smith, Waryznyk, etc.                 */
/*  with polynomial Jet ala Cook          */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = jetDelay     */
/*                CONTROL2 = noiseGain    */
/*		  CONTROL3 = vibFreq      */
/*		  MOD_WHEEL= vibAmt       */
/******************************************/

#include "Flute.h"

Flute :: Flute(MY_FLOAT lowestFreq)
{
    long length;
    length = (long) (SRATE / lowestFreq + 1);
    boreDelay = new DLineL(length);
    length >> 1;
    jetDelay = new DLineL(length);
    jetTable = new JetTabl;
    filter = new OnePole;
    dcBlock = new DCBlock;
    noise = new Noise;
    adsr = new ADSR;
    vibr = new RawLoop("rawwaves/sinewave.raw");
    this->clear();

    boreDelay->setDelay(100.0);
    jetDelay->setDelay(49.0);

    filter->setPole(0.7 - (0.1 * RATE_NORM));
    filter->setGain(-1.0);
    vibr->normalize();
    vibr->setFreq(5.925);
    adsr->setAll(0.02, 0.05, 0.8, 0.001);
    endRefl = 0.5;
    jetRefl = 0.5;
    noiseGain = 0.15;          /* Breath pressure random component   */
    vibrGain = 0.05;            /* breath periodic vibrato component  */
    jetRatio = 0.32;
}

Flute :: ~Flute()
{
    delete jetDelay;
    delete boreDelay;
    delete jetTable;
    delete filter;
    delete dcBlock;
    delete noise;
    delete adsr;
    delete vibr;
}

void Flute :: clear()
{
    jetDelay->clear();
    boreDelay->clear();
    filter->clear();
    dcBlock->clear();
/*    adsr->reset();      */
}

void Flute :: setFreq(MY_FLOAT frequency)
{
    MY_FLOAT temp;
    lastFreq = frequency * 0.66666;	/* we're overblowing here */
    temp = SRATE / lastFreq - 2.0;      /* Length - approx. filter delay */
    boreDelay->setDelay(temp);          /* Length of bore tube           */
    jetDelay->setDelay(temp * jetRatio);   /* jet delay shorter             */
}

void Flute :: startBlowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
    adsr->setAttackRate(rate);
    maxPressure = amplitude / 0.8;
    adsr->keyOn();
}

void Flute :: stopBlowing(MY_FLOAT rate)
{
    adsr->setReleaseRate(rate);
    adsr->keyOff();
}

void Flute :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    this->setFreq(freq);
    this->startBlowing(1.1 + (amp * 0.20),amp * 0.02);
    outputGain = amp + 0.001;
#if defined(_debug_)        
    printf("Flute : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Flute :: noteOff(MY_FLOAT amp)
{
    this->stopBlowing(amp * 0.02);
#if defined(_debug_)        
    printf("Flute : NoteOff: Amp=%lf\n",amp);
#endif    
}

void Flute :: setJetRefl(MY_FLOAT refl)
{
   jetRefl = refl;
}

void Flute :: setEndRefl(MY_FLOAT refl)
{         
   endRefl = refl;
}               

void Flute :: setJetDelay(MY_FLOAT aRatio)
{
    MY_FLOAT temp;
    temp = SRATE / lastFreq - 2.0;       /* Length - approx. filter delay */
    jetRatio = aRatio;
    jetDelay->setDelay(temp * aRatio);   /* Scaled by ratio               */
}

MY_FLOAT Flute :: tick()
{
    MY_FLOAT temp;
    MY_FLOAT pressureDiff;
    MY_FLOAT randPressure;
    MY_FLOAT breathPressure;

    breathPressure = maxPressure * adsr->tick();   /* Breath Pressure               */
    randPressure = noiseGain * noise->tick();      /* Random Deviation              */
    randPressure += vibrGain * vibr->tick();       /* + breath vibrato              */
    randPressure *= breathPressure;                /* All scaled by Breath Pressure */

    temp = filter->tick(boreDelay->lastOut());     
    temp = dcBlock->tick(temp);                    /* Block DC on reflection        */
    pressureDiff = breathPressure + randPressure - /* Breath Pressure               */
			     (jetRefl * temp);     /*    - reflected                */
    pressureDiff = jetDelay->tick(pressureDiff);   /* Jet Delay Line                */
    pressureDiff = jetTable->lookup(pressureDiff)  /* Non-Lin Jet + reflected       */
			+ (endRefl * temp);        
    lastOutput = 0.3 * boreDelay->tick(pressureDiff);  /* Bore Delay and "bell" filter  */

    lastOutput *= outputGain;
    return lastOutput;

}

void Flute :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Flute : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
	this->setJetDelay(0.08 + (0.48 * value * NORM_7));
    else if (number == MIDI_control2)
	noiseGain = (value * NORM_7 * 0.4);
    else if (number == MIDI_control3)
	vibr->setFreq((value * NORM_7 * 12.0));
    else if (number == MIDI_mod_wheel)
	vibrGain = (value * NORM_7 * 0.4);
    else if (number == MIDI_after_touch)
        adsr->setTarget(value * NORM_7);
    else    {        
        printf("Flute : Undefined Control Number!!\n");
    }   
}
