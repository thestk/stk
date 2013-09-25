/**********************************************************/
/*  PhISEM (Physically Informed Stochastic Event Modeling */
/*    by Perry R. Cook, Princeton, February 1997          */
/*                                                        */
/*  Meta-model that simulates all of:                     */
/*  Maraca Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Sekere Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Cabasa Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Bamboo Windchime Simulation, by Perry R. Cook, 1996-7 */
/*  Water Drops Simulation, by Perry R. Cook, 1996-7      */
/*  Tambourine Simulation, by Perry R. Cook, 1996-7       */
/*  Sleighbells Simulation, by Perry R. Cook, 1996-7      */
/*  Guiro Simulation, by Perry R. Cook, 1996-7            */
/*                                                        */
/**********************************************************/
/*  PhOLIES (Physically-Oriented Library of               */
/*    Imitated Environmental Sounds), Perry Cook, 1997-9  */
/*                                                        */
/*  Stix1 (walking on brittle sticks)                     */
/*  Crunch1 (like new fallen snow, or not)                */
/*  Wrench (basic socket wrench, friend of guiro)         */
/*  Sandpapr (sandpaper)                                  */
/**********************************************************/

#include "Object.h"
#ifdef __OS_NeXT_
  #include <libc.h>
#endif

int my_random(int max) //  Return Random Int Between 0 and max
{
  long temp;
#if defined(__OS_Win_) /* For Windoze */
  temp = (long) rand();
#else /* This is for unix */
  temp = random() >> 16;
#endif
  temp = temp * (long) max;
  temp = temp >> 15;
  return (int) temp; 
}

MY_FLOAT float_random(MY_FLOAT max) // Return random float between 0.0 and max
{	
  MY_FLOAT temp;
  temp = (MY_FLOAT) my_random(32767);
  temp = temp * 0.0000305185;
  temp = temp * max;
  return temp;	
}

MY_FLOAT noise_tick() //  Return random MY_FLOAT float between -1.0 and 1.0
{
  MY_FLOAT temp;
  temp = my_random(32767) - 16384;
  temp *= 0.0000610352;
  return temp;
}

/************************* MARACA *****************************/
#define MARA_SOUND_DECAY 0.95
#define MARA_SYSTEM_DECAY 0.999
#define MARA_GAIN 25.0
#define MARA_NUM_BEANS 25
#define MARA_CENTER_FREQ 3200.0
#define MARA_RESON 0.96
/***********************  SEKERE *****************************/
#define SEKE_SOUND_DECAY 0.96
#define SEKE_SYSTEM_DECAY 0.999
#define SEKE_GAIN 30.0
#define SEKE_NUM_BEANS 64
#define SEKE_CENTER_FREQ 5500.0
#define SEKE_RESON 0.6
/***********************  SANDPAPER **************************/
#define SANDPAPR_SOUND_DECAY 0.999
#define SANDPAPR_SYSTEM_DECAY 0.999
#define SANDPAPR_GAIN 1.0
#define SANDPAPR_NUM_GRAINS 128
#define SANDPAPR_CENTER_FREQ 4500.0
#define SANDPAPR_RESON 0.6
/************************ CABASA *****************************/
#define CABA_SOUND_DECAY 0.96
#define CABA_SYSTEM_DECAY 0.997
#define CABA_GAIN 150.0
#define CABA_NUM_BEADS 512
#define CABA_CENTER_FREQ 3000.0
#define CABA_RESON 0.7
/************************ Bamboo Wind Chimes *****************/
#define BAMB_SOUND_DECAY 0.95
#define BAMB_SYSTEM_DECAY 0.99995
#define BAMB_GAIN 2.0
#define BAMB_NUM_TUBES 1.25
#define BAMB_CENTER_FREQ0  2800.0
#define BAMB_CENTER_FREQ1  0.8 * 2800.0
#define BAMB_CENTER_FREQ2  1.2 * 2800.0
#define BAMB_RESON	   0.995
/******************* Water Drops  ****************************/
#define WUTR_SOUND_DECAY 0.95
#define WUTR_SYSTEM_DECAY 0.999
#define WUTR_GAIN 1.0
#define WUTR_NUM_SOURCES 4
#define WUTR_CENTER_FREQ0  450.0
#define WUTR_CENTER_FREQ1  600.0
#define WUTR_CENTER_FREQ2  750.0 
#define WUTR_RESON   0.9985
#define WUTR_FREQ_SWEEP  1.0001
/****************** TAMBOURINE  *****************************/
#define TAMB_SOUND_DECAY 0.95
#define TAMB_SYSTEM_DECAY 0.9985
#define TAMB_GAIN 10.0
#define TAMB_NUM_TIMBRELS 32
#define TAMB_SHELL_FREQ 2300
#define TAMB_SHELL_GAIN 0.1
#define TAMB_SHELL_RESON 0.96
#define TAMB_CYMB_FREQ1  5600
#define TAMB_CYMB_FREQ2 8100
#define TAMB_CYMB_RESON 0.99
/********************** SLEIGHBELLS *************************/
#define SLEI_SOUND_DECAY 0.97
#define SLEI_SYSTEM_DECAY 0.9994
#define SLEI_GAIN 2.0
#define SLEI_NUM_BELLS 32
#define SLEI_CYMB_FREQ0 2500
#define SLEI_CYMB_FREQ1 5300
#define SLEI_CYMB_FREQ2 6500
#define SLEI_CYMB_FREQ3 8300
#define SLEI_CYMB_FREQ4 9800
#define SLEI_CYMB_RESON 0.99 
/***************************  GUIRO  ***********************/
#define GUIR_SOUND_DECAY 0.95
#define GUIR_GAIN 10.0
#define GUIR_NUM_PARTS 128
#define GUIR_GOURD_FREQ  2500.0
#define GUIR_GOURD_RESON 0.97
#define GUIR_GOURD_FREQ2  4000.0
#define GUIR_GOURD_RESON2 0.97
/**************************  WRENCH  ***********************/
#define WRENCH_SOUND_DECAY 0.95
#define WRENCH_GAIN 5
#define WRENCH_NUM_PARTS 128
#define WRENCH_FREQ  3200.0
#define WRENCH_RESON 0.99
#define WRENCH_FREQ2  8000.0
#define WRENCH_RESON2 0.992
/************************ COKECAN **************************/
#define COKECAN_SOUND_DECAY 0.97
#define COKECAN_SYSTEM_DECAY 0.999
#define COKECAN_GAIN 1.0
#define COKECAN_NUM_PARTS 48
#define COKECAN_HELMFREQ 370
#define COKECAN_HELM_RES  0.99
#define COKECAN_METLFREQ0 1025
#define COKECAN_METLFREQ1 1424
#define COKECAN_METLFREQ2 2149
#define COKECAN_METLFREQ3 3596
#define COKECAN_METL_RES 0.992 
/************************************************************/
/*  PhOLIES (Physically-Oriented Library of                 */
/*    Imitated Environmental Sounds), Perry Cook, 1997-8    */
/************************************************************/

/***********************  STIX1 *****************************/
#define STIX1_SOUND_DECAY 0.96
#define STIX1_SYSTEM_DECAY 0.998
#define STIX1_GAIN 30.0
#define STIX1_NUM_BEANS 2
#define STIX1_CENTER_FREQ 5500.0
#define STIX1_RESON 0.6
/************************ Crunch1 ***************************/
#define CRUNCH1_SOUND_DECAY 0.95
#define CRUNCH1_SYSTEM_DECAY 0.99806
#define CRUNCH1_GAIN 30.0
#define CRUNCH1_NUM_BEADS 7
#define CRUNCH1_CENTER_FREQ 800.0
#define CRUNCH1_RESON 0.95

/************  THE ACTUAL CLASS ITSELF  *********************/

#include "Shakers.h"
#include "SKINI11.msg"

Shakers :: Shakers() : Instrmnt()
{
  int i;

  instType = 0;
  shakeEnergy = 0.0;
  num_freqs = 0;
  sndLevel = 0.0;

  for (i=0;i<MAX_FREQS;i++)	{
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
  num_objects = 0.0;
  collLikely = 0.0;
  totalEnergy = 0.0;
  ratchet=0.0;
  ratchetDelta=0.0005;
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

#define NUM_INST 13

char instrs[NUM_INST][16] = 
		{"Maraca", "Cabasa", "Sekere", "Guiro",
		"Waterdrp", "Bamboo", "Tambourn", "Sleighbl", 
		"Stix1", "Crunch1", "Wrench", "SandPapr", "CokeCan"}; 

int Shakers :: setupName(char* instr)
{
  int i, which = 0;

  for (i=0;i<NUM_INST;i++)	{
    if (!strcmp(instr,instrs[i]))
	    which = i;
  }
#if defined(_debug_)
  printf("Shakers: Setting Instrument to %s\n",instrs[which]);
#endif
  return this->setupNum(which);
}

void Shakers :: setFinalZs(MY_FLOAT z0, MY_FLOAT z1, MY_FLOAT z2)	{
  finalZCoeffs[0] = z0;
  finalZCoeffs[1] = z1;
  finalZCoeffs[2] = z2;
}

void Shakers :: setDecays(MY_FLOAT sndDecay, MY_FLOAT sysDecay)	{
  soundDecay = sndDecay;
  systemDecay = sysDecay;
}

int Shakers :: setFreqAndReson(int which, MY_FLOAT freq, MY_FLOAT reson)	{
  if (which < MAX_FREQS)	{
    resons[which] = reson;
    center_freqs[which] = freq;
    t_center_freqs[which] = freq;
    coeffs[which][1] = reson * reson;
    coeffs[which][0] = -reson * 2.0 * cos(freq * TWO_PI / SRATE);
    return 1;
  }
  else return 0;
}

int Shakers :: setupNum(int inst)
{
  int i;
  MY_FLOAT temp;

  if (inst==1) {	//  cabasa_setup();
    num_objects = CABA_NUM_BEADS;
    setDecays(CABA_SOUND_DECAY, CABA_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = CABA_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0] = temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CABA_CENTER_FREQ,CABA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst==2) {	//  sekere_setup();
    num_objects = SEKE_NUM_BEANS;
    this->setDecays(SEKE_SOUND_DECAY,SEKE_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = SEKE_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SEKE_CENTER_FREQ,SEKE_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst==3) {	//  guiro_setup();
    num_objects = GUIR_NUM_PARTS;
    setDecays(GUIR_SOUND_DECAY,1.0);
    num_freqs = 2;
    baseGain = GUIR_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else if (inst==4) {	//  wuter_setup();
    num_objects = WUTR_NUM_SOURCES;
    setDecays(WUTR_SOUND_DECAY,WUTR_SYSTEM_DECAY);
    num_freqs = 3;
    baseGain = WUTR_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else if (inst==5) {	//  bamboo_setup();
    num_objects = BAMB_NUM_TUBES;
    setDecays(BAMB_SOUND_DECAY, BAMB_SYSTEM_DECAY);
    num_freqs = 3;
    baseGain = BAMB_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else if (inst==6) {	//  tambourine_setup();
    num_objects = TAMB_NUM_TIMBRELS;
    setDecays(TAMB_SOUND_DECAY,TAMB_SYSTEM_DECAY);
    num_freqs = 3;
    baseGain = TAMB_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else if (inst==7) {	//  sleighbell_setup();
    num_objects = SLEI_NUM_BELLS;
    setDecays(SLEI_SOUND_DECAY,SLEI_SYSTEM_DECAY);
    num_freqs = 5;
    baseGain = SLEI_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0]=temp;
    gains[1]=temp;
    gains[2]=temp;
    gains[3]=temp*0.5;
    gains[4]=temp*0.3;
    for (i=0;i<num_freqs;i++)	{
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
  else if (inst==8) {	//  stix1_setup();
    num_objects = STIX1_NUM_BEANS;
    setDecays(STIX1_SOUND_DECAY,STIX1_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = STIX1_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,STIX1_CENTER_FREQ,STIX1_RESON);
    setFinalZs(1.0,0.0,-1.0);
  }
  else if (inst==9) {	//  crunch1_setup();
    num_objects = CRUNCH1_NUM_BEADS;
    setDecays(CRUNCH1_SOUND_DECAY,CRUNCH1_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = CRUNCH1_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,CRUNCH1_CENTER_FREQ,CRUNCH1_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  else if (inst==10) {	//  wrench_setup();
    num_objects = WRENCH_NUM_PARTS;
    setDecays(WRENCH_SOUND_DECAY,1.0);
    num_freqs = 2;
    baseGain = WRENCH_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else if (inst==11) {	//  sandpapr_setup();
    num_objects = SANDPAPR_NUM_GRAINS;
    this->setDecays(SANDPAPR_SOUND_DECAY,SANDPAPR_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = SANDPAPR_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0] = temp;
    freqalloc[0] = 0;
    this->setFreqAndReson(0,SANDPAPR_CENTER_FREQ,SANDPAPR_RESON);
    this->setFinalZs(1.0, 0.0, -1.0);
  }
  else if (inst==12) {	//  cokecan_setup();
    num_objects = COKECAN_NUM_PARTS;
    setDecays(COKECAN_SOUND_DECAY,COKECAN_SYSTEM_DECAY);
    num_freqs = 5;
    baseGain = COKECAN_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
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
  else { //  maraca_setup();   inst == 0 or other
    num_objects = MARA_NUM_BEANS;
    setDecays(MARA_SOUND_DECAY,MARA_SYSTEM_DECAY);
    num_freqs = 1;
    baseGain = MARA_GAIN;
    temp = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
    gains[0]=temp;
    freqalloc[0] = 0;
    setFreqAndReson(0,MARA_CENTER_FREQ,MARA_RESON);
    setFinalZs(1.0,-1.0,0.0);
  }
  return inst;
}

void Shakers :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  // Yep ... pretty kludgey, but it works!
  int noteNum = (int) ((12*log(freq/220)/log(2)) + 57.01) % 32;
  if (instType !=  noteNum) instType = this->setupNum(noteNum);
  shakeEnergy = amp * MAX_SHAKE * 0.1;
  if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  if (instType==10 || instType==3) ratchetPos += 1;
#if defined(_debug_)        
	printf("Shakers : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif
}

void Shakers :: noteOff(MY_FLOAT amp)
{
  shakeEnergy = 0.0;
}

#define MIN_ENERGY 0.3

MY_FLOAT Shakers :: tick()
{
  MY_FLOAT data;
  MY_FLOAT temp_rand;
  int i;

  if (instType==4) {
  	if (shakeEnergy > MIN_ENERGY)	{
      lastOutput = wuter_tick();
    }
    else {
      lastOutput = 0.0;
    }
  }
  else if (instType==10 || instType==3) {
    if (ratchetPos > 0) {
      ratchet -= (ratchetDelta + (0.002*totalEnergy));
      if (ratchet < 0.0) {
        ratchet = 1.0;
        ratchetPos -= 1;
	    }
      totalEnergy = ratchet;
      lastOutput = ratchet_tick();
    }
    else lastOutput = 0.0;
  }
  else  {
    //  MY_FLOAT generic_tick()	{
    if (shakeEnergy > MIN_ENERGY)	{
      shakeEnergy *= systemDecay;               // Exponential system decay
      if (float_random(1024.0) < num_objects) {
        sndLevel += shakeEnergy;   
        for (i=0;i<num_freqs;i++)	{
          if (freqalloc[i])	{
            temp_rand = t_center_freqs[i] * (1.0 + (freq_rand[i] * noise_tick()));
            coeffs[i][0] = -resons[i] * 2.0 * cos(temp_rand * TWO_PI / SRATE);
          }
        }
    	}
      inputs[0] = sndLevel * noise_tick();      // Actual Sound is Random
      for (i=1; i<num_freqs; i++)	{
        inputs[i] = inputs[0];
      }
      sndLevel *= soundDecay;                   // Exponential Sound decay 
      finalZ[2] = finalZ[1];
      finalZ[1] = finalZ[0];
      finalZ[0] = 0;
      for (i=0;i<num_freqs;i++)	{
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
      lastOutput = data;
    }
    else lastOutput = 0.0;
  }

  lastOutput *= 0.0001;

  return lastOutput;
}

void Shakers :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT temp;
  int i;
#if defined(_debug_)        
  printf("Shakers : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_Breath_) { // control_change #2
#if defined(_debug_)        
    printf("shaking \n");
#endif    
    shakeEnergy += value * NORM_7 * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3) {
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else if (number == __SK_FootControl_) { // control_change #4
#if defined(_debug_)        
    printf("setting decay\n");
#endif    
    systemDecay = 0.998 + (value * NORM_7 * 0.002);
  }
  else if (number == __SK_ModFrequency_) { // control_change #11
#if defined(_debug_)        
    printf("setting number of objects\n");
#endif
    if (instType==5) num_objects = (MY_FLOAT) (value/32.0) + 1;
    else num_objects = (MY_FLOAT) value + 1;
    gains[0] = log(num_objects) * baseGain / (MY_FLOAT) num_objects;
  }
  else if (number == __SK_ModWheel_) { // control_change #1
    for (i=0;i<num_freqs;i++)	{
      temp = center_freqs[i] * pow (1.015,value-64);
      t_center_freqs[i] = temp;
#if defined(_debug_)        
      printf("setting resonance %i freq. to %f\n",i,temp);
#endif    
      coeffs[i][0] = -resons[i] * 2.0 * cos(temp * TWO_PI / SRATE);
      coeffs[i][1] = resons[i]*resons[i];
    }
  }
  else if (number == __SK_AfterTouch_Cont_) { // control_change #128
#if defined(_debug_)        
    printf("shaking \n");
#endif    
    shakeEnergy += value * NORM_7 * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
    if (instType==10 || instType==3)	{
	    ratchetPos = (int) fabs(value - lastRatchetPos);
	    ratchetDelta = 0.0002 * ratchetPos;
	    lastRatchetPos = (int) value;
    }
  }
  else  if (number == __SK_ShakerInst_) { // control_change #1071
    instType = (int) (value + 0.5);	//  Just to be safe
    this->setupNum(instType);
  }                                       
  else	{
    printf("Shakers : Undefined Control Number!!\n");
  }  
}

/*********************************************************/
/****************   KLUDGE-O-MATIC-O-RAMA   **************/

MY_FLOAT Shakers :: wuter_tick() {
  MY_FLOAT data;
  int j;
  shakeEnergy *= systemDecay;               // Exponential system decay
  if (my_random(32767) < num_objects) {     
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
      cos(center_freqs[0] * TWO_PI / SRATE);
  }
  gains[1] *= resons[1];
  if (gains[1] > 0.001) {
    center_freqs[1] *= WUTR_FREQ_SWEEP;
    coeffs[1][0] = -resons[1] * 2.0 * 
      cos(center_freqs[1] * TWO_PI / SRATE);
  }
  gains[2] *= resons[2];
  if (gains[2] > 0.001) {
    center_freqs[2] *= WUTR_FREQ_SWEEP;
    coeffs[2][0] = -resons[2] * 2.0 * 
      cos(center_freqs[2] * TWO_PI / SRATE);
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
  if (my_random(1024) < num_objects) {
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

