/*******************************************/
/*  Swept Filter SubClass of Sampling      */
/*  Synthesizer,  by Perry R. Cook, 1995-96*/ 
/*  This instrument inherits up to 5       */
/*  attack waves, 5 looped waves, an ADSR  */
/*  envelope, and adds a 4 pole swept      */
/*  filter.                                */
/*******************************************/

#include "SamplFlt.h"

SamplFlt :: SamplFlt() : Sampler()
{
    MY_FLOAT tempCoeffs[2] = {0.0,-1.0};
    twozeroes[0] = new TwoZero;    
    twozeroes[0]->setZeroCoeffs(tempCoeffs);
    twozeroes[0]->setGain(1.0);
    twozeroes[1] = new TwoZero;    
    twozeroes[1]->setZeroCoeffs(tempCoeffs);
    twozeroes[1]->setGain(1.0);
    filters[0] = new FormSwep;
    filters[0]->setTargets(0.0,0.7,0.5);
    filters[1] = new FormSwep;
    filters[1]->setTargets(0.0,0.7,0.5);
}  

SamplFlt :: ~SamplFlt()
{
    delete filters[0];
    delete filters[1];
    delete twozeroes[0];
    delete twozeroes[1];
}

MY_FLOAT SamplFlt :: tick()
{
    MY_FLOAT output;
    output = Sampler :: tick();
    output = twozeroes[0]->tick(output);
    output = filters[0]->tick(output);
    output = twozeroes[1]->tick(output);
    output = filters[1]->tick(output);
    return output;
}

void SamplFlt :: controlChange(int number, MY_FLOAT value)
{
}
