/*******************************************/
/*  Marimba SubClass of Modal4 Instrument, */
/*  by Perry R. Cook, 1995-96              */ 
/*                                         */
/*   Controls:    CONTROL1 = stickHardness */
/*                CONTROL2 = strikePosition*/
/*		  CONTROL3 = vibFreq       */
/*		  MOD_WHEEL= vibAmt        */
/*******************************************/

#include "Marimba.h"

Marimba :: Marimba() : Modal4()
{
    wave = new RawWave("rawwaves/marmstk1.raw");
    wave->normalize();
    wave->setRate(0.5);                      /*  normal stick  */
    this->setRatioAndReson(0, 1.00,0.9996);  /*  Set all       132.0  */
    this->setRatioAndReson(1, 3.99,0.9994);  /*  of our        523.0  */
    this->setRatioAndReson(2,10.65,0.9994);  /*  default       1405.0 */
    this->setRatioAndReson(3,-18.50,0.999);  /*  resonances    2443.0 */
    this->setFiltGain(0,0.08);               /*  and        */
    this->setFiltGain(1,0.02);               /*  gains      */
    this->setFiltGain(2,0.02);               /*  for each   */
    this->setFiltGain(3,0.015);              /*  resonance  */
    directGain = 0.1;
    multiStrike = 0;
}  

Marimba :: ~Marimba()
{
}  

void Marimba :: setStickHardness(MY_FLOAT hardness)
{
    stickHardness = hardness;
    wave->setRate(0.25 * pow(4.0,stickHardness));
    masterGain = 0.1 + (1.8 * stickHardness);
}

void Marimba :: setStrikePosition(MY_FLOAT position)
{
    MY_FLOAT temp,temp2;
    temp2 = position * PI;
    strikePosition = position;             /*  Hack only first three modes */
    temp = sin(temp2);                                       
    this->setFiltGain(0,0.12 * temp);      /*  1st mode function of pos.   */
    temp = sin(0.05 + (3.9 * temp2));
    this->setFiltGain(1,-0.03 * temp);     /*  2nd mode function of pos.   */
    temp = sin(-0.05 + (11 * temp2));
    this->setFiltGain(2,0.11 * temp);      /*  3rd mode function of pos.   */
}

void Marimba :: setModulationSpeed(MY_FLOAT mSpeed)
{
    /* don't bother here, marimba decay so fast, mod doesn't make sense  */
}

void Marimba :: setModulationDepth(MY_FLOAT mDepth)
{
}

void Marimba :: strike(MY_FLOAT amplitude)
{
    int temp;
    temp = random() >> 26;
    if (temp < 2) {
        multiStrike = 1;
#if defined(_debug_)
	printf("striking twice here!!\n");
#endif
    }
    else if (temp < 1) {
        multiStrike = 2;
#if defined(_debug_)
	printf("striking three times here!!!\n");
#endif
    }
    else multiStrike = 0;
    Modal4::strike(amplitude);
}

void Marimba :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Marimba : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
        this->setStickHardness(value * NORM_7);
    else if (number == MIDI_control2)
        this->setStrikePosition(value * NORM_7);
    else if (number == MIDI_control3)
	vibr->setFreq((value * NORM_7 * 12.0));
    else if (number == MIDI_mod_wheel)
	vibrGain = (value * NORM_7);
    else if (number == MIDI_after_touch)
        this->strike(value * NORM_7);
    else    {
        printf("Marimba : Undefined Control Number!!\n");
    }    
}

MY_FLOAT Marimba :: tick()
{
    if (multiStrike>0)
        if (wave->isAllDone()) {
	    wave->reset();
	    multiStrike -= 1;
	}
    return Modal4::tick();
}
