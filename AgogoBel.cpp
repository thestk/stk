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
#include "SKINI11.msg"

AgogoBel :: AgogoBel() : Modal4()
{
  wave = new RawWave("rawwaves/britestk.raw");
  wave->normalize();
  wave->setRate((MY_FLOAT) 7.0);     		/* hardstick */
  this->setRatioAndReson(0, (MY_FLOAT) 1.00,(MY_FLOAT) 0.999);    /*  Set our      */
  this->setRatioAndReson(1, (MY_FLOAT) 4.08,(MY_FLOAT) 0.999);    /*  resonances   */
  this->setRatioAndReson(2,(MY_FLOAT) 6.669,(MY_FLOAT) 0.999);    /*  here         */
  this->setRatioAndReson(3,(MY_FLOAT) -3725.0, (MY_FLOAT)0.999);  /*  (One fixed)  */
  this->setFiltGain(0,(MY_FLOAT) 0.06);                /*  And filter   */
  this->setFiltGain(1,(MY_FLOAT) 0.05);                /*    gains too  */
  this->setFiltGain(2,(MY_FLOAT) 0.03);
  this->setFiltGain(3,(MY_FLOAT) 0.02);
  directGain = (MY_FLOAT) 0.25;
}  

AgogoBel :: ~AgogoBel()
{
  delete wave;
}  

void AgogoBel :: setStickHardness(MY_FLOAT hardness)
{
  stickHardness = hardness;                    	/* To an approximation, */
  wave->setRate((MY_FLOAT) 3.0 + ((MY_FLOAT) 8.0 * stickHardness)); 	/* hardness <-> center  */
  masterGain = (MY_FLOAT) 1.0;   				/* freq and amplitude  */
}

void AgogoBel :: setStrikePosition(MY_FLOAT position)
{
  MY_FLOAT temp,temp2;
  temp2 = position * PI;
  strikePosition = position;		/*  Hack only first  */
  temp = (MY_FLOAT)  sin(0.7 * temp2);       	/*  three modes,     */
  this->setFiltGain(0,(MY_FLOAT) 0.08 * temp);	/*  leave the other  */
  temp = (MY_FLOAT)  sin(0.1 + (5.0 * temp2));	/*    fixed.  Why?   */
  this->setFiltGain(1,(MY_FLOAT) 0.07 * temp);	/*  So it doesn't    */
  temp = (MY_FLOAT)  sin(0.2 + (7.0 * temp2));	/*  sound like a     */
  this->setFiltGain(2,(MY_FLOAT) 0.04 * temp);	/*  sample!          */
}

void AgogoBel :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("AgogoBel : ControlChange: Number=%i Value=%f\n",number,value);
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
	printf("AgogoBel : Undefined Control Number!!\n");
  } 
}
