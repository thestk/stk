/******************************************/
/*  Algorithm 4 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*                                        */
/*  Alg 4 is :      4->3--\               */
/*                     2-- + -->1-->Out   */
/*                                        */
/*  Controls: control1 = total mod index  */
/*            control2 = crossfade of two */
/*                          modulators    */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#include "FM4Alg4.h"

FM4Alg4 :: FM4Alg4() : FM4Op()
{
    /*  We still don't make the waves here yet, because     */
    /*  we still don't know what they will be.              */
}  

FM4Alg4 :: ~FM4Alg4()
{
}

MY_FLOAT FM4Alg4 :: tick()
{
    MY_FLOAT temp;

    temp = vibWave->tick() * modDepth * (MY_FLOAT) 0.2;    
    waves[0]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp) * ratios[0]);
    waves[1]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp) * ratios[1]);
    waves[2]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp) * ratios[2]);
    waves[3]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp) * ratios[3]);
    
    waves[3]->addPhaseOffset(twozero->lastOut());
    temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
    twozero->tick(temp);
    waves[2]->addPhaseOffset(temp);
    temp = ((MY_FLOAT) 1.0 - (control2 * (MY_FLOAT) 0.5)) * 
				gains[2] * adsr[2]->tick() * waves[2]->tick();
    temp += control2 * (MY_FLOAT) 0.5 * gains[1] * adsr[1]->tick() * waves[1]->tick();
    temp = temp * control1;
    waves[0]->addPhaseOffset(temp);
    temp = gains[0] * adsr[0]->tick() * waves[0]->tick();
    
    lastOutput = temp * (MY_FLOAT) 0.5;
    return lastOutput;
}

