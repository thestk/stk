/******************************************/
/*  Algorithm 5 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is 2 simple  */
/*  FM Pairs summed together, like:       */
/*                                        */
/*   Alg 5 is :      4->3--\              */
/*                          + --> Out     */
/*                   2->1--/              */
/*                                        */
/*  Controls: control1 = mod index 1      */
/*            control2 = crossfade of two */
/*                          outputs       */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#include "FM4Alg5.h"

FM4Alg5 :: FM4Alg5() : FM4Op()
{
    /*  We still don't make the waves here yet, because    */
    /*  we still don't know what they will be.             */
}  

MY_FLOAT FM4Alg5 :: tick()
{
    MY_FLOAT temp,temp2;

    temp = gains[1] * adsr[1]->tick() * waves[1]->tick();
    temp = temp * control1;
    waves[0]->addPhaseOffset(temp);
    waves[3]->addPhaseOffset(twozero->lastOut());
    temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
    twozero->tick(temp);
    waves[2]->addPhaseOffset(temp);
    temp = (1.0 - (control2 * 0.5)) * gains[0] * adsr[0]->tick() * waves[0]->tick();
    temp += control2 * 0.5 * gains[2] * adsr[2]->tick() * waves[2]->tick();
    
    temp2 = vibWave->tick() * modDepth;         /*  Calculate amplitude mod  */
    temp = temp * (1.0 + temp2);                /*  and apply it to output   */
    
    lastOutput = temp * 0.5;
    return lastOutput;
}

