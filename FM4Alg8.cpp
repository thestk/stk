/******************************************/
/*  Algorithm 8 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is simple    */
/*  Additive Synthesis, like:             */
/*                                        */
/*                   1 --.                */
/*                   2 -\|                */
/*                       +-> Out          */
/*                   3 -/|                */
/*                   4 --                 */
/*                                        */
/*  Controls: control1 = op4 (fb) gain    */
/*            control2 = op3 gain         */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#include "FM4Alg8.h"

FM4Alg8 :: FM4Alg8() : FM4Op()
{
    /*  We still don't make the waves here yet, because    */
    /*  we still don't know what they will be.             */
}  

FM4Alg8 :: ~FM4Alg8()
{
}

MY_FLOAT FM4Alg8 :: tick()
{
    MY_FLOAT temp;

    waves[3]->addPhaseOffset(twozero->lastOut());
    temp = control1 * (MY_FLOAT) 2.0 * gains[3] * adsr[3]->tick() * waves[3]->tick();
    twozero->tick(temp);
    temp += control2 * (MY_FLOAT) 2.0 * gains[2] * adsr[2]->tick() * waves[2]->tick();
    temp += gains[1] * adsr[1]->tick() * waves[1]->tick();
    temp += gains[0] * adsr[0]->tick() * waves[0]->tick();
    
    lastOutput = temp * (MY_FLOAT) 0.125;
    return lastOutput;
}

