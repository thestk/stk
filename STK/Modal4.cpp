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
  // We don't make the excitation wave here yet,
  // because we don't know what it's going to be.
  filters[0] = new BiQuad;
  filters[1] = new BiQuad;
  filters[2] = new BiQuad;
  filters[3] = new BiQuad;
  onepole = new OnePole;

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibr = new RawWvIn(strcat(file,"rawwaves/sinewave.raw"),"looping");
  vibr->normalize();
  vibr->setFreq((MY_FLOAT) 6.0);
  vibrGain = (MY_FLOAT) 0.05;
    
  directGain = (MY_FLOAT) 0.0;
  masterGain = (MY_FLOAT) 1.0;
  baseFreq = (MY_FLOAT) 440.0;
  this->setRatioAndReson(0,(MY_FLOAT) 1.00,(MY_FLOAT) 0.9997);    /*  Set some      */
  this->setRatioAndReson(1,(MY_FLOAT) 1.30,(MY_FLOAT) 0.9997);    /*  silly         */
  this->setRatioAndReson(2,(MY_FLOAT) 1.77,(MY_FLOAT) 0.9997);    /*  default       */
  this->setRatioAndReson(3,(MY_FLOAT) 2.37,(MY_FLOAT) 0.9997);    /*  values here   */
  this->setFiltGain(0,(MY_FLOAT) 0.01);
  this->setFiltGain(1,(MY_FLOAT) 0.01);
  this->setFiltGain(2,(MY_FLOAT) 0.01);
  this->setFiltGain(3,(MY_FLOAT) 0.01);
  this->clear();
  filters[0]->setEqualGainZeroes();
  filters[1]->setEqualGainZeroes();
  filters[2]->setEqualGainZeroes();
  filters[3]->setEqualGainZeroes();
  stickHardness = (MY_FLOAT) 0.5;
  strikePosition = (MY_FLOAT) 0.561;
}  

Modal4 :: ~Modal4()
{
    delete envelope; 
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
        while (temp*baseFreq > SRATE_OVER_TWO) temp *= (MY_FLOAT) 0.5;
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
    envelope->setRate((MY_FLOAT) 1.0);
    envelope->setTarget(amplitude);
    onepole->setPole((MY_FLOAT) 1.0 - amplitude);
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
    this->damp((MY_FLOAT) 1.0 - (amp * (MY_FLOAT) 0.03));  /*  (high amplitude means fast damping) */
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
        temp = (MY_FLOAT) 1.0 + (vibr->tick() * vibrGain);     /*  Calculate AM             */
        temp2 = temp * temp2;              		/*  and apply to master out */
    }
    
    lastOutput = temp2 * (MY_FLOAT) 2.0;
    return lastOutput;
}
 
