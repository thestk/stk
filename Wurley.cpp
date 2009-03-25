/******************************************/
/*  Wurlitzer Electric Piano Subclass     */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "Wurley.h"

Wurley :: Wurley() : FM4Alg5()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/fwavblnk.raw");
    
    this->setRatio(0,1.0);
    this->setRatio(1,4.0);
    this->setRatio(2,-510.0);
    this->setRatio(3,-510.0);
    gains[0] = __FM4Op_gains[99];
    gains[1] = __FM4Op_gains[82];
    gains[2] = __FM4Op_gains[92];
    gains[3] = __FM4Op_gains[68];   /* Originally 78, but sounded stinky */
    twozero->setGain(2.0);
    adsr[0]->setAll(0.05,0.00003,0.0,0.02);
    adsr[1]->setAll(0.05,0.00003,0.0,0.02);
    adsr[2]->setAll(0.05,0.0002,0.0,0.02);
    adsr[3]->setAll(0.05,0.0003,0.0,0.02);
    vibWave->setFreq(8.0);
}  

void Wurley :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(ratios[2]);	/* Note here a 'fixed resonance' */
    waves[3]->setFreq(ratios[3]);
}

void Wurley :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[99];
    gains[1] = amp * __FM4Op_gains[82];
    gains[2] = amp * __FM4Op_gains[82];      /*   Originally 92 */
    gains[3] = amp * __FM4Op_gains[68];      /*   Originally 78 */
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("Wurley : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}
