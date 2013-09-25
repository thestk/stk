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
#include "SKINI11.msg"

Vibraphn :: Vibraphn() : Modal4()
{
    wave = new RawInterp("rawwaves/marmstk1.raw");
    wave->normalize();
    wave->setRate((MY_FLOAT) 13.33);
    vibr->setFreq((MY_FLOAT) 4.0);
    onepole->setPole((MY_FLOAT) 0.2);
    this->setRatioAndReson(0, (MY_FLOAT) 1.0,(MY_FLOAT) 0.99995);    /*  Set         */
    this->setRatioAndReson(1, (MY_FLOAT) 2.01,(MY_FLOAT) 0.99991);    /*  our         */
    this->setRatioAndReson(2, (MY_FLOAT) 3.9,(MY_FLOAT) 0.99992);    /*  resonance   */
    this->setRatioAndReson(3,(MY_FLOAT) 14.37,(MY_FLOAT) 0.99990);    /*  values here */
    this->setFiltGain(0,(MY_FLOAT) 0.025);
    this->setFiltGain(1,(MY_FLOAT) 0.015);
    this->setFiltGain(2,(MY_FLOAT) 0.015);
    this->setFiltGain(3,(MY_FLOAT) 0.015);
    masterGain = (MY_FLOAT) 1.0;
    directGain = (MY_FLOAT) 0.0;
    vibrGain = (MY_FLOAT) 0.2;
}  

Vibraphn :: ~Vibraphn()
{
  delete wave;
}  

void Vibraphn :: setStickHardness(MY_FLOAT hardness)
{
    wave->setRate((MY_FLOAT) 2.0 + ((MY_FLOAT)  22.66 * hardness));
    masterGain = (MY_FLOAT) 0.2 + (hardness * (MY_FLOAT) 1.6);
}

void Vibraphn :: setStrikePosition(MY_FLOAT position)
{
    MY_FLOAT temp,temp2;
    temp2 = position * PI;
    strikePosition = position;         /*  Hack only first three modes */
    temp = (MY_FLOAT) sin(strikePosition * PI);
    this->setFiltGain(0,(MY_FLOAT) 0.025 * temp);
    temp = (MY_FLOAT) sin(0.1 + (2.01 * temp2));
    this->setFiltGain(1,(MY_FLOAT) 0.015 * temp);
    temp = (MY_FLOAT) sin(3.95 * temp2);
    this->setFiltGain(2,(MY_FLOAT) 0.015 * temp);
}

void Vibraphn :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
    printf("Vibraphn : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == __SK_StickHardness_)
        this->setStickHardness(value * NORM_7);
    else if (number == __SK_StrikePosition_)
        this->setStrikePosition(value * NORM_7);
    else if (number == __SK_ModFrequency_)
	    vibr->setFreq((value * NORM_7 * (MY_FLOAT) 12.0));
    else if (number == __SK_ModWheel_)
	    vibrGain = (value * NORM_7);
    else if (number == __SK_AfterTouch_Cont_)
        this->strike(value * NORM_7);
    else    {        
        printf("Vibraphn : Undefined Control Number!!\n");
    }    
}

