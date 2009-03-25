/******************************************/
/*  Test Sampler Subclass of              */                   
/*  Sampling Synthesizer Class            */
/*  by Perry R. Cook, 1995-96             */ 
/*                                        */
/*   Controls:    CONTROL1 = filterQ      */
/*                CONTROL2 = filterRate   */
/*		  CONTROL3 = vibFreq      */
/*		  MOD_WHEEL= vibAmt       */
/******************************************/

#include "Moog1.h"

Moog1 :: Moog1() : SamplFlt()
{
    attacks[0] = new RawWave("rawwaves/mandpluk.raw");
    loops[0] = new RawWave("rawwaves/impuls20.raw");
    loops[1] = new RawWave("rawwaves/sinewave.raw"); /* Steal one for vibrato */
    attacks[0]->normalize();
    loops[0]->normalize();
    loops[0]->setLooping(1);
    loops[1]->normalize();
    loops[1]->setLooping(1);
    loops[1]->setFreq(6.122);
    adsr->setAll(0.05,0.00003,0.6,0.0002);
    filterQ = 0.85;
    filterRate = 0.0001;
    modDepth = 0.0;
}  

Moog1 :: ~Moog1()
{
    delete attacks[0];
    delete loops[0];
    delete loops[1];
}

void Moog1 :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
    attacks[0]->setFreq(baseFreq * 0.01);
    loops[0]->setFreq(baseFreq);
}

void Moog1 :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    MY_FLOAT temp;
    
    this->setFreq(freq);
    this->keyOn();
    attackGain = amp * 0.5;
    loopGain = amp;

    temp = filterQ+0.05;
    filters[0]->setStates(2000,temp,2.0 * (1.0 - temp));
    filters[1]->setStates(2000,temp,2.0 * (1.0 - temp));
    temp = filterQ+0.099;
    filters[0]->setTargets(  0,temp,2.0 * (1.0 - temp));
    filters[1]->setTargets(  0,temp,2.0 * (1.0 - temp));
    filters[0]->setSweepRate(filterRate * RATE_NORM);
    filters[1]->setSweepRate(filterRate * RATE_NORM);
#if defined(_debug_)        
    printf("Moog1 : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Moog1 :: setModulationSpeed(MY_FLOAT mSpeed)
{
    loops[1]->setFreq(mSpeed);
}

void Moog1 :: setModulationDepth(MY_FLOAT mDepth)
{
    modDepth = mDepth * 0.5;
}

void Moog1 :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Moog1 : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
        filterQ = 0.80 + (0.1 * value * NORM_7);
    else if (number == MIDI_control2)
        filterRate = (value * NORM_7 * 0.0002);
    else if (number == MIDI_control3)
        this->setModulationSpeed(value * NORM_7 * 12.0);
    else if (number == MIDI_mod_wheel)
        this->setModulationDepth(value * NORM_7);
    else if (number == MIDI_after_touch)
        adsr->setTarget(value * NORM_7);
    else    {
        printf("Moog1 : Undefined Control Number!!\n");
    }    
}

MY_FLOAT Moog1 :: tick()
{
    MY_FLOAT temp;

    if (modDepth!=0.0) {
        temp = loops[1]->tick() * modDepth;    
        loops[0]->setFreq(baseFreq * (1.0 + temp));
    }
    lastOutput = SamplFlt :: tick();
    return lastOutput;
}

