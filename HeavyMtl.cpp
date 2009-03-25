/******************************************/
/*  Heavy Metal Synth Subclass            */
/*  of Algorithm 3 (TX81Z) Subclass of    */ 
/*  3 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "HeavyMtl.h"

HeavyMtl :: HeavyMtl() : FM4Alg3()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/twopeaks.raw",
                    "rawwaves/twopeaks.raw",
                    "rawwaves/sinewave.raw");
    
    this->setRatio(0,1.00     );
    this->setRatio(1,4.00 * 0.999);
    this->setRatio(2,3.00 * 1.001);
    this->setRatio(3,0.50 * 1.002);
    gains[0] = __FM4Op_gains[92];
    gains[1] = __FM4Op_gains[76];
    gains[2] = __FM4Op_gains[91];
    gains[3] = __FM4Op_gains[68];
    adsr[0]->setAll(0.050,0.0100,1.0,0.001);
    adsr[1]->setAll(0.050,0.0010,1.0,0.0001);
    adsr[2]->setAll(0.001,0.0020,1.0,0.0002);
    adsr[3]->setAll(0.050,0.0010,0.2,0.0002);
    twozero->setGain(2.0);
    vibWave->setFreq(5.5);
    modDepth = 0.00;
}  

HeavyMtl :: ~HeavyMtl()
{

}

void HeavyMtl :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(baseFreq * ratios[2]);
    waves[3]->setFreq(baseFreq * ratios[3]);
}

void HeavyMtl :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[92];
    gains[1] = amp * __FM4Op_gains[76];
    gains[2] = amp * __FM4Op_gains[91];
    gains[3] = amp * __FM4Op_gains[68];
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("HeavyMtl : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

