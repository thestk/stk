/*******************************************/
/*  SubSampled Noise Generator Class,      */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#include "SubNoise.h"    

SubNoise :: SubNoise() : Noise()
{    
    lastOutput = (MY_FLOAT) 0.0;
    howOften = 15;
    counter = 15;
}

SubNoise :: ~SubNoise()
{

}

SubNoise :: SubNoise(int subSample) : Noise()
{    
    lastOutput = (MY_FLOAT) 0.0;
    howOften = subSample-1;
    counter = subSample-1;
}

MY_FLOAT SubNoise :: tick()
{
    if (!counter)       {
        lastOutput = Noise::tick();
        counter = howOften;
    }
    else counter -= 1;
    return lastOutput;
}

void SubNoise :: setHowOften(int howOft)
{
    howOften = howOft;
}

/************  Test Main  ************************/
/*
void main()
{
    long i;
    SubNoise test(5);
    for (i=0;i<100;i++) printf("%lf\n",test.tick());
}
*/

