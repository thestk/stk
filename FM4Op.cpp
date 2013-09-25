/*******************************************/
/*  Master Class for 4 Operator FM Synth   */
/*  by Perry R. Cook, 1995-96              */ 
/*  This instrument contains 4 waves,      */
/*  4 adsr, and various state vars.        */
/*                                         */
/*  The basic Chowning/Stanford FM patent  */
/*  expired April 1995, but there exist    */
/*  follow-on patents, mostly assigned to  */
/*  Yamaha.  If you are of the type who    */
/*  should worry about this (making money) */
/*  worry away.                            */
/*                                         */
/*******************************************/

#include "FM4Op.h"
#include "SKINI11.msg"

FM4Op :: FM4Op()
{
    int i;
    MY_FLOAT temp;
    MY_FLOAT tempCoeffs[2] = {(MY_FLOAT) 0.0, (MY_FLOAT) -1.0};
    adsr[0] = new ADSR;
    adsr[1] = new ADSR;
    adsr[2] = new ADSR;
    adsr[3] = new ADSR;
    twozero = new TwoZero;    
    vibWave = new RawLoop("rawwaves/sinewave.raw");    
    vibWave->normalize();
    vibWave->setFreq((MY_FLOAT) 6.0);                /* should make this random?? */
    modDepth = (MY_FLOAT) 0.0;
        /*  We don't make the waves here yet, because    */
        /*  we don't know what they will be.             */
    baseFreq = (MY_FLOAT) 440.0;
    ratios[0] = (MY_FLOAT) 1.0;
    ratios[1] = (MY_FLOAT) 1.0;
    ratios[2] = (MY_FLOAT) 1.0;
    ratios[3] = (MY_FLOAT) 1.0;
    gains[0] = (MY_FLOAT) 1.0;
    gains[1] = (MY_FLOAT) 1.0;
    gains[2] = (MY_FLOAT) 1.0;
    gains[3] = (MY_FLOAT) 1.0;
    twozero->setZeroCoeffs(tempCoeffs);
    twozero->setGain((MY_FLOAT) 0.0);
    control1 = (MY_FLOAT) 1.0;
    control2 = (MY_FLOAT) 1.0;
    temp = (MY_FLOAT) 1.0;
    for (i=99;i>=0;i--)	{
        __FM4Op_gains[i] = temp;
        temp *= (MY_FLOAT) 0.933033;
    }
    temp = (MY_FLOAT) 1.0;
    for (i=15;i>=0;i--)	{
        __FM4Op_susLevels[i] = temp;
        temp *= (MY_FLOAT) 0.707101;
    }
    temp = (MY_FLOAT) 8.498186;
    for (i=0;i<32;i++)	{
        __FM4Op_attTimes[i] = temp;
        temp *= (MY_FLOAT) 0.707101;
    }
}  

FM4Op :: ~FM4Op()
{
    delete adsr[0]; 
    delete adsr[1]; 
    delete adsr[2]; 
    delete adsr[3]; 
    delete waves[0];
    delete waves[1];
    delete waves[2];
    delete waves[3];
    delete vibWave;
    delete twozero;
}

void FM4Op :: loadWaves(char* wave1, char* wave2, char* wave3, char* wave4)
{
    int i;
    waves[0] = new RawLoop(wave1);
    waves[1] = new RawLoop(wave2);
    waves[2] = new RawLoop(wave3);
    waves[3] = new RawLoop(wave4);
    for (i=0;i<4;i++)   {
        waves[i]->normalize();
    }
}
void FM4Op :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    waves[0]->setFreq(baseFreq * ratios[0]);
    waves[1]->setFreq(baseFreq * ratios[1]);
    waves[2]->setFreq(baseFreq * ratios[2]);
    waves[3]->setFreq(baseFreq * ratios[3]);
}

void FM4Op :: setRatio(int whichOne, MY_FLOAT ratio)
{
    ratios[whichOne] = ratio;
    if (ratio>0.0) 
        waves[whichOne]->setFreq(baseFreq * ratio);
    else
        waves[whichOne]->setFreq(ratio);
}

void FM4Op :: setGain(int whichOne, MY_FLOAT gain)
{
    gains[whichOne]=gain;
}

void FM4Op :: keyOn()
{
    adsr[0]->keyOn();
    adsr[1]->keyOn();
    adsr[2]->keyOn();
    adsr[3]->keyOn();
}

void FM4Op :: keyOff()
{
    adsr[0]->keyOff();
    adsr[1]->keyOff();
    adsr[2]->keyOff();
    adsr[3]->keyOff();
}

void FM4Op :: noteOff(MY_FLOAT amp)
{
    this->keyOff();
#if defined(_debug_)        
    printf("FM4Op : NoteOff: Amp=%lf\n",amp);
#endif    
}

void FM4Op :: setModulationSpeed(MY_FLOAT mSpeed)
{
    vibWave->setFreq(mSpeed);
}

void FM4Op :: setModulationDepth(MY_FLOAT mDepth)
{
    modDepth = mDepth;
}

void FM4Op :: setControl1(MY_FLOAT cVal)
{
    control1 = cVal * (MY_FLOAT) 2.0;
}

void FM4Op :: setControl2(MY_FLOAT cVal)
{
    control2 = cVal * (MY_FLOAT) 2.0;
}

void FM4Op :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("FM4Op : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == __SK_Breath_)
        this->setControl1(value * NORM_7);
    else if (number == __SK_FootControl_)
        this->setControl2(value * NORM_7);
    else if (number == __SK_ModFrequency_)
        this->setModulationSpeed(value * NORM_7 * (MY_FLOAT) 12.0);  /* 0 to 12 Hz */
    else if (number == __SK_ModWheel_)
        this->setModulationDepth(value * NORM_7);
    else if (number == __SK_AfterTouch_Cont_)	{
        adsr[0]->setTarget(value * NORM_7);
        adsr[1]->setTarget(value * NORM_7);
        adsr[2]->setTarget(value * NORM_7);
        adsr[3]->setTarget(value * NORM_7);
    }
    else	{
        printf("FM4Op : Undefined Control Number!!\n");
    }  
}

