/******************************************/
/*  Tubular Bell (Orch. Chime) Subclass   */
/*  of Algorithm 5 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "TubeBell.h"

TubeBell :: TubeBell() : FM4Alg5()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw");
    
    this->setRatio(0,(MY_FLOAT) (1.0   * 0.995));
    this->setRatio(1,(MY_FLOAT) (1.414 * 0.995));
    this->setRatio(2,(MY_FLOAT) (1.0   * 1.005));
    this->setRatio(3,(MY_FLOAT) (1.414 * 1.000));
    gains[0] = __FM4Op_gains[94];
    gains[1] = __FM4Op_gains[76];
    gains[2] = __FM4Op_gains[99];
    gains[3] = __FM4Op_gains[71];
    adsr[0]->setAllTimes((MY_FLOAT) 0.005,(MY_FLOAT) 4.0,(MY_FLOAT) 0.0,(MY_FLOAT) 0.04);
    adsr[1]->setAllTimes((MY_FLOAT) 0.005,(MY_FLOAT) 4.0,(MY_FLOAT) 0.0,(MY_FLOAT) 0.04);
    adsr[2]->setAllTimes((MY_FLOAT) 0.001,(MY_FLOAT) 2.0,(MY_FLOAT) 0.0,(MY_FLOAT) 0.04);
    adsr[3]->setAllTimes((MY_FLOAT) 0.004,(MY_FLOAT) 4.0,(MY_FLOAT) 0.0,(MY_FLOAT) 0.04);
    twozero->setGain((MY_FLOAT) 0.5);
    vibWave->setFreq((MY_FLOAT) 2.0);
}  

void TubeBell :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(baseFreq * ratios[2]);
    waves[3]->setFreq(baseFreq * ratios[3]);
}

void TubeBell :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[94];
    gains[1] = amp * __FM4Op_gains[76];
    gains[2] = amp * __FM4Op_gains[99];
    gains[3] = amp * __FM4Op_gains[71];
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("TubeBell : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}
