/******************************************/
/*  Hammond(OID) Organ Subclass           */
/*  of Algorithm 8 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "BeeThree.h"

BeeThree :: BeeThree() : FM4Alg8()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw");
    this->setRatio(0,0.999);
    this->setRatio(1,1.997);
    this->setRatio(2,3.006);
    this->setRatio(3,6.009);
    gains[0] = __FM4Op_gains[95];
    gains[1] = __FM4Op_gains[95];
    gains[2] = __FM4Op_gains[99];
    gains[3] = __FM4Op_gains[95];
    adsr[0]->setAll(0.05,0.03,1.0,0.04);
    adsr[1]->setAll(0.05,0.03,1.0,0.04);
    adsr[2]->setAll(0.05,0.03,1.0,0.04);
    adsr[3]->setAll(0.05,0.001,0.4,0.06);
    twozero->setGain(0.1);
} 

BeeThree :: ~BeeThree()
{

}

void BeeThree :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(baseFreq * ratios[2]);
    waves[3]->setFreq(baseFreq * ratios[3]);
}

MY_FLOAT BeeThree :: tick()
{
    MY_FLOAT temp;
    if (modDepth > 0.0)	{
        temp = 1.0 + (modDepth * vibWave->tick() * 0.1);
        waves[0]->setFreq(baseFreq * ratios[0] * temp);
        waves[1]->setFreq(baseFreq * ratios[1] * temp);
        waves[2]->setFreq(baseFreq * ratios[2] * temp);
        waves[3]->setFreq(baseFreq * ratios[3] * temp);
    }
    lastOutput = FM4Alg8 :: tick();
    return lastOutput;
}

void BeeThree :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[95];
    gains[1] = amp * __FM4Op_gains[95];
    gains[2] = amp * __FM4Op_gains[99];
    gains[3] = amp * __FM4Op_gains[95];
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("BeeThree : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

