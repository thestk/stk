/******************************************/
/*  Algorithm 6 (TX81Z) Subclass of       */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/*  This connection topology is three     */
/*  Carriers and a common Modulator       */
/*                                        */
/*                /->1 -\                 */
/*             4-|-->2 - +-> Out          */
/*                \->3 -/                 */
/*                                        */
/*  Controls: control1 = vowel      	  */
/*            control2 = spectral tilt    */
/*            control3 = LFO speed        */
/*            modWheel = LFO amount       */
/*                                        */
/******************************************/

#include "FM4Alg6.h"

FM4Alg6 :: FM4Alg6() : FM4Op()
{
    /*  We still don't make the waves here yet, because    */
    /*  we still don't know what they will be.             */
}  

FM4Alg6 :: ~FM4Alg6()
{
}

MY_FLOAT FM4Alg6 :: tick()
{
    MY_FLOAT temp,temp2;

    temp = gains[3] * adsr[3]->tick() * waves[3]->tick();
    temp2 = vibWave->tick() * modDepth * (MY_FLOAT) 0.1;         /*  Calculate frequency mod  */

    waves[0]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp2) * ratios[0]);
    waves[1]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp2) * ratios[1]);
    waves[2]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp2) * ratios[2]);
    waves[3]->setFreq(baseFreq * ((MY_FLOAT) 1.0 + temp2) * ratios[3]);

    waves[0]->addPhaseOffset(temp * mods[0]);
    waves[1]->addPhaseOffset(temp * mods[1]);
    waves[2]->addPhaseOffset(temp * mods[2]);
    waves[3]->addPhaseOffset(twozero->lastOut());
    twozero->tick(temp);
    temp =  gains[0] * tilt[0] * adsr[0]->tick() * waves[0]->tick();
    temp += gains[1] * tilt[1] * adsr[1]->tick() * waves[1]->tick();
    temp += gains[2] * tilt[2] * adsr[2]->tick() * waves[2]->tick();
    
    return temp * (MY_FLOAT) 0.33;
}

void FM4Alg6 :: controlChange(int number, MY_FLOAT value)
{

}
