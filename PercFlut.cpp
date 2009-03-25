/******************************************/
/*  Percussive Flute Subclass             */
/*  of Algorithm 4 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "PercFlut.h"

PercFlut :: PercFlut() : FM4Alg4()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw");
    
    this->setRatio(0,1.50     );
    this->setRatio(1,3.00 * 0.995);
    this->setRatio(2,2.99 * 1.005);
    this->setRatio(3,6.00 * 0.997);
    gains[0] = __FM4Op_gains[99];
    gains[1] = __FM4Op_gains[71];
    gains[2] = __FM4Op_gains[93];
    gains[3] = __FM4Op_gains[85];
    adsr[0]->setAll(0.001,0.001,__FM4Op_susLevels[14],0.001);
    adsr[1]->setAll(0.05,0.0001,__FM4Op_susLevels[13],0.0001);
    adsr[2]->setAll(0.05,0.0020,__FM4Op_susLevels[11],0.001);
    adsr[3]->setAll(0.05,0.0010,__FM4Op_susLevels[13],0.005);
    twozero->setGain(0.0);
    modDepth = 0.005;
}  

void PercFlut :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
}

void PercFlut :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[99];
    gains[1] = amp * __FM4Op_gains[71];
    gains[2] = amp * __FM4Op_gains[93];
    gains[3] = amp * __FM4Op_gains[85];
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("PercFlut : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

