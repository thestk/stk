/*******************************************/
/*
  ModalBar SubClass of Modal4 Instrument
  by Perry R. Cook, 1999-2000

  Controls:    CONTROL1 = stickHardness
               CONTROL2 = strikePosition
               CONTROL3 = Mode Presets
*/
/*******************************************/

#include "ModalBar.h"
#include "SKINI11.msg"

ModalBar :: ModalBar() : Modal4()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  wave = new RawWvIn(strcat(file,"rawwaves/marmstk1.raw"),"oneshot");
  wave->setRate((MY_FLOAT) 0.5);  /*  normal stick  */

  this->setRatioAndReson(0, (MY_FLOAT) 1.00,(MY_FLOAT) 0.9996);  /*  Set all       132.0  */
  this->setRatioAndReson(1, (MY_FLOAT) 3.99,(MY_FLOAT) 0.9994);  /*  of our        523.0  */
  this->setRatioAndReson(2,(MY_FLOAT) 10.65,(MY_FLOAT) 0.9994);  /*  default       1405.0 */
  this->setRatioAndReson(3,-(MY_FLOAT) 2443.0,(MY_FLOAT) 0.999);  /*  resonances    2443.0 */
  this->setFiltGain(0,(MY_FLOAT) 0.04);               /*  and        */
  this->setFiltGain(1,(MY_FLOAT) 0.01);               /*  gains      */
  this->setFiltGain(2,(MY_FLOAT) 0.01);               /*  for each   */
  this->setFiltGain(3,(MY_FLOAT) 0.008);              /*  resonance  */
  directGain = (MY_FLOAT) 0.1;
}  

ModalBar :: ~ModalBar()
{
  delete wave;
}  

void ModalBar :: setStickHardness(MY_FLOAT hardness)
{
  stickHardness = hardness;
  wave->setRate((MY_FLOAT) (0.25 * (MY_FLOAT)  pow(4.0,stickHardness)));
  masterGain = (MY_FLOAT) 0.1 + ((MY_FLOAT) 1.8 * stickHardness);
}

void ModalBar :: setStrikePosition(MY_FLOAT position)
{
  MY_FLOAT temp,temp2;
  temp2 = position * PI;
  strikePosition = position;             /*  Hack only first three modes */
  temp = (MY_FLOAT)  sin(temp2);                                       
  this->setFiltGain(0,(MY_FLOAT) 0.12 * temp);      /*  1st mode function of pos.   */
  temp = (MY_FLOAT)  sin(0.05 + (3.9 * temp2));
  this->setFiltGain(1,(MY_FLOAT) -0.03 * temp);     /*  2nd mode function of pos.   */
  temp = (MY_FLOAT)  sin(-0.05 + (11 * temp2));
  this->setFiltGain(2,(MY_FLOAT) 0.11 * temp);      /*  3rd mode function of pos.   */
}

void ModalBar :: setModalPreset(int which)
{
  /* presets:
   * first line:  relative modal frequencies (negative number is
   *              a fixed mode that doesn't scale with frequency
   * second line: resonances of the modes
   * third line:  mode volumes
   * fourth line: stickHardness, strikePosition, and direct stick
   *              gain (mixed directly into the output
   */
  int i, temp;
  MY_FLOAT presets[9][4][4] = { 
    {{1.0, 3.99, 10.65, -2443},		// Marimba
     {0.9996, 0.9994, 0.9994, 0.999},
     {0.04, 0.01, 0.01, 0.008},
     {0.429688, 0.445312, 0.093750}},
    {{1.0, 2.01, 3.9, 14.37}, 		// Vibraphone
     {0.99995, 0.99991, 0.99992, 0.9999},	
     {0.025, 0.015, 0.015, 0.015 },
     {0.390625,0.570312,0.078125}},
    {{1.0, 4.08, 6.669, -3725.0},		// Agogo 
     {0.999, 0.999, 0.999, 0.999},	
     {0.06, 0.05, 0.03, 0.02},
     {0.609375,0.359375,0.140625}},
    {{1.0, 2.777, 7.378, 15.377},		// Wood1
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.460938,0.375000,0.046875}},
    {{1.0, 2.777, 7.378, 15.377},		// Reso
     {0.99996, 0.99994, 0.99994, 0.9999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.453125,0.250000,0.101562}},
    {{1.0, 1.777, 2.378, 3.377},		// Wood2
     {0.996, 0.994, 0.994, 0.99},	
     {0.04, 0.01, 0.01, 0.008},
     {0.312500,0.445312,0.109375}},
    {{1.0, 1.004, 1.013, 2.377},		// Beats
     {0.9999, 0.9999, 0.9999, 0.999},	
     {0.02, 0.005, 0.005, 0.004},
     {0.398438,0.296875,0.070312}},
    {{1.0, 4.0, -1320.0, -3960.0},		// 2Fix
     {0.9996, 0.999, 0.9994, 0.999},	
     {0.04, 0.01, 0.01, 0.008},
     {0.453125,0.453125,0.070312}},
    {{1.0, 1.217, 1.475, 1.729},		// Clump
     {0.999, 0.999, 0.999, 0.999},	
     {0.03, 0.03, 0.03, 0.03 },
     {0.390625,0.570312,0.078125}},
  };

  temp = (which % 9);
  for (i=0; i<4; i++)	{
    this->setRatioAndReson(i, presets[temp][0][i], presets[temp][1][i]);
    this->setFiltGain(i,presets[temp][2][i]);
  }
  this->setStickHardness(presets[temp][3][0]);
  this->setStrikePosition(presets[temp][3][1]);
  directGain = presets[temp][3][2];

  if (temp == 1) // vibraphone
    vibrGain = 0.2;
  else
    vibrGain = 0.0;
}

void ModalBar :: controlChange(int number, MY_FLOAT value)
{
#if defined(_debug_)        
  printf("ModalBar : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_StickHardness_)
    this->setStickHardness(value * NORM_7);
  else if (number == __SK_StrikePosition_)
    this->setStrikePosition(value * NORM_7);
  else if (number == __SK_ProphesyRibbon_)
		this->setModalPreset((int) value);
  else if (number == __SK_ModWheel_)
    directGain = value * NORM_7;
  else if (number == __SK_AfterTouch_Cont_)	
    envelope->setTarget(value * NORM_7);
  else if (number == __SK_ModFrequency_)
    vibr->setFreq(value * NORM_7 * 12.0);
  else if (number == 1024)	{	//  HACKED Poop message
    printf("StickHard=%f   StrikePos=%f   directGain=%f\n",
           stickHardness, strikePosition, directGain);
  }
  else    {
    printf("ModalBar : Undefined Control Number!!\n");
  }    
}

