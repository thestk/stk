/*******************************************/
/*  Vibraphone SubClass of Modal4          */
/*  Instrument, by Perry R. Cook, 1995-96  */ 
/*                                         */
/*   Controls:    CONTROL1 = stickHardness */
/*                CONTROL2 = strikePosition*/
/*		  CONTROL3 = vibFreq       */
/*		  MOD_WHEEL= vibAmt        */
/*******************************************/

#include "Vibraphn.h"

Vibraphn :: Vibraphn() : Modal4()
{
    wave = new RawWave("rawwaves/marmstk1.raw");
    wave->normalize();
    wave->setRate(13.33);
    vibr->setFreq(4.0);
    onepole->setPole(0.2);
    this->setRatioAndReson(0, 1.0,0.99995);    /*  Set         */
    this->setRatioAndReson(1, 2.01,0.99991);    /*  our         */
    this->setRatioAndReson(2, 3.9,0.99992);    /*  resonance   */
    this->setRatioAndReson(3,14.37,0.99990);    /*  values here */
    this->setFiltGain(0,0.025);
    this->setFiltGain(1,0.015);
    this->setFiltGain(2,0.015);
    this->setFiltGain(3,0.015);
    masterGain = 1.0;
    directGain = 0.0;
    vibrGain = 0.2;
}  

Vibraphn :: ~Vibraphn()
{
}  

void Vibraphn :: setStickHardness(MY_FLOAT hardness)
{
    wave->setRate(2.0 + (22.66 * hardness));
    masterGain = 0.2 + (hardness * 1.6);
}

void Vibraphn :: setStrikePosition(MY_FLOAT position)
{
    MY_FLOAT temp,temp2;
    temp2 = position * PI;
    strikePosition = position;         /*  Hack only first three modes */
    temp = sin(strikePosition * PI);
    this->setFiltGain(0,0.025 * temp);
    temp = sin(0.1 + (2.01 * temp2));
    this->setFiltGain(1,0.015 * temp);
    temp = sin(3.95 * temp2);
    this->setFiltGain(2,0.015 * temp);
}

void Vibraphn :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Vibraphn : ControlChange: Number=%i Value=%f\n",number,value);
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
        printf("Vibraphn : Undefined Control Number!!\n");
    }    
}

