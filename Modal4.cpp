/*******************************************/
/*  4 Resonance Modal Synthesis Instrument */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains an excitation */
/*  wavetable, an envelope, and four reso- */
/*  nances (Non-Sweeping BiQuad Filters).  */
/*******************************************/

#include "Modal4.h"

Modal4 :: Modal4()
{
    envelope = new Envelope;
        /*  We don't make the excitation wave here yet,   */
        /*  because we don't know what it's going to be.  */
    filters[0] = new BiQuad;
    filters[1] = new BiQuad;
    filters[2] = new BiQuad;
    filters[3] = new BiQuad;
    onepole = new OnePole;

    vibr = new RawLoop("rawwaves/sinewave.raw");
    vibr->normalize();
    vibr->setFreq(6.0);
    vibrGain = 0.05;
    
    directGain = 0.0;
    masterGain = 1.0;
    baseFreq = 440.0;
    this->setRatioAndReson(0,1.00,0.9997);    /*  Set some      */
    this->setRatioAndReson(1,1.30,0.9997);    /*  silly         */
    this->setRatioAndReson(2,1.77,0.9997);    /*  default       */
    this->setRatioAndReson(3,2.37,0.9997);    /*  values here   */
    this->setFiltGain(0,0.01);
    this->setFiltGain(1,0.01);
    this->setFiltGain(2,0.01);
    this->setFiltGain(3,0.01);
    this->clear();
    filters[0]->setEqualGainZeroes();
    filters[1]->setEqualGainZeroes();
    filters[2]->setEqualGainZeroes();
    filters[3]->setEqualGainZeroes();
    stickHardness = 0.5;
    strikePosition = 0.561;
}  

Modal4 :: ~Modal4()
{
    delete envelope; 
    delete wave;
    delete filters[0];
    delete filters[1];
    delete filters[2];
    delete filters[3];
    delete onepole;
    delete vibr;
}

void Modal4 :: clear()
{    
    onepole->clear();
    filters[0]->clear();
    filters[1]->clear();
    filters[2]->clear();
    filters[3]->clear();
}

void Modal4 :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    this->setRatioAndReson(0,ratios[0],resons[0]);
    this->setRatioAndReson(1,ratios[1],resons[1]);
    this->setRatioAndReson(2,ratios[2],resons[2]);
    this->setRatioAndReson(3,ratios[3],resons[3]);
}

#include <stdio.h>

void Modal4 :: setRatioAndReson(int whichOne, MY_FLOAT ratio,MY_FLOAT reson)
{
    MY_FLOAT temp;
    if (ratio*baseFreq < SRATE_OVER_TWO) {
        ratios[whichOne] = ratio;
    }
    else {
        temp = ratio;
        while (temp*baseFreq > SRATE_OVER_TWO) temp *= 0.5;
        ratios[whichOne] = temp;
#if defined(_debug_)        
    printf("Modal4 : Aliasing would occur here, correcting.\n");
#endif    
    }
    resons[whichOne] = reson;
    if (ratio<0) 
        temp = -ratio;
    else
        temp = ratio*baseFreq;
    filters[whichOne]->setFreqAndReson(temp,reson);
}

void Modal4 :: setMasterGain(MY_FLOAT aGain)
{
    masterGain = aGain;
}

void Modal4 :: setDirectGain(MY_FLOAT aGain)
{
    directGain = aGain;
}

void Modal4 :: setFiltGain(int whichOne, MY_FLOAT gain)
{
    filters[whichOne]->setGain(gain);
}

void Modal4 :: strike(MY_FLOAT amplitude)
{
    int i;
    MY_FLOAT temp;
    envelope->setRate(1.0);
    envelope->setTarget(amplitude);
    onepole->setPole(1.0 - amplitude);
    envelope->tick();
    wave->reset();
    for (i=0;i<4;i++)   {
        if (ratios[i] < 0)
            temp = -ratios[i];
        else
            temp = ratios[i] * baseFreq;
        filters[i]->setFreqAndReson(temp,resons[i]);
    }
}

void Modal4 :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    this->strike(amp);
    this->setFreq(freq);
#if defined(_debug_)        
    printf("Modal4 : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Modal4 :: noteOff(MY_FLOAT amp) /*  This calls damp, but inverts the    */
{                                    /*  meaning of amplitude.               */
    this->damp(1.0 - (amp * 0.03));  /*  (high amplitude means fast damping) */
#if defined(_debug_)        
    printf("Modal4 : NoteOff: Amp=%lf\n",amp);
#endif    
}

void Modal4 :: damp(MY_FLOAT amplitude)
{
    int i;
    MY_FLOAT temp;
    for (i=0;i<4;i++)   {
        if (ratios[i] < 0)
            temp = -ratios[i];
        else
            temp = ratios[i] * baseFreq;
        filters[i]->setFreqAndReson(temp,resons[i]*amplitude);
    }
}

void Modal4 :: controlChange(int number, MY_FLOAT value)
{
}

MY_FLOAT Modal4 :: tick()
{
    MY_FLOAT temp,temp2;
    temp   = masterGain * onepole->tick(wave->tick() * envelope->tick());
    temp2  = filters[0]->tick(temp);
    temp2 += filters[1]->tick(temp);
    temp2 += filters[2]->tick(temp);
    temp2 += filters[3]->tick(temp);
    temp2  = temp2 - (temp2 * directGain);
    temp2 += directGain * temp;

    if (vibrGain != 0.0)	{
        temp = 1.0 + (vibr->tick() * vibrGain);     /*  Calculate AM             */
        temp2 = temp * temp2;              		/*  and apply to master out */
    }
    
    lastOutput = temp2 * 2.0;
    return lastOutput;
}
 
