/******************************************/
/*  Instrument SuperClass for Toolkit96   */
/*  Perry R. Cook, Princeton University   */
/******************************************/

#include "Instrmnt.h"

Instrmnt :: Instrmnt()
{
}

Instrmnt :: ~Instrmnt()
{
}

void Instrmnt :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    printf("Warning!! Instrument Class noteOn here!! %f %f\n",freq,amp);
}

void Instrmnt :: noteOff(MY_FLOAT amp)
{
    printf("Warning!! Instrument Class noteOff here!! %f\n",amp);
}

void Instrmnt :: setFreq(MY_FLOAT freq)
{
    printf("Warning!! Instrument Class setFreq here!! %f\n",freq);
}

MY_FLOAT Instrmnt :: tick()
{
    printf("Warning!! Instrument Class tick here!!\n");
    return lastOutput;
}

MY_FLOAT Instrmnt :: lastOut()
{
    return lastOutput;
}

void Instrmnt :: controlChange(int number, MY_FLOAT value)
{
    printf("Warning!! Instrument Class Control Change here!! %i %f\n",number,value);
}
