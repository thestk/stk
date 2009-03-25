/******************************************/
/*  Bowed String model ala Smith          */
/*  after McIntyre, Schumacher, Woodhouse */
/*  by Perry Cook, 1995-96                */
/*                                        */
/*  This is a waveguide model, and thus   */
/*  relates to various Stanford Univ.     */
/*  and possibly Yamaha and other patents.*/
/*                                        */
/*   Controls:    CONTROL1 = bowPressure  */
/*                CONTROL2 = bowPosition  */
/*		  CONTROL3 = vibrFreq     */
/*		  MOD_WHEEL= vibrGain     */
/*					  */
/******************************************/

#include "Bowed.h"

Bowed :: Bowed(MY_FLOAT lowestFreq)
{
    long length;
    length = (long) (SRATE / lowestFreq + 1);
    neckDelay = new DLineL(length);
    length >> 1;
    bridgeDelay = new DLineL(length);
    bowTabl = new BowTabl;
    reflFilt = new OnePole;
    bodyFilt = new BiQuad;
    vibr = new RawLoop("rawwaves/sinewave.raw");
    adsr = new ADSR;
    vibrGain = 0.0;

    neckDelay->setDelay(100.0);
    bridgeDelay->setDelay(29.0);

    bowTabl->setSlope(3.0);

    reflFilt->setPole(0.6 - (0.1 * RATE_NORM));
    reflFilt->setGain(0.95);

    bodyFilt->setFreqAndReson(500.0, 0.85);
    bodyFilt->setEqualGainZeroes();
    bodyFilt->setGain(0.2);

    vibr->normalize();
    vibr->setFreq(6.12723);

    adsr->setAll(0.002,0.01,0.9,0.01);
    
    betaRatio = 0.127236;
}

Bowed :: ~Bowed()
{
    delete neckDelay;
    delete bridgeDelay;
    delete bowTabl;
    delete reflFilt;
    delete bodyFilt;
    delete vibr;
    delete adsr;
}

void Bowed :: clear()
{
    neckDelay->clear();
    bridgeDelay->clear();
}

void Bowed :: setFreq(MY_FLOAT frequency)
{
    baseDelay = (MY_FLOAT) SRATE / frequency - 4.0;   	/* delay - approx. filter delay */
    bridgeDelay->setDelay(baseDelay * betaRatio); 	/* bow to bridge length   */
    neckDelay->setDelay(baseDelay * (1.0 - betaRatio)); /* bow to nut (finger) length   */
}

void Bowed :: startBowing(MY_FLOAT amplitude, MY_FLOAT rate)
{
    adsr->setRate(rate);
    adsr->keyOn();
    maxVelocity = 0.03 + (0.2 * amplitude); 
}

void Bowed :: stopBowing(MY_FLOAT rate)
{
    adsr->setRate(rate);
    adsr->keyOff();
}

void Bowed :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    this->startBowing(amp,amp * 0.001);
    this->setFreq(freq);
#if defined(_debug_)        
    printf("Bowed : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Bowed :: noteOff(MY_FLOAT amp)
{
    this->stopBowing((1.0 - amp) * 0.005);
#if defined(_debug_)        
    printf("Bowed : NoteOff: Amp=%lf\n",amp);
#endif    
}

void Bowed :: setVibrato(MY_FLOAT amount)
{
    vibrGain = amount;
}

MY_FLOAT Bowed :: tick()
{
    MY_FLOAT bowVelocity;
    MY_FLOAT bridgeRefl=0,nutRefl=0;
    MY_FLOAT newVel=0,velDiff=0,stringVel=0;
    
    bowVelocity = maxVelocity * adsr->tick();

    bridgeRefl = -reflFilt->tick(
                bridgeDelay->lastOut());          /* Bridge Reflection      */
    nutRefl = -neckDelay->lastOut();              /* Nut Reflection         */
    stringVel = bridgeRefl + nutRefl;             /* Sum is String Velocity */
    velDiff = bowVelocity - stringVel;            /* Differential Velocity  */
    newVel = velDiff * bowTabl->lookup(velDiff);  /* Non-Lin Bow Function   */
    neckDelay->tick(bridgeRefl + newVel);         /* Do string              */     
    bridgeDelay->tick(nutRefl + newVel);          /*   propagations         */
    
    if (vibrGain > 0.0)  {
        neckDelay->setDelay((baseDelay * (1.0 - betaRatio)) + 
                 (baseDelay * vibrGain*vibr->tick()));
    }

    lastOutput = bodyFilt->tick(bridgeDelay->lastOut());                 

    return lastOutput;
}

void Bowed :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Bowed : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
	bowTabl->setSlope(5.0 - (4.0 * value * NORM_7));
    else if (number == MIDI_control2)	{
	betaRatio = 0.027236 + (0.2 * value * NORM_7);
        bridgeDelay->setDelay(baseDelay * betaRatio); 	/* bow to bridge length   */
        neckDelay->setDelay(baseDelay * (1.0 - betaRatio)); /* bow to nut (finger) length   */
    }
    else if (number == MIDI_control3)
	vibr->setFreq((value * NORM_7 * 12.0));
    else if (number == MIDI_mod_wheel)
	vibrGain = (value * NORM_7 * 0.4);
    else if (number == MIDI_after_touch)
	adsr->setTarget(value * NORM_7);
    else    {        
        printf("Bowed : Undefined Control Number!!\n");
    }    
}
