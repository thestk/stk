/*******************************************/
/*  ADSR Subclass of the Envelope Class,   */
/*  by Perry R. Cook, 1995-96              */ 
/*  This is the traditional ADSR (Attack   */
/*  Decay, Sustain, Release) ADSR.         */
/*  It responds to simple KeyOn and KeyOff */
/*  messages, keeping track of it's state. */         
/*  There are two tick (update value)      */
/*  methods, one returns the value, and    */
/*  other returns the state (0 = A, 1 = D, */
/*  2 = S, 3 = R)                          */
/*******************************************/

#include "ADSR.h"    

ADSR :: ADSR() : Envelope()
{    
    target = 0.0;
    value = 0.0;
    attackRate = 0.001;
    decayRate = 0.001;
    sustainLevel = 0.5;
    releaseRate = 0.01;
    state = 0;
}

ADSR :: ~ADSR()
{    
    /* Nothing to do here */
}

void ADSR :: keyOn()
{
    target = 1.0;
    rate = attackRate;
    state = 0;
}

void ADSR :: keyOff()
{
    target = 0.0;
    rate = releaseRate;
    state = 3;
}

void ADSR :: setAttackRate(MY_FLOAT aRate)
{
    if (aRate < 0.0) {
        printf("negative rates not allowed!!, correcting\n");
        attackRate = -aRate;
    }
    else attackRate = aRate;
    attackRate = attackRate * RATE_NORM;  /*  SEE Object.h  */
}

void ADSR :: setDecayRate(MY_FLOAT aRate)
{
    if (aRate < 0.0) {
        printf("negative rates not allowed!!, correcting\n");
        decayRate = -aRate;
    }
    else decayRate = aRate;
    decayRate = decayRate * RATE_NORM;	/*  SEE Object.h  */
}

void ADSR :: setSustainLevel(MY_FLOAT aLevel)
{
    if (aLevel < 0.0 ) {
        printf("Sustain level out of range!!, correcting\n");
        sustainLevel = 0.0;
    }
    else sustainLevel = aLevel;
}

void ADSR :: setReleaseRate(MY_FLOAT aRate)
{
    if (aRate < 0.0) {
        printf("negative rates not allowed!!, correcting\n");
        releaseRate = -aRate;
    }
    else releaseRate = aRate;
    releaseRate = releaseRate * RATE_NORM;	/*  SEE Object.h  */
}

void ADSR :: setAll(MY_FLOAT attRate, MY_FLOAT decRate, MY_FLOAT susLevel, MY_FLOAT relRate)
{
    this->setAttackRate(attRate);
    this->setDecayRate(decRate);
    this->setSustainLevel(susLevel);
    this->setReleaseRate(relRate);
}

void ADSR :: setTarget(MY_FLOAT aTarget)
{
    target = aTarget;
    if (value < target) {
	state = ATTACK;
	this->setSustainLevel(target);
	rate = attackRate;
    }
    if (value > target) {
	this->setSustainLevel(target);
	state = DECAY;
	rate = decayRate;
    }
}

void ADSR :: setValue(MY_FLOAT aValue)
{
    state = SUSTAIN;
    target = aValue;
    value = aValue;
    this->setSustainLevel(aValue);
    rate = 0.0;
}

MY_FLOAT ADSR :: tick()
{
    if (state==ATTACK)  {
        value += rate;
	if (value >= target)    {
            value = target;
            rate = decayRate;
            target = sustainLevel;
	    state = DECAY;
        }
    }    
    else if (state==DECAY)  {
        value -= decayRate;
        if (value <= sustainLevel)    {
            value = sustainLevel;
            rate = 0.0;
            state = SUSTAIN;
        }
    }
    else if (state==RELEASE)  {
        value -= releaseRate;
        if (value <= 0.0)       {
            value = 0.0;
            state = 4;
        }
    }
    return value;
}

int ADSR :: informTick()
{
    this->tick();
    return state;
}

MY_FLOAT ADSR :: lastOut()
{
    return value;
}

/************  Test Main  ************************/
/*
void main()
{
    long i;
    ADSR test;
    
    test.setAttackRate(0.15);
    test.keyOn();
    while(test.informTick()==ATTACK) printf("%lf\n",test.tick());
    test.setDecayRate(0.1);
    while (test.informTick()==DECAY) printf("%lf\n",test.lastOut());
    test.setReleaseRate(0.05);
    test.keyOff();
    while(test.informTick()==RELEASE) printf("%lf\n",test.lastOut());
}
*/
