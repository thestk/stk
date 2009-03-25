/******************************************/
/*  Fender Rhodes Electric Piano Subclass */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "Rhodey.h"

Rhodey :: Rhodey() : FM4Alg5()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/fwavblnk.raw");
    this->setRatio(0,1.0);
    this->setRatio(1,0.5);
    this->setRatio(2,1.0);
    this->setRatio(3,15.0);
    gains[0] = __FM4Op_gains[99];
    gains[1] = __FM4Op_gains[90];
    gains[2] = __FM4Op_gains[99];
    gains[3] = __FM4Op_gains[67];
    adsr[0]->setAll(0.05,0.00003,0.0,0.02);
    adsr[1]->setAll(0.05,0.00003,0.0,0.02);
    adsr[2]->setAll(0.05,0.00005,0.0,0.02);
    adsr[3]->setAll(0.05,0.0002,0.0,0.02);
    twozero->setGain(1.0);
}  

Rhodey :: ~Rhodey()
{

}

void Rhodey :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency * 2.0;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(baseFreq * ratios[2]);
    waves[3]->setFreq(baseFreq * ratios[3]);
}

void Rhodey :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[99];
    gains[1] = amp * __FM4Op_gains[90];
    gains[2] = amp * __FM4Op_gains[99];
    gains[3] = amp * __FM4Op_gains[67];
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("Rhodey : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

