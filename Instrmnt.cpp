/******************************************/
/*  Instrument SuperClass for Toolkit96   */
/*  Perry R. Cook, Princeton University   */
/******************************************/

#include "Instrmnt.h"

Instrmnt :: Instrmnt()
{
}

void Instrmnt :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
}

void Instrmnt :: noteOff(MY_FLOAT amp)
{
}

void Instrmnt :: setFreq(MY_FLOAT freq)
{
}

MY_FLOAT Instrmnt :: tick()
{
}

MY_FLOAT Instrmnt :: lastOut()
{
    return lastOutput;
}

void Instrmnt :: controlChange(int number, MY_FLOAT value)
{
}
