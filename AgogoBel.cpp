/*******************************************/
/*  AgogoBell SubClass of Modal4 Instrument*/
/*  by Perry R. Cook, 1995-96              */ 
/*                                         */
/*   Controls:    CONTROL1 = stickHardness */
/*                CONTROL2 = strikePosition*/
/*		  CONTROL3 = vibFreq       */
/*		  MOD_WHEEL= vibAmt        */
/*******************************************/

/*   Modes measured from my Agogo Bell by FFT:  */
/*   360, 1470, 2401, 4600                      */

#include "AgogoBel.h"

AgogoBel :: AgogoBel() : Modal4()
{
    wave = new RawWave("rawwaves/britestk.raw");
    wave->normalize();
    wave->setRate(7.0);     		/* hardstick */
    this->setRatioAndReson(0, 1.00,0.999);    /*  Set our      */
    this->setRatioAndReson(1, 4.08,0.999);    /*  resonances   */
    this->setRatioAndReson(2,6.669,0.999);    /*  here         */
    this->setRatioAndReson(3,-3725.0,0.999);  /*  (One fixed)  */
    this->setFiltGain(0,0.07);                /*  And filter   */
    this->setFiltGain(1,0.06);                /*    gains too  */
    this->setFiltGain(2,0.04);
    this->setFiltGain(3,0.02);
    directGain = 0.3;
}  

AgogoBel :: ~AgogoBel()
{
    delete wave;
}  

void AgogoBel :: setStickHardness(MY_FLOAT hardness)
{
    stickHardness = hardness;                    	/* To an approximation, */
    wave->setRate(3.0 + (8.0 * stickHardness)); 	/* hardness <-> center  */
    masterGain = 1.0;   				/* freq and amplitude  */
}

void AgogoBel :: setStrikePosition(MY_FLOAT position)
{
    MY_FLOAT temp,temp2;
    temp2 = position * PI;
    strikePosition = position;		/*  Hack only first  */
    temp = sin(0.7 * temp2);       	/*  three modes,     */
    this->setFiltGain(0,0.08 * temp);	/*  leave the other  */
    temp = sin(0.1 + (5.0 * temp2));	/*    fixed.  Why?   */
    this->setFiltGain(1,0.07 * temp);	/*  So it doesn't    */
    temp = sin(0.2 + (7.0 * temp2));	/*  sound like a     */
    this->setFiltGain(2,0.04 * temp);	/*  sample!          */
}

void AgogoBel :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("AgogoBel : ControlChange: Number=%i Value=%f\n",number,value);
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
        printf("AgogoBel : Undefined Control Number!!\n");
    } 
}
