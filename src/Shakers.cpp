/***************************************************/
/*! \class Shakers
    \brief PhISEM and PhOLIES class.

    PhISEM (Physically Informed Stochastic Event
    Modeling) is an algorithmic approach for
    simulating collisions of multiple independent
    sound producing objects.  This class is a
    meta-model that can simulate a Maraca, Sekere,
    Cabasa, Bamboo Wind Chimes, Water Drops,
    Tambourine, Sleighbells, and a Guiro.

    PhOLIES (Physically-Oriented Library of
    Imitated Environmental Sounds) is a similar
    approach for the synthesis of environmental
    sounds.  This class implements simulations of
    breaking sticks, crunchy snow (or not), a
    wrench, sandpaper, and more.

    Control Change Numbers: 
       - Shake Energy = 2
       - System Decay = 4
       - Number Of Objects = 11
       - Resonance Frequency = 1
       - Shake Energy = 128
       - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Guiro = 3
        - Water Drops = 4
        - Bamboo Chimes = 5
        - Tambourine = 6
        - Sleigh Bells = 7
        - Sticks = 8
        - Crunch = 9
        - Wrench = 10
        - Sand Paper = 11
        - Coke Can = 12
        - Next Mug = 13
        - Penny + Mug = 14
        - Nickle + Mug = 15
        - Dime + Mug = 16
        - Quarter + Mug = 17
        - Franc + Mug = 18
        - Peso + Mug = 19
        - Big Rocks = 20
        - Little Rocks = 21
        - Tuned Bamboo Chimes = 22

    by Perry R. Cook, 1996 - 1999.
*/
/***************************************************/

#include "Stk.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

int my_random(int max) //  Return Random Int Between 0 and max
{
  int temp = (int) ((float)max * rand() / (RAND_MAX + 1.0) );
  return temp;
}

MY_FLOAT float_random(MY_FLOAT max) // Return random float between 0.0 and max
{	
  MY_FLOAT temp = (MY_FLOAT) (max * rand() / (RAND_MAX + 1.0) );
  return temp;	
}

MY_FLOAT noise_tick() //  Return random MY_FLOAT float between -1.0 and 1.0
{
  MY_FLOAT temp = (MY_FLOAT) (2.0 * rand() / (RAND_MAX + 1.0) );
  temp -= 1.0;
  return temp;
}

// Maraca
#define MARA_SOUND_DECAY 0.95
#define MARA_SYSTEM_DECAY 0.999
#define MARA_GAIN 20.0
#define MARA_NUM_BEANS 25
#define MARA_CENTER_FREQ 3200.0
#define MARA_RESON 0.96

// Sekere
#define SEKE_SOUND_DECAY 0.96
#define SEKE_SYSTEM_DECAY 0.999
#define SEKE_GAIN 20.0
#define SEKE_NUM_BEANS 64
#define SEKE_CENTER_FREQ 5500.0
#define SEKE_RESON 0.6

// Sandpaper
#define SANDPAPR_SOUND_DECAY 0.999
#define SANDPAPR_SYSTEM_DECAY 0.999
#define SANDPAPR_GAIN 0.5
#define SANDPAPR_NUM_GRAINS 128
#define SANDPAPR_CENTER_FREQ 4500.0
#define SANDPAPR_RESON 0.6

// Cabasa
#define CABA_SOUND_DECAY 0.96
#define CABA_SYSTEM_DECAY 0.997
#define CABA_GAIN 40.0
#define CABA_NUM_BEADS 512
#define CABA_CENTER_FREQ 3000.0
#define CABA_RESON 0.7

// Bamboo Wind Chimes
#define BAMB_SOUND_DECAY 0.95
#define BAMB_SYSTEM_DECAY 0.9999
#define BAMB_GAIN 2.0
#define BAMB_NUM_TUBES 1.25
#define BAMB_CENTER_FREQ0  2800.0
#define BAMB_CENTER_FREQ1  0.8 * 2800.0
#define BAMB_CENTER_FREQ2  1.2 * 2800.0
#define BAMB_RESON	   0.995

// Tuned Bamboo Wind Chimes (Anklung)
#define TBAMB_SOUND_DECAY 0.95
#define TBAMB_SYSTEM_DECAY 0.9999
#define TBAMB_GAIN 1.0
#define TBAMB_NUM_TUBES 1.25
#define TBAMB_CENTER_FREQ0 1046.6
#define TBAMB_CENTER_FREQ1  1174.8
#define TBAMB_CENTER_FREQ2  1397.0
#define TBAMB_CENTER_FREQ3  1568.0
#define TBAMB_CENTER_FREQ4  1760.0
#define TBAMB_CENTER_FREQ5  2093.3
#define TBAMB_CENTER_FREQ6  2350.0
#define TBAMB_RESON	   0.996

// Water Drops
#define WUTR_SOUND_DECAY 0.95
#define WUTR_SYSTEM_DECAY 0.996
#define WUTR_GAIN 1.0
#define WUTR_NUM_SOURCES 10
#define WUTR_CENTER_FREQ0  450.0
#define WUTR_CENTER_FREQ1  600.0
#define WUTR_CENTER_FREQ2  750.0 
#define WUTR_RESON   0.9985
#define WUTR_FREQ_SWEEP  1.0001

// Tambourine
#define TAMB_SOUND_DECAY 0.95
#define TAMB_SYSTEM_DECAY 0.9985
#define TAMB_GAIN 5.0
#define TAMB_NUM_TIMBRELS 32
#define TAMB_SHELL_FREQ 2300
#define TAMB_SHELL_GAIN 0.1
#define TAMB_SHELL_RESON 0.96
#define TAMB_CYMB_FREQ1  5600
#define TAMB_CYMB_FREQ2 8100
#define TAMB_CYMB_RESON 0.99

// Sleighbells
#define SLEI_SOUND_DECAY 0.97
#define SLEI_SYSTEM_DECAY 0.9994
#define SLEI_GAIN 1.0
#define SLEI_NUM_BELLS 32
#define SLEI_CYMB_FREQ0 2500
#define SLEI_CYMB_FREQ1 5300
#define SLEI_CYMB_FREQ2 6500
#define SLEI_CYMB_FREQ3 8300
#define SLEI_CYMB_FREQ4 9800
#define SLEI_CYMB_RESON 0.99 

// Guiro
#define GUIR_SOUND_DECAY 0.95
#define GUIR_GAIN 10.0
#define GUIR_NUM_PARTS 128
#define GUIR_GOURD_FREQ  2500.0
#define GUIR_GOURD_RESON 0.97
#define GUIR_GOURD_FREQ2  4000.0
#define GUIR_GOURD_RESON2 0.97

// Wrench
#define WRENCH_SOUND_DECAY 0.95
#define WRENCH_GAIN 5
#define WRENCH_NUM_PARTS 128
#define WRENCH_FREQ  3200.0
#define WRENCH_RESON 0.99
#define WRENCH_FREQ2  8000.0
#define WRENCH_RESON2 0.992

// Cokecan
#define COKECAN_SOUND_DECAY 0.97
#define COKECAN_SYSTEM_DECAY 0.999
#define COKECAN_GAIN 0.8
#define COKECAN_NUM_PARTS 48
#define COKECAN_HELMFREQ 370
#define COKECAN_HELM_RES  0.99
#define COKECAN_METLFREQ0 1025
#define COKECAN_METLFREQ1 1424
#define COKECAN_METLFREQ2 2149
#define COKECAN_METLFREQ3 3596
#define COKECAN_METL_RES 0.992 

// PhOLIES (Physically-Oriented Library of Imitated Environmental
// Sounds), Perry Cook, 1997-8

// Stix1
#define STIX1_SOUND_DECAY 0.96
#define STIX1_SYSTEM_DECAY 0.998
#define STIX1_GAIN 30.0
#define STIX1_NUM_BEANS 2
#define STIX1_CENTER_FREQ 5500.0
#define STIX1_RESON 0.6

// Crunch1
#define CRUNCH1_SOUND_DECAY 0.95
#define CRUNCH1_SYSTEM_DECAY 0.99806
#define CRUNCH1_GAIN 20.0
#define CRUNCH1_NUM_BEADS 7
#define CRUNCH1_CENTER_FREQ 800.0
#define CRUNCH1_RESON 0.95

// Nextmug
#define NEXTMUG_SOUND_DECAY 0.97
#define NEXTMUG_SYSTEM_DECAY 0.9995
#define NEXTMUG_GAIN 0.8
#define NEXTMUG_NUM_PARTS 3
#define NEXTMUG_FREQ0 2123
#define NEXTMUG_FREQ1 4518
#define NEXTMUG_FREQ2 8856
#define NEXTMUG_FREQ3 10753
#define NEXTMUG_RES 0.997 

#define PENNY_FREQ0 11000
#define PENNY_FREQ1 5200
#define PENNY_FREQ2 3835
#define PENNY_RES   0.999

#define NICKEL_FREQ0 5583
#define NICKEL_FREQ1 9255
#define NICKEL_FREQ2 9805
#define NICKEL_RES   0.9992

#define DIME_FREQ0 4450
#define DIME_FREQ1 4974
#define DIME_FREQ2 9945
#define DIME_RES   0.9993

#define QUARTER_FREQ0 1708
#define QUARTER_FREQ1 8863
#define QUARTER_FREQ2 9045
#define QUARTER_RES   0.9995

#define FRANC_FREQ0 5583
#define FRANC_FREQ1 11010
#define FRANC_FREQ2 1917
#define FRANC_RES   0.9995

#define PESO_FREQ0 7250
#define PESO_FREQ1 8150
#define PESO_FREQ2 10060
#define PESO_RES   0.9996

// Big Gravel
#define BIGROCKS_SOUND_DECAY 0.98
#define BIGROCKS_SYSTEM_DECAY 0.9965
#define BIGROCKS_GAIN 20.0
#define BIGROCKS_NUM_PARTS 23
#define BIGROCKS_FREQ 6460
#define BIGROCKS_RES 0.932 

// Little Gravel
#define LITLROCKS_SOUND_DECAY 0.98
#define LITLROCKS_SYSTEM_DECAY 0.99586
#define LITLROCKS_GAIN 20.0
#define LITLROCKS_NUM_PARTS 1600
#define LITLROCKS_FREQ 9000
#define LITLROCKS_RES 0.843 

// Finally ... the class code!

#include "Shakers.h"
#include "SKINI.msg"

Shakers :: Shakers()
{
  int i;

  instType = 0;
  shakeEnergy = 0.0;
  nFreqs = 0;
  sndLevel = 0.0;

  for ( i=0; i<MAX_FREQS; i++ )	{
    inputs[i] = 0.0;
    outputs[i][0] = 0.0;
    outputs[i][1] = 0.0;
    coeffs[i][0] = 0.0;
    coeffs[i][1] = 0.0;
    gains[i] = 0.0;
    center_freqs[i] = 0.0;
    resons[i] =  0.0;
    freq_rand[i] = 0.0;
    freqalloc[i] = 0;
  }

  soundDecay = 0.0;
  systemDecay = 0.0;
  nObjects = 0.0;
  collLikely = 0.0;
  totalEnergy = 0.0;
  ratchet = 0.0;
  ratchetDelta = 0.0005;
  lastRatchetPos = 0;
  finalZ[0] = 0.0;
  finalZ[1] = 0.0;
  finalZ[2] = 0.0;
  finalZCoeffs[0] = 1.0;
  finalZCoeffs[1] = 0.0;
  finalZCoeffs[2] = 0.0;

  this->setupNum(instType);
}

Shakers :: ~Shakers()
{
}

#define MAX_SHAKE 2000.0

char instrs[NUM_INSTR][10] = {
  "Maraca", "Cabasa", "Sekere", "Guiro",
  "Waterdrp", "Bamboo", "Tambourn", "Sleighbl", 
  "Stix1", "Crunch1", "Wrench", "SandPapr",
  "CokeCan", "NextMug", "PennyMug", "NicklMug",
  "DimeMug", "QuartMug", "FrancMug", "PesoMug",
  "BigRocks", "LitlRoks", "TBamboo"
};

int Shakers :: setupName(char* instr)
{
  int which = 0;

  for (int i=0;i<NUM_INSTR;i++)	{
    if ( !strcmp(instr,instrs[i]) )
	    which = i;
  }

#if defined(_STK_DEBUG_)
  cerr << "Shakers: Setting instrument to " << instrs[which] << endl;
#endif

  return this->setupNum(which);
}

void Shakers :: setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2)	 {
  finalZCoeffs[0] = z0;
  finalZCoeffs[1] = z1;
  finalZCoeffs[2] = z2;
}

void Shakers :: setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay) {
  soundDecay = sndDecay;
  systemDecay = sysDecay;
}

int Shakers :: setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson) {
  if (which < MAX_FREQS)	{
    resons[which] = reson;
    center_freqs[which] = freq;
    t_center_freqs[which] = freq;
    coeffs[which][1] = reson * reson;
    coeffs[which][0] = -reson * 2.0 * cos(freq * TWO_PI / Stk::sampleRate());
    return 1;
  }
  else return 0;
}

int Shakers :: setupNum(int inst)
{
  int i, rv = 0;
  MY_FLOAT temp;

  if (inst == 1) { // Cabasa
    rv = inst;
    nObjects = CABA_NUM_BEADS;
    defObjs[inst] = CABA_NUM_BEADS;
    setDecays(CABA_SOUND_DECAY, CABA_SYSTEM_DECAY);
    defDecays[inst] = CABA_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = CABA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CABA_CENTER_FREQ,CABA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 2) { // Sekere
    rv = inst;
    nObjects = SEKE_NUM_BEANS;
    defObjs[inst] = SEKE_NUM_BEANS;
    this->setDecays(SEKE_SOUND_DECAY,SEKE_SYSTEM_DECAY);
    defDecays[inst] = SEKE_SYSTEM_DECAY;
    decayScale[inst] = 0.94;
    nFreqs = 1;
    baseGain = SEKE_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SEKE_CENTER_FREQ,SEKE_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 3) { //  Guiro
    rv = inst;
    nObjects = GUIR_NUM_PARTS;
    defObjs[inst] = GUIR_NUM_PARTS;
    setDecays(GUIR_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 1.0;
    nFreqs = 2;
    baseGain = GUIR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,GUIR_GOURD_FREQ,GUIR_GOURD_RESON);
    setFreqAndReson(1,GUIR_GOURD_FREQ2,GUIR_GOURD_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 4) { //  Water Drops
    rv = inst;
    nObjects = WUTR_NUM_SOURCES;
    defObjs[inst] = WUTR_NUM_SOURCES;
    setDecays(WUTR_SOUND_DECAY,WUTR_SYSTEM_DECAY);
    defDecays[inst] = WUTR_SYSTEM_DECAY;
    decayScale[inst] = 0.8;
    nFreqs = 3;
    baseGain = WUTR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(1,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFreqAndReson(2,WUTR_CENTER_FREQ0,WUTR_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 5) { // Bamboo
    rv = inst;
    nObjects = BAMB_NUM_TUBES;
    defObjs[inst] = BAMB_NUM_TUBES;
    setDecays(BAMB_SOUND_DECAY, BAMB_SYSTEM_DECAY);
    defDecays[inst] = BAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 3;
    baseGain = BAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    freqalloc[0] = 1;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.2;
    freq_rand[1] = 0.2;
    freq_rand[2] = 0.2;
    setFreqAndReson(0,BAMB_CENTER_FREQ0,BAMB_RESON);
    setFreqAndReson(1,BAMB_CENTER_FREQ1,BAMB_RESON);
    setFreqAndReson(2,BAMB_CENTER_FREQ2,BAMB_RESON);
    setFinalZs(1.0,0.0,0.0);
  }
  else if (inst == 6) { // Tambourine
    rv = inst;
    nObjects = TAMB_NUM_TIMBRELS;
    defObjs[inst] = TAMB_NUM_TIMBRELS;
    setDecays(TAMB_SOUND_DECAY,TAMB_SYSTEM_DECAY);
    defDecays[inst] = TAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 3;
    baseGain = TAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp*TAMB_SHELL_GAIN;
    gains[1]=temp*0.8;
    gains[2]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 1;
    freqalloc[2] = 1;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.05;
    freq_rand[2] = 0.05;
    setFreqAndReson(0,TAMB_SHELL_FREQ,TAMB_SHELL_RESON);
    setFreqAndReson(1,TAMB_CYMB_FREQ1,TAMB_CYMB_RESON);
    setFreqAndReson(2,TAMB_CYMB_FREQ2,TAMB_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 7) { // Sleighbell
    rv = inst;
    nObjects = SLEI_NUM_BELLS;
    defObjs[inst] = SLEI_NUM_BELLS;
    setDecays(SLEI_SOUND_DECAY,SLEI_SYSTEM_DECAY);
    defDecays[inst] = SLEI_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 5;
    baseGain = SLEI_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp*0.5;
    gains[4]=temp*0.3;
    for (i=0;i<nFreqs;i++)	{
	    freqalloc[i] = 1;
	    freq_rand[i] = 0.03;
    }
    setFreqAndReson(0,SLEI_CYMB_FREQ0,SLEI_CYMB_RESON);
    setFreqAndReson(1,SLEI_CYMB_FREQ1,SLEI_CYMB_RESON);
    setFreqAndReson(2,SLEI_CYMB_FREQ2,SLEI_CYMB_RESON);
    setFreqAndReson(3,SLEI_CYMB_FREQ3,SLEI_CYMB_RESON);
    setFreqAndReson(4,SLEI_CYMB_FREQ4,SLEI_CYMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 8) { // Stix1
    rv = inst;
    nObjects = STIX1_NUM_BEANS;
    defObjs[inst] = STIX1_NUM_BEANS;
    setDecays(STIX1_SOUND_DECAY,STIX1_SYSTEM_DECAY);
    defDecays[inst] = STIX1_SYSTEM_DECAY;

    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = STIX1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,STIX1_CENTER_FREQ,STIX1_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 9) { // Crunch1
    rv = inst;
    nObjects = CRUNCH1_NUM_BEADS;
    defObjs[inst] = CRUNCH1_NUM_BEADS;
    setDecays(CRUNCH1_SOUND_DECAY,CRUNCH1_SYSTEM_DECAY);
    defDecays[inst] = CRUNCH1_SYSTEM_DECAY;
    decayScale[inst] = 0.96;
    nFreqs = 1;
    baseGain = CRUNCH1_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CRUNCH1_CENTER_FREQ,CRUNCH1_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst == 10) { // Wrench
    rv = inst;
    nObjects = WRENCH_NUM_PARTS;
    defObjs[inst] = WRENCH_NUM_PARTS;
    setDecays(WRENCH_SOUND_DECAY,1.0);
    defDecays[inst] = 0.9999;
    decayScale[inst] = 0.98;
    nFreqs = 2;
    baseGain = WRENCH_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    setFreqAndReson(0,WRENCH_FREQ,WRENCH_RESON);
    setFreqAndReson(1,WRENCH_FREQ2,WRENCH_RESON2);
    ratchet = 0;
    ratchetPos = 10;
  }
  else if (inst == 11) { // Sandpapr
    rv = inst;
    nObjects = SANDPAPR_NUM_GRAINS;
    defObjs[inst] = SANDPAPR_NUM_GRAINS;
    this->setDecays(SANDPAPR_SOUND_DECAY,SANDPAPR_SYSTEM_DECAY);
    defDecays[inst] = SANDPAPR_SYSTEM_DECAY;
    decayScale[inst] = 0.97;
    nFreqs = 1;
    baseGain = SANDPAPR_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SANDPAPR_CENTER_FREQ,SANDPAPR_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst == 12) { // Cokecan
    rv = inst;
    nObjects = COKECAN_NUM_PARTS;
    defObjs[inst] = COKECAN_NUM_PARTS;
    setDecays(COKECAN_SOUND_DECAY,COKECAN_SYSTEM_DECAY);
    defDecays[inst] = COKECAN_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 5;
    baseGain = COKECAN_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*1.8;
    gains[2]=temp*1.8;
    gains[3]=temp*1.8;
    gains[4]=temp*1.8;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    setFreqAndReson(0,COKECAN_HELMFREQ,COKECAN_HELM_RES);
    setFreqAndReson(1,COKECAN_METLFREQ0,COKECAN_METL_RES);
    setFreqAndReson(2,COKECAN_METLFREQ1,COKECAN_METL_RES);
    setFreqAndReson(3,COKECAN_METLFREQ2,COKECAN_METL_RES);
    setFreqAndReson(4,COKECAN_METLFREQ3,COKECAN_METL_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst>12 && inst<20) { // Nextmug
    rv = inst;
    nObjects = NEXTMUG_NUM_PARTS;
    defObjs[inst] = NEXTMUG_NUM_PARTS;
    setDecays(NEXTMUG_SOUND_DECAY,NEXTMUG_SYSTEM_DECAY);
    defDecays[inst] = NEXTMUG_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    nFreqs = 4;
    baseGain = NEXTMUG_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp*0.8;
    gains[2]=temp*0.6;
    gains[3]=temp*0.4;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    setFreqAndReson(0,NEXTMUG_FREQ0,NEXTMUG_RES);
    setFreqAndReson(1,NEXTMUG_FREQ1,NEXTMUG_RES);
    setFreqAndReson(2,NEXTMUG_FREQ2,NEXTMUG_RES);
    setFreqAndReson(3,NEXTMUG_FREQ3,NEXTMUG_RES);
    setFinalZs(1.0,0.0,-1.0);

    if (inst == 14) { // Mug + Penny
      nFreqs = 7;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,PENNY_FREQ0,PENNY_RES);
      setFreqAndReson(5,PENNY_FREQ1,PENNY_RES);
      setFreqAndReson(6,PENNY_FREQ2,PENNY_RES);
    }
    else if (inst == 15) { // Mug + Nickel
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,NICKEL_FREQ0,NICKEL_RES);
      setFreqAndReson(5,NICKEL_FREQ1,NICKEL_RES);
      setFreqAndReson(6,NICKEL_FREQ2,NICKEL_RES);
    }
    else if (inst == 16) { // Mug + Dime
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*0.8;
      gains[6] = temp*0.5;
      setFreqAndReson(4,DIME_FREQ0,DIME_RES);
      setFreqAndReson(5,DIME_FREQ1,DIME_RES);
      setFreqAndReson(6,DIME_FREQ2,DIME_RES);
    }
    else if (inst == 17) { // Mug + Quarter
      nFreqs = 6;
      gains[4] = temp*1.3;
      gains[5] = temp*1.0;
      gains[6] = temp*0.8;
      setFreqAndReson(4,QUARTER_FREQ0,QUARTER_RES);
      setFreqAndReson(5,QUARTER_FREQ1,QUARTER_RES);
      setFreqAndReson(6,QUARTER_FREQ2,QUARTER_RES);
    }
    else if (inst == 18) { // Mug + Franc
      nFreqs = 6;
      gains[4] = temp*0.7;
      gains[5] = temp*0.4;
      gains[6] = temp*0.3;
      setFreqAndReson(4,FRANC_FREQ0,FRANC_RES);
      setFreqAndReson(5,FRANC_FREQ1,FRANC_RES);
      setFreqAndReson(6,FRANC_FREQ2,FRANC_RES);
    }
    else if (inst == 19) { // Mug + Peso
      nFreqs = 6;
      gains[4] = temp;
      gains[5] = temp*1.2;
      gains[6] = temp*0.7;
      setFreqAndReson(4,PESO_FREQ0,PESO_RES);
      setFreqAndReson(5,PESO_FREQ1,PESO_RES);
      setFreqAndReson(6,PESO_FREQ2,PESO_RES);
    }
  }
  else if (inst == 20) { // Big Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = BIGROCKS_NUM_PARTS;
    defObjs[inst] = BIGROCKS_NUM_PARTS;
    setDecays(BIGROCKS_SOUND_DECAY,BIGROCKS_SYSTEM_DECAY);
    defDecays[inst] = BIGROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = BIGROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.11;
    setFreqAndReson(0,BIGROCKS_FREQ,BIGROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 21) { // Little Rocks
    nFreqs = 1;
    rv = inst;
    nObjects = LITLROCKS_NUM_PARTS;
    defObjs[inst] = LITLROCKS_NUM_PARTS;
    setDecays(LITLROCKS_SOUND_DECAY,LITLROCKS_SYSTEM_DECAY);
    defDecays[inst] = LITLROCKS_SYSTEM_DECAY;
    decayScale[inst] = 0.95;
    baseGain = LITLROCKS_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 1;
    freq_rand[0] = 0.18;
    setFreqAndReson(0,LITLROCKS_FREQ,LITLROCKS_RES);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst == 22) { // Tuned Bamboo
    rv = inst;
    nObjects = TBAMB_NUM_TUBES;
    defObjs[inst] = TBAMB_NUM_TUBES;
    setDecays(TBAMB_SOUND_DECAY, TBAMB_SYSTEM_DECAY);
    defDecays[inst] = TBAMB_SYSTEM_DECAY;
    decayScale[inst] = 0.7;
    nFreqs = 7;
    baseGain = TBAMB_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp;
    gains[4]=temp;
    gains[5]=temp;
    gains[6]=temp;
    freqalloc[0] = 0;
    freqalloc[1] = 0;
    freqalloc[2] = 0;
    freqalloc[3] = 0;
    freqalloc[4] = 0;
    freqalloc[5] = 0;
    freqalloc[6] = 0;
    freq_rand[0] = 0.0;
    freq_rand[1] = 0.0;
    freq_rand[2] = 0.0;
    freq_rand[3] = 0.0;
    freq_rand[4] = 0.0;
    freq_rand[5] = 0.0;
    freq_rand[6] = 0.0;
    setFreqAndReson(0,TBAMB_CENTER_FREQ0,TBAMB_RESON);
    setFreqAndReson(1,TBAMB_CENTER_FREQ1,TBAMB_RESON);
    setFreqAndReson(2,TBAMB_CENTER_FREQ2,TBAMB_RESON);
    setFreqAndReson(3,TBAMB_CENTER_FREQ3,TBAMB_RESON);
    setFreqAndReson(4,TBAMB_CENTER_FREQ4,TBAMB_RESON);
    setFreqAndReson(5,TBAMB_CENTER_FREQ5,TBAMB_RESON);
    setFreqAndReson(6,TBAMB_CENTER_FREQ6,TBAMB_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else { // Maraca (inst == 0) or default
    rv = 0;
    nObjects = MARA_NUM_BEANS;
    defObjs[0] = MARA_NUM_BEANS;
    setDecays(MARA_SOUND_DECAY,MARA_SYSTEM_DECAY);
    defDecays[0] = MARA_SYSTEM_DECAY;
    decayScale[inst] = 0.9;
    nFreqs = 1;
    baseGain = MARA_GAIN;
    temp = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,MARA_CENTER_FREQ,MARA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  return rv;
}

void Shakers :: noteOn(MY_FLOAT frequency, MY_FLOAT amplitude)
{
  // Yep ... pretty kludgey, but it works!
  int noteNum = (int) ((12*log(frequency/220)/log(2)) + 57.01) % 32;
  if (instType !=  noteNum) instType = this->setupNum(noteNum);
  shakeEnergy += amplitude * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;

#if defined(_STK_DEBUG_)
  cerr << "Shakers: NoteOn frequency = " << frequency << ", amplitude = " << amplitude << endl;
#endif
}

void Shakers :: noteOff(MY_FLOAT amplitude)
{
  shakeEnergy = 0.0;
  if (instType==10 || instType==3) ratchetPos = 0;
}

#define MIN_ENERGY 0.3

MY_FLOAT Shakers :: tick()
{
  MY_FLOAT data;
  MY_FLOAT temp_rand;
  int i;

  if (instType == 4) {
  	if (shakeEnergy > MIN_ENERGY)	{
      lastOutput = wuter_tick();
      lastOutput *= 0.0001;
    }
    else {
      lastOutput = 0.0;
    }
  }
  else if (instType == 22) {
    lastOutput = tbamb_tick();
  }
  else if (instType == 10 || instType == 3) {
    if (ratchetPos > 0) {
      ratchet -= (ratchetDelta + (0.002*totalEnergy));
      if (ratchet < 0.0) {
        ratchet = 1.0;
        ratchetPos -= 1;
	    }
      totalEnergy = ratchet;
      lastOutput = ratchet_tick();
      lastOutput *= 0.0001;
    }
    else lastOutput = 0.0;
  }
  else  {
    //  MY_FLOAT generic_tick()	{
    if (shakeEnergy > MIN_ENERGY) {
      shakeEnergy *= systemDecay;               // Exponential system decay
      if (float_random(1024.0) < nObjects) {
        sndLevel += shakeEnergy;   
        for (i=0;i<nFreqs;i++) {
          if (freqalloc[i])	{
            temp_rand = t_center_freqs[i] * (1.0 + (freq_rand[i] * noise_tick()));
            coeffs[i][0] = -resons[i] * 2.0 * cos(temp_rand * TWO_PI / Stk::sampleRate());
          }
        }
    	}
      inputs[0] = sndLevel * noise_tick();      // Actual Sound is Random
      for (i=1; i<nFreqs; i++)	{
        inputs[i] = inputs[0];
      }
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)	{
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)	data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      lastOutput = data * 0.0001;
    }
    else lastOutput = 0.0;
  }

  return lastOutput;
}

void Shakers :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT norm = value * ONE_OVER_128;
  if ( norm < 0 ) {
    norm = 0.0;
    cerr << "Shakers: Control value less than zero!" << endl;
  }
  else if ( norm > 1.0 ) {
    norm = 1.0;
    cerr << "Shakers: Control value greater than 128.0!" << endl;
  }

  MY_FLOAT temp;
  int i;

  if (number == __SK_Breath_) { // 2 ... energy
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3) {
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else if (number == __SK_ModFrequency_) { // 4 ... decay
    if (instType != 3 && instType != 10) {
      systemDecay = defDecays[instType] + ((value - 64.0) *
                                           decayScale[instType] *
                                           (1.0 - defDecays[instType]) / 64.0 );
      gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
      for (i=1;i<nFreqs;i++) gains[i] = gains[0];
      if (instType == 6) { // tambourine
        gains[0] *= TAMB_SHELL_GAIN;
        gains[1] *= 0.8;
      }
      else if (instType == 7) { // sleighbell
        gains[3] *= 0.5;
        gains[4] *= 0.3;
      }
      else if (instType == 12) { // cokecan
        for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
      }
      for (i=0;i<nFreqs;i++) gains[i] *= ((128-value)/100.0 + 0.36);
    }
  }
  else if (number == __SK_FootControl_) { // 11 ... number of objects
    if (instType == 5) // bamboo
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 0.3;
    else
      nObjects = (MY_FLOAT) (value * defObjs[instType] / 64.0) + 1.1;
    gains[0] = log(nObjects) * baseGain / (MY_FLOAT) nObjects;
    for (i=1;i<nFreqs;i++) gains[i] = gains[0];
    if (instType == 6) { // tambourine
      gains[0] *= TAMB_SHELL_GAIN;
      gains[1] *= 0.8;
    }
    else if (instType == 7) { // sleighbell
      gains[3] *= 0.5;
      gains[4] *= 0.3;
    }
    else if (instType == 12) { // cokecan
      for (i=1;i<nFreqs;i++) gains[i] *= 1.8;
    }
    if (instType != 3 && instType != 10) {
    // reverse calculate decay setting
    double temp = (double) (64.0 * (systemDecay-defDecays[instType])/(decayScale[instType]*(1-defDecays[instType])) + 64.0);
    // scale gains by decay setting
    for (i=0;i<nFreqs;i++) gains[i] *= ((128-temp)/100.0 + 0.36);
    }
  }
  else if (number == __SK_ModWheel_) { // 1 ... resonance frequency
    for (i=0; i<nFreqs; i++)	{
      if (instType == 6 || instType == 2 || instType == 7) // limit range a bit for tambourine
        temp = center_freqs[i] * pow (1.008,value-64);
      else
        temp = center_freqs[i] * pow (1.015,value-64);
      t_center_freqs[i] = temp;

      coeffs[i][0] = -resons[i] * 2.0 * cos(temp * TWO_PI / Stk::sampleRate());
      coeffs[i][1] = resons[i]*resons[i];
    }
  }
  else if (number == __SK_AfterTouch_Cont_) { // 128
    shakeEnergy += norm * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3)	{
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else  if (number == __SK_ShakerInst_) { // 1071
    instType = (int) (value + 0.5);	//  Just to be safe
    this->setupNum(instType);
  }                                       
  else
    cerr << "Shakers: Undefined Control Number (" << number << ")!!" << endl;

#if defined(_STK_DEBUG_)
  cerr << "Shakers: controlChange number = " << number << ", value = " << value << endl;
#endif
}

// KLUDGE-O-MATIC-O-RAMA

MY_FLOAT Shakers :: wuter_tick() {
  MY_FLOAT data;
  int j;
  shakeEnergy *= systemDecay;               // Exponential system decay
  if (my_random(32767) < nObjects) {     
    sndLevel = shakeEnergy;   
    j = my_random(3);
	  if (j == 0)   {
      center_freqs[0] = WUTR_CENTER_FREQ1 * (0.75 + (0.25 * noise_tick()));
	    gains[0] = fabs(noise_tick());
	  }
	  else if (j == 1)      {
      center_freqs[1] = WUTR_CENTER_FREQ1 * (1.0 + (0.25 * noise_tick()));
	    gains[1] = fabs(noise_tick());
	  }
	  else  {
      center_freqs[2] = WUTR_CENTER_FREQ1 * (1.25 + (0.25 * noise_tick()));
	    gains[2] = fabs(noise_tick());
	  }
	}
	
  gains[0] *= resons[0];
  if (gains[0] >  0.001) {
    center_freqs[0]  *= WUTR_FREQ_SWEEP;
    coeffs[0][0] = -resons[0] * 2.0 * 
      cos(center_freqs[0] * TWO_PI / Stk::sampleRate());
  }
  gains[1] *= resons[1];
  if (gains[1] > 0.001) {
    center_freqs[1] *= WUTR_FREQ_SWEEP;
    coeffs[1][0] = -resons[1] * 2.0 * 
      cos(center_freqs[1] * TWO_PI / Stk::sampleRate());
  }
  gains[2] *= resons[2];
  if (gains[2] > 0.001) {
    center_freqs[2] *= WUTR_FREQ_SWEEP;
    coeffs[2][0] = -resons[2] * 2.0 * 
      cos(center_freqs[2] * TWO_PI / Stk::sampleRate());
  }
	
  sndLevel *= soundDecay;        // Each (all) event(s) 
                                 // decay(s) exponentially 
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick();     // Actual Sound is Random
  inputs[1] = inputs[0] * gains[1];
  inputs[2] = inputs[0] * gains[2];
  inputs[0] *= gains[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  data = gains[0]*outputs[0][0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
  data += gains[1]*outputs[1][0];
  inputs[2] -= outputs[2][0]*coeffs[2][0];
  inputs[2] -= outputs[2][1]*coeffs[2][1];
  outputs[2][1] = outputs[2][0];
  outputs[2][0] = inputs[2];
  data += gains[2]*outputs[2][0];
 
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data * 4;

  data = finalZ[2] - finalZ[0];
  return data;
}

MY_FLOAT Shakers :: ratchet_tick() {
  MY_FLOAT data;
  if (my_random(1024) < nObjects) {
    sndLevel += 512 * ratchet * totalEnergy;
  }
  inputs[0] = sndLevel;
  inputs[0] *= noise_tick() * ratchet;
  sndLevel *= soundDecay;
		 
  inputs[1] = inputs[0];
  inputs[0] -= outputs[0][0]*coeffs[0][0];
  inputs[0] -= outputs[0][1]*coeffs[0][1];
  outputs[0][1] = outputs[0][0];
  outputs[0][0] = inputs[0];
  inputs[1] -= outputs[1][0]*coeffs[1][0];
  inputs[1] -= outputs[1][1]*coeffs[1][1];
  outputs[1][1] = outputs[1][0];
  outputs[1][0] = inputs[1];
     
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = gains[0]*outputs[0][1] + gains[1]*outputs[1][1];
  data = finalZ[0] - finalZ[2];
  return data;
}

MY_FLOAT Shakers :: tbamb_tick() {
  MY_FLOAT data, temp;
  static int which = 0;
  int i;

  if (shakeEnergy > MIN_ENERGY)	{
      shakeEnergy *= systemDecay;    // Exponential system decay
      if (float_random(1024.0) < nObjects) {
	    sndLevel += shakeEnergy;
	    which = my_random(7);
	  }  
      temp = sndLevel * noise_tick();      // Actual Sound is Random
	  for (i=0;i<nFreqs;i++)	inputs[i] = 0;
	  inputs[which] = temp;
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<nFreqs;i++)	{
        inputs[i] -= outputs[i][0]*coeffs[i][0];  // Do
        inputs[i] -= outputs[i][1]*coeffs[i][1];  // resonant
        outputs[i][1] = outputs[i][0];            // filter
        outputs[i][0] = inputs[i];                // calculations
        finalZ[0] += gains[i] * outputs[i][1];
      }
      data = finalZCoeffs[0] * finalZ[0];     // Extra zero(s) for shape
      data += finalZCoeffs[1] * finalZ[1];    // Extra zero(s) for shape
      data += finalZCoeffs[2] * finalZ[2];    // Extra zero(s) for shape
      if (data > 10000.0)	data = 10000.0;
      if (data < -10000.0) data = -10000.0;
      data = data * 0.0001;
  }
  else data = 0.0;
  return data;
}
