/******************************************/
/*  Singing Voice Synthesis Subclass      */
/*  of Algorithm 6 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1996                */ 
/******************************************/

#include "FMVoices.h"

FMVoices :: FMVoices() : FM4Alg6()
{
    this->loadWaves("rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw",
                    "rawwaves/sinewave.raw");
    
    this->setRatio(0,2.00);
    this->setRatio(1,4.00);
    this->setRatio(2,12.0);
    this->setRatio(3,1.00);
    gains[3] = __FM4Op_gains[80];
    adsr[0]->setAll(0.001,0.001,__FM4Op_susLevels[15],0.001);
    adsr[1]->setAll(0.001,0.001,__FM4Op_susLevels[15],0.001);
    adsr[2]->setAll(0.001,0.001,__FM4Op_susLevels[15],0.001);
    adsr[3]->setAll(0.05,0.05,__FM4Op_susLevels[15],0.0001);
    twozero->setGain(0.0);
    modDepth = 0.005;
    currentVowel = 0;
    tilt[0] = 1.0;
    tilt[1] = 0.5;
    tilt[2] = 0.2;    
    mods[0] = 1.0;
    mods[1] = 1.1;
    mods[2] = 1.1;
    baseFreq = 110.0;
    this->setFreq(110.0);    
}  

#include "phonTabl.h"

void FMVoices :: setFreq(MY_FLOAT frequency)
{    
    MY_FLOAT temp,temp2;
    int tempi,tempi2;

    if (currentVowel < 16)	{
	tempi2 = currentVowel;
	temp2 = 0.9;
    }
    else if (currentVowel < 32)	{
	tempi2 = currentVowel - 16;
	temp2 = 1.0;
    }
    else if (currentVowel < 48)	{
	tempi2 = currentVowel - 32;
	temp2 = 1.1;
    }
    else if (currentVowel < 64)	{
	tempi2 = currentVowel - 48;
	temp2 = 1.2;
    }
    baseFreq = frequency;
    temp = (temp2 * phonParams[tempi2][0][0] / baseFreq) + 0.5;
    tempi = (int) temp;
    this->setRatio(0,(MY_FLOAT) tempi);
    temp = (temp2 * phonParams[tempi2][1][0] / baseFreq) + 0.5;
    tempi = (int) temp;
    this->setRatio(1,(MY_FLOAT) tempi);
    temp = (temp2 * phonParams[tempi2][2][0] / baseFreq) + 0.5;
    tempi = (int) temp;
    this->setRatio(2,(MY_FLOAT) tempi);    
    gains[0] = 1.0;  // pow(10.0,phonParams[tempi2][0][2] * 0.05);
    gains[1] = 1.0;  // pow(10.0,phonParams[tempi2][1][2] * 0.05);
    gains[2] = 1.0;  // pow(10.0,phonParams[tempi2][2][2] * 0.05);
}

void FMVoices :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    this->setFreq(freq);
    tilt[0] = amp;
    tilt[1] = amp * amp;
    tilt[2] = amp * amp * amp;
    this->keyOn();
#if defined(_debug_)        
    printf("FMVoices : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void FMVoices :: controlChange(int number, MY_FLOAT value)
{
    MY_FLOAT temp;
    int tempi;

#if defined(_debug_)        
    printf("FM4Op : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
    if (number == MIDI_control1)
        gains[3] = __FM4Op_gains[(int) (value * 0.78125)];
    else if (number == MIDI_control2)	{
	tempi = (int) (value / 2);
 	currentVowel = tempi;
  	this->setFreq(baseFreq);
    }
    else if (number == MIDI_control3)
        this->setModulationSpeed(value * NORM_7 * 12.0);  /* 0 to 12 Hz */
    else if (number == MIDI_mod_wheel)
        this->setModulationDepth(value * NORM_7);
    else if (number == MIDI_after_touch)	{
        temp = value * NORM_7;
	tilt[0] = temp;
	tilt[1] = temp * temp;
	tilt[2] = temp * temp * temp;
    }
    else	{
        printf("FM4Op : Undefined Control Number!!\n");
    }  
}

