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
/*		  CONTROL3 = vibFreq      */
/*		  MOD_WHEEL= vibAmt       */
/******************************************/

#include "Clarinet.h"

Clarinet :: Clarinet(MY_FLOAT lowestFreq)
{
    length = (long) (SRATE / lowestFreq + 1);
    delayLine = new DLineL(length);
    reedTable = new ReedTabl;
    reedTable->setOffset(0.7);
    reedTable->setSlope(-0.3);
    filter = new OneZero;
    envelope = new Envelope;
    noise = new Noise;
    vibr = new RawLoop("rawwaves/sinewave.raw");
    vibr->normalize();
    vibr->setFreq(5.735);
    outputGain = 1.0;
    noiseGain = 0.2;
    vibrGain = 0.1;
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
    filter->tick(0.0);
}

void Clarinet :: setFreq(MY_FLOAT frequency)
{
    delayLine->setDelay         /* length - approx filter delay */
	((SRATE / frequency) * 0.5 - 1.5);
}

void Clarinet :: startBlowing(MY_FLOAT amplitude,MY_FLOAT rate)
{
    envelope->setRate(rate);
    envelope->setTarget(amplitude); 
}

void Clarinet :: stopBlowing(MY_FLOAT rate)
{
    envelope->setRate(rate);
    envelope->setTarget(0.0); 
}

void Clarinet :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    this->setFreq(freq);
    this->startBlowing(0.55 + (amp * 0.30),amp * 0.005);
    outputGain = amp + 0.001;
#if defined(_debug_)        
    printf("Clarinet : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Clarinet :: noteOff(MY_FLOAT amp)
{
    this->stopBlowing(amp * 0.01);
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
    pressureDiff = (pressureDiff * -0.95) - breathPressure;      /* of reflected and mouth */
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
    if (number == MIDI_control1)
	reedTable->setSlope(-0.44 + (0.26 * value * NORM_7));
    else if (number == MIDI_control2)
	noiseGain = (value * NORM_7 * 0.4);
    else if (number == MIDI_control3)
	vibr->setFreq((value * NORM_7 * 12.0));
    else if (number == MIDI_mod_wheel)
	vibrGain = (value * NORM_7 * 0.5);
    else if (number == MIDI_after_touch)	{
        envelope->setValue(value * NORM_7);
    }
    else	{
        printf("Clarinet : Undefined Control Number!!\n");
    }
}
