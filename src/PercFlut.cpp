/******************************************/
/*  Percussive Flute Subclass             */
/*  of Algorithm 4 (TX81Z) Subclass of    */ 
/*  4 Operator FM Synth                   */
/*  by Perry R. Cook, 1995-96             */ 
/******************************************/

#include "PercFlut.h"

PercFlut :: PercFlut() : FM4Alg4()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file1[128];
  char file2[128];
  char file3[128];
  char file4[128];
  strcpy(file1, RAWWAVE_PATH);
  strcpy(file2, RAWWAVE_PATH);
  strcpy(file3, RAWWAVE_PATH);
  strcpy(file4, RAWWAVE_PATH);
  this->loadWaves(strcat(file1,"rawwaves/sinewave.raw"),
                  strcat(file2,"rawwaves/sinewave.raw"),
                  strcat(file3,"rawwaves/sinewave.raw"),
                  strcat(file4,"rawwaves/fwavblnk.raw"));
    
  this->setRatio(0,(MY_FLOAT) (1.50 * 1.000));
  this->setRatio(1,(MY_FLOAT) (3.00 * 0.995));
  this->setRatio(2,(MY_FLOAT) (2.99 * 1.005));
  this->setRatio(3,(MY_FLOAT) (6.00 * 0.997));
  gains[0] = __FM4Op_gains[99];
  gains[1] = __FM4Op_gains[71];
  gains[2] = __FM4Op_gains[93];
  gains[3] = __FM4Op_gains[85];
  adsr[0]->setAllTimes((MY_FLOAT) 0.05,(MY_FLOAT) 0.05,
                       __FM4Op_susLevels[14],(MY_FLOAT) 0.05);
  adsr[1]->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.50,
                       __FM4Op_susLevels[13],(MY_FLOAT) 0.5);
  adsr[2]->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.30,
                       __FM4Op_susLevels[11],(MY_FLOAT) 0.05);
  adsr[3]->setAllTimes((MY_FLOAT) 0.02,(MY_FLOAT) 0.05,
                       __FM4Op_susLevels[13],(MY_FLOAT) 0.01);
  twozero->setGain((MY_FLOAT) 0.0);
  modDepth = (MY_FLOAT) 0.005;
}  

void PercFlut :: setFreq(MY_FLOAT frequency)
{    
    baseFreq = frequency;
}

void PercFlut :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
    gains[0] = amp * __FM4Op_gains[99] * 0.5;
    gains[1] = amp * __FM4Op_gains[71] * 0.5;
    gains[2] = amp * __FM4Op_gains[93] * 0.5;
    gains[3] = amp * __FM4Op_gains[85] * 0.5;
    this->setFreq(freq);
    this->keyOn();
#if defined(_debug_)        
    printf("PercFlut : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

