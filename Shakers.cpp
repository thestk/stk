/**********************************************************/
/*  PhISEM (Physically Informed Stochastic Event Modeling */
/*    by Perry R. Cook, Princeton, February 1997          */
/*  Meta-model that simulates all of:                     */
/*  Maraca Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Sekere Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Cabasa Simulation by Perry R. Cook, Princeton, 1996-7 */
/*  Bamboo Windchime Simulation, by Perry R. Cook, 1996-7 */
/*  Water Drops Simulation, by Perry R. Cook, 1996-7      */
/*  Tambourine Simulation, by Perry R. Cook, 1996-7       */
/*  Sleighbells Simulation, by Perry R. Cook, 1996-7      */
/*  Guiro Simulation, by Perry R. Cook, 1996-7            */
/**********************************************************/

#include "Object.h"

#ifdef __NeXT_
  #include <libc.h>
#endif

int my_random(int max)  {   //  Return Random Int Between 0 and max
	unsigned long temp;

#if defined(__OS_Linux_) /* RAND_MAX for Linux is 32 bit */
  temp = (unsigned long) rand() >> 16;
#else                    /* RAND_MAX for everything else is 16 bit */
  temp = (unsigned long) rand();
#endif
	temp *= (unsigned long) max;
	temp >>= 15;
	return (int) temp; 
}

MY_FLOAT noise_tick()   //  Return random MY_FLOAT float between -1.0 and 1.0
{
  MY_FLOAT temp;
  temp = (MY_FLOAT) (my_random(32767) - 16384);
  temp *= 0.0000610351;
  return temp;
}

MY_FLOAT guiroScrape = 0;
MY_FLOAT shakeEnergy = 0.0;
MY_FLOAT input = 0.0,output[2] = {0.0,0.0};
MY_FLOAT coeffs[2];
MY_FLOAT input1 = 0.0,output1[2] = {0.0,0.0};
MY_FLOAT coeffs1[2];
MY_FLOAT input2 = 0.0,output2[2] = {0.0,0.0};
MY_FLOAT coeffs2[2];
MY_FLOAT input3 = 0.0,output3[2] = {0.0, 0.0};
MY_FLOAT coeffs3[2];
MY_FLOAT input4 = 0.0,output4[2] = {0.0, 0.0};
MY_FLOAT coeffs4[2];
MY_FLOAT sndLevel = 0.0;
MY_FLOAT gain = 0.0, gain1 = 0, gain2 = 0;
MY_FLOAT freq_rand = 0.0;
MY_FLOAT soundDecay = 0.0;
MY_FLOAT systemDecay = 0.0;
MY_FLOAT cymb_rand = 0.0;
long   num_objects;
MY_FLOAT freq, freq1, freq2;

MY_FLOAT collLikely,scrapeVel = 0.00015;
MY_FLOAT totalEnergy;
MY_FLOAT ratchet=0.0,ratchetDelta=0.0005;
MY_FLOAT finalZ[3] = {0.0, 0.0, 0.0};

/************************* MARACA *****************************/
#define MARA_SOUND_DECAY 0.95
#define MARA_SYSTEM_DECAY 0.999
#define MARA_NUM_BEANS 25

void maraca_setup() {
  num_objects = MARA_NUM_BEANS;
  gain = log(num_objects) / log(4.0) * 40.0 / (MY_FLOAT) num_objects;
  coeffs[0] = -0.96 * 2.0 * cos(3200.0 * TWO_PI / SRATE);
  coeffs[1] = 0.96*0.96;
  soundDecay = MARA_SOUND_DECAY;
  systemDecay = MARA_SYSTEM_DECAY;
}

MY_FLOAT maraca_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;         // Exponential system decay
  if (my_random(1024) < num_objects)  // If collision
    sndLevel += gain * shakeEnergy;   //   add energy
  input = sndLevel * noise_tick();    // Actual Sound is Random
  sndLevel *= soundDecay;             // Exponential Sound decay 
  input -= output[0]*coeffs[0];       // Do gourd
  input -= output[1]*coeffs[1];       //   resonance
  output[1] = output[0];              //     filter
  output[0] = input;                  //       calculations
  data = output[0] - output[1];       // Extra zero for shape
  return data;
}
	
/***********************  SEKERE *****************************/
#define SEKE_SOUND_DECAY 0.96
#define SEKE_SYSTEM_DECAY 0.999
#define SEKE_NUM_BEANS 64

void sekere_setup() {
  num_objects = SEKE_NUM_BEANS;
  gain = log(num_objects) / log(4.0) * 40.0 / (MY_FLOAT) num_objects;
  coeffs[0] = -0.6 * 2.0 * cos(5500.0 * TWO_PI / SRATE);
  coeffs[1] = 0.6*0.6;
  soundDecay = SEKE_SOUND_DECAY;
  systemDecay = SEKE_SYSTEM_DECAY;
}

MY_FLOAT sekere_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;         // Exponential system decay
  if (my_random(1024) < num_objects)  // If collision
    sndLevel += gain * shakeEnergy;   //   add energy
  input = sndLevel * noise_tick();    // Actual Sound is Random
  sndLevel *= soundDecay;             // Exponential Sound decay 
  input -= output[0]*coeffs[0];       // Do gourd
  input -= output[1]*coeffs[1];       //   resonance
  output[1] = output[0];              //     filter
  output[0] = input;                  //       calculations
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = output[1];
  data = finalZ[0] - finalZ[2];
  return data * 2;			//  Normalization hack
}

/************************ CABASA ***************************/
#define CABA_SOUND_DECAY 0.95
#define CABA_SYSTEM_DECAY 0.997
#define CABA_NUM_BEADS 512

void cabasa_setup()  {
  num_objects = CABA_NUM_BEADS;
  gain = log(num_objects) / log(4.0) * 120.0 / (MY_FLOAT) num_objects;
  coeffs[0] = -0.7 * 2.0 * cos(3000.0 * TWO_PI / SRATE);
  coeffs[1] = 0.7*0.7;
  soundDecay = CABA_SOUND_DECAY;
  systemDecay = CABA_SYSTEM_DECAY;
}

MY_FLOAT cabasa_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;         // Exponential system decay
  if (my_random(1024) < num_objects)  // If collision
    sndLevel += gain * shakeEnergy;   //   add energy
  input = sndLevel * noise_tick();    // Actual Sound is Random
  sndLevel *= soundDecay;             // Exponential Sound decay 
  input -= output[0]*coeffs[0];       // Do gourd
  input -= output[1]*coeffs[1];       //   resonance
  output[1] = output[0];              //     filter
  output[0] = input;                  //       calculations
  data = output[0] - output[1];
  return data * 2;			//  Normalization hack
}

/************************ Bamboo Wind Chimes *****************/
#define BAMB_SOUND_DECAY 0.95
#define BAMB_SYSTEM_DECAY 0.99995
#define BAMB_NUM_TUBES 5
#define BAMB_BASE_FREQ  2800

void bamboo_setup() {
  num_objects = BAMB_NUM_TUBES;
  soundDecay = BAMB_SOUND_DECAY;
  systemDecay = BAMB_SYSTEM_DECAY;
  gain = 4.0 / (MY_FLOAT) num_objects;
  coeffs[0] = -0.995 * 2.0 * cos(BAMB_BASE_FREQ * TWO_PI / SRATE);
  coeffs[1] = 0.995*0.995;
  coeffs1[0] = -0.995 * 2.0 * cos(BAMB_BASE_FREQ * 0.8 * TWO_PI / SRATE);
  coeffs1[1] = 0.995*0.995;
  coeffs2[0] = -0.995 * 2.0 * cos(BAMB_BASE_FREQ * 1.2 * TWO_PI / SRATE);
  coeffs2[1] = 0.995*0.995;
}

MY_FLOAT bamboo_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;       // Exponential System Decay
  if (my_random(4096) < num_objects) {
    sndLevel += gain *  shakeEnergy;   
    freq_rand = BAMB_BASE_FREQ * (1.0 + (0.2 * noise_tick()));
    coeffs[0] = -0.995 * 2.0 * 
      cos(freq_rand * TWO_PI / SRATE);
    freq_rand = BAMB_BASE_FREQ * (0.8 + (0.2 * noise_tick()));
    coeffs1[0] = -0.995 * 2.0 * 
      cos(freq_rand * TWO_PI / SRATE);
    freq_rand = BAMB_BASE_FREQ * (1.2 + (0.2 * noise_tick()));
    coeffs2[0] = -0.995 * 2.0 * 
      cos(freq_rand * TWO_PI / SRATE);
  }
  input = sndLevel;
  input *= noise_tick();             // Actual Sound is Random
  input1 = input;
  input2 = input;
	
  sndLevel *= soundDecay;            // Each (all) event(s) 
  // decay(s) exponentially 
  input -= output[0]*coeffs[0];
  input -= output[1]*coeffs[1];
  output[1] = output[0];
  output[0] = input;
  data = output[0];
  input1 -= output1[0]*coeffs1[0];
  input1 -= output1[1]*coeffs1[1];
  output1[1] = output1[0];
  output1[0] = input1;
  data += output1[0];
  input2 -= output2[0]*coeffs2[0];
  input2 -= output2[1]*coeffs2[1];
  output2[1] = output2[0];
  output2[0] = input2;
  data += output2[0];
  return data * 0.5;			//  Normalization hack
}

/******************* Water Drops  ******************************/
#define WUTR_SOUND_DECAY 0.95
#define WUTR_NUM_SOURCES 4
#define WUTR_FILT_POLE   0.9985
#define WUTR_FREQ_SWEEP  1.0001
#define WUTR_BASE_FREQ  600

void wuter_setup()  {
  num_objects = WUTR_NUM_SOURCES;
  soundDecay = WUTR_SOUND_DECAY;
  freq =  WUTR_BASE_FREQ * TWO_PI / SRATE;
  freq1 = WUTR_BASE_FREQ * 2.0 * TWO_PI / SRATE;
  freq2 = WUTR_BASE_FREQ * 3.0 * TWO_PI / SRATE;
  coeffs[0]  = -WUTR_FILT_POLE * 2.0 * cos(freq);
  coeffs[1]  = WUTR_FILT_POLE * WUTR_FILT_POLE;
  coeffs1[0] = -WUTR_FILT_POLE * 2.0 * cos(freq1);
  coeffs1[1] = WUTR_FILT_POLE * WUTR_FILT_POLE;
  coeffs2[0] = -WUTR_FILT_POLE * 2.0 * cos(freq2);
  coeffs2[1] = WUTR_FILT_POLE * WUTR_FILT_POLE;
}
MY_FLOAT wuter_tick() {
  MY_FLOAT data;
  int j;
  if (my_random(32767) < num_objects) {     
    sndLevel = shakeEnergy;   
    j = my_random(3);
	  if (j == 0)   {
      freq = WUTR_BASE_FREQ * (0.75 + (0.25 * noise_tick()));
	    gain = fabs(noise_tick());
	  }
	  else if (j == 1)      {
      freq1 = WUTR_BASE_FREQ * (1.0 + (0.25 * noise_tick()));
	    gain1 = fabs(noise_tick());
	  }
	  else  {
      freq2 = WUTR_BASE_FREQ * (1.25 + (0.25 * noise_tick()));
	    gain2 = fabs(noise_tick());
	  }
	}
	
  gain  *= WUTR_FILT_POLE;
  if (gain >  0.001) {
    freq  *= WUTR_FREQ_SWEEP;
    coeffs[0] = -WUTR_FILT_POLE * 2.0 * 
      cos(freq * TWO_PI / SRATE);
  }
  gain1 *= WUTR_FILT_POLE;
  if (gain1 > 0.001) {
    freq1 *= WUTR_FREQ_SWEEP;
    coeffs1[0] = -WUTR_FILT_POLE * 2.0 * 
      cos(freq1 * TWO_PI / SRATE);
  }
  gain2 *= WUTR_FILT_POLE;
  if (gain2 > 0.001) {
    freq2 *= WUTR_FREQ_SWEEP;
    coeffs2[0] = -WUTR_FILT_POLE * 2.0 * 
      cos(freq2 * TWO_PI / SRATE);
  }
	
  sndLevel *= soundDecay;        // Each (all) event(s) 
  // decay(s) exponentially 
  input = sndLevel;
  input *= noise_tick();         // Actual Sound is Random
  input1 = input * gain1;
  input2 = input * gain2;
  input *= gain;
  input -= output[0]*coeffs[0];
  input -= output[1]*coeffs[1];
  output[1] = output[0];
  output[0] = input;
  data = output[0];
  input1 -= output1[0]*coeffs1[0];
  input1 -= output1[1]*coeffs1[1];
  output1[1] = output1[0];
  output1[0] = input1;
  data += output1[0];
  input2 -= output2[0]*coeffs2[0];
  input2 -= output2[1]*coeffs2[1];
  output2[1] = output2[0];
  output2[0] = input2;
  data += output2[0];
 
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data * 4;

  data = finalZ[2] - finalZ[0];
  return data;
}

/****************** TAMBOURINE  ******************************/
#define TAMB_SOUND_DECAY 0.95
#define TAMB_SYSTEM_DECAY 0.9985
#define TAMB_NUM_TIMBRELS 32
#define TAMB_SHELL_FREQ 2300
#define TAMB_SHELL_GAIN 0.1
#define TAMB_CYMB_FREQ  5600
#define TAMB_CYMB_FREQ2 8100
#define TAMB_CYMB_RESON 0.99

void tambourine_setup() {
  num_objects = TAMB_NUM_TIMBRELS;
  soundDecay = TAMB_SOUND_DECAY;
  systemDecay = TAMB_SYSTEM_DECAY;
  gain = 24.0 / num_objects;
  coeffs[0] = -0.96 * 2.0 * cos(TAMB_SHELL_FREQ * TWO_PI / SRATE);
  coeffs[1] = 0.96*0.96;
  coeffs1[0] = -TAMB_CYMB_RESON * 2.0 * cos(TAMB_CYMB_FREQ * TWO_PI / SRATE);
  coeffs1[1] = TAMB_CYMB_RESON * TAMB_CYMB_RESON;
  coeffs2[0] = -TAMB_CYMB_RESON * 2.0 * cos(TAMB_CYMB_FREQ2 * TWO_PI / SRATE);
  coeffs2[1] = TAMB_CYMB_RESON * TAMB_CYMB_RESON;
}

MY_FLOAT tambourine_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;    // Exponential System Decay
  if (my_random(1024) < num_objects) {     
    sndLevel += gain *  shakeEnergy;   
    cymb_rand = noise_tick() * TAMB_CYMB_FREQ * 0.05;
    coeffs1[0] = -TAMB_CYMB_RESON * 2.0 * 
      cos((TAMB_CYMB_FREQ + cymb_rand) * TWO_PI / SRATE);
    cymb_rand = noise_tick() * TAMB_CYMB_FREQ * 0.05;
    coeffs2[0] = -TAMB_CYMB_RESON * 2.0 * 
      cos((TAMB_CYMB_FREQ2 + cymb_rand) * TWO_PI / SRATE);
  }
  input = sndLevel;
  input *= noise_tick();         // Actual Sound is Random
  input1 = input * 0.8;
  input2 = input;
  input *= TAMB_SHELL_GAIN;
	
  sndLevel *= soundDecay;          // Each (all) event(s) 
  // decay(s) exponentially 
  input -= output[0]*coeffs[0];
  input -= output[1]*coeffs[1];
  output[1] = output[0];
  output[0] = input;
  data = output[0];
  input1 -= output1[0]*coeffs1[0];
  input1 -= output1[1]*coeffs1[1];
  output1[1] = output1[0];
  output1[0] = input1;
  data += output1[0];
  input2 -= output2[0]*coeffs2[0];
  input2 -= output2[1]*coeffs2[1];
  output2[1] = output2[0];
  output2[0] = input2;
  data += output2[0];
	
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data;
  data = finalZ[2] - finalZ[0];

  return data;
}

/************************ SLEIGHBELLS **************************/
#define SLEI_SOUND_DECAY 0.97
#define SLEI_SYSTEM_DECAY 0.9994
#define SLEI_NUM_BELLS 32
#define SLEI_CYMB_FREQ0 2500
#define SLEI_CYMB_FREQ1 5300
#define SLEI_CYMB_FREQ2 6500
#define SLEI_CYMB_FREQ3 8300
#define SLEI_CYMB_FREQ4 9800
#define SLEI_CYMB_RESON 0.99

void sleighbell_setup() {
  num_objects = SLEI_NUM_BELLS;
  soundDecay = SLEI_SOUND_DECAY;
  systemDecay = SLEI_SYSTEM_DECAY;
  gain = 8.0 / num_objects;
  coeffs[0] = -SLEI_CYMB_RESON * 2.0 * cos(SLEI_CYMB_FREQ0 * TWO_PI / SRATE);
  coeffs[1] = SLEI_CYMB_RESON*SLEI_CYMB_RESON;
  coeffs1[0] = -SLEI_CYMB_RESON * 2.0 * cos(SLEI_CYMB_FREQ1 * TWO_PI / SRATE);
  coeffs1[1] = SLEI_CYMB_RESON*SLEI_CYMB_RESON;
  coeffs2[0] = -SLEI_CYMB_RESON * 2.0 * cos(SLEI_CYMB_FREQ2 * TWO_PI / SRATE);
  coeffs2[1] = SLEI_CYMB_RESON*SLEI_CYMB_RESON;                      
  coeffs3[0] = -SLEI_CYMB_RESON * 2.0 * cos(SLEI_CYMB_FREQ3 * TWO_PI / SRATE);
  coeffs3[1] = SLEI_CYMB_RESON*SLEI_CYMB_RESON;
  coeffs4[0] = -SLEI_CYMB_RESON * 2.0 * cos(SLEI_CYMB_FREQ4 * TWO_PI / SRATE);
  coeffs4[1] = SLEI_CYMB_RESON*SLEI_CYMB_RESON;
}

MY_FLOAT sleighbell_tick() {
  MY_FLOAT data;
  shakeEnergy *= systemDecay;         // Exponential System Decay
  if (my_random(1024) < num_objects) {     
    sndLevel += gain *  shakeEnergy;   
    cymb_rand = noise_tick() * SLEI_CYMB_FREQ0 * 0.05;
    coeffs[0] = -SLEI_CYMB_RESON * 2.0 * 
      cos((SLEI_CYMB_FREQ0 + cymb_rand) * TWO_PI / SRATE);
    cymb_rand = noise_tick() * SLEI_CYMB_FREQ1 * 0.03;
    coeffs1[0] = -SLEI_CYMB_RESON * 2.0 * 
      cos((SLEI_CYMB_FREQ1 + cymb_rand) * TWO_PI / SRATE);
    cymb_rand = noise_tick() * SLEI_CYMB_FREQ2 * 0.03;
    coeffs2[0] = -SLEI_CYMB_RESON * 2.0 * 
      cos((SLEI_CYMB_FREQ2 + cymb_rand) * TWO_PI / SRATE);
    cymb_rand = noise_tick() * SLEI_CYMB_FREQ3 * 0.03;
    coeffs3[0] = -SLEI_CYMB_RESON * 2.0 * 
      cos((SLEI_CYMB_FREQ3 + cymb_rand) * TWO_PI / SRATE);
    cymb_rand = noise_tick() * SLEI_CYMB_FREQ4 * 0.03;
    coeffs4[0] = -SLEI_CYMB_RESON * 2.0 * 
      cos((SLEI_CYMB_FREQ4 + cymb_rand) * TWO_PI / SRATE);
  }
  input = sndLevel;
  input *= noise_tick();        // Actual Sound is Random
  input1 = input;
  input2 = input;
  input3 = input * 0.5;
  input4 = input * 0.3;

  sndLevel *= soundDecay;            // Each (all) event(s) 
  // decay(s) exponentially 
  input -= output[0]*coeffs[0];
  input -= output[1]*coeffs[1];
  output[1] = output[0];
  output[0] = input;
  data = output[0];
	
  input1 -= output1[0]*coeffs1[0];
  input1 -= output1[1]*coeffs1[1];
  output1[1] = output1[0];
  output1[0] = input1;
  data += output1[0];

  input2 -= output2[0]*coeffs2[0];
  input2 -= output2[1]*coeffs2[1];
  output2[1] = output2[0];
  output2[0] = input2;
  data += output2[0];
	
  input3 -= output3[0]*coeffs3[0];
  input3 -= output3[1]*coeffs3[1];
  output3[1] = output3[0];
  output3[0] = input3;
  data += output3[0];
	
  input4 -= output4[0]*coeffs4[0];
  input4 -= output4[1]*coeffs4[1];
  output4[1] = output4[0];
  output4[0] = input4;
  data += output4[0];
	 
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = data;
  data = finalZ[2] - finalZ[0];
		
  return data;
}
 
/****************************  GUIRO  ***********************/
#define GUIR_SOUND_DECAY 0.95
#define GUIR_NUM_RATCHETS 128
#define GUIR_GOURD_FREQ  2500.0
#define GUIR_GOURD_RESON 0.97
#define GUIR_GOURD_FREQ2  4000.0
#define GUIR_GOURD_RESON2 0.97

void guiro_setup()  {
  num_objects = GUIR_NUM_RATCHETS;
  soundDecay = GUIR_SOUND_DECAY;
  coeffs[0] = -GUIR_GOURD_RESON * 2.0 * cos(GUIR_GOURD_FREQ * TWO_PI / SRATE);
  coeffs[1] = GUIR_GOURD_RESON*GUIR_GOURD_RESON;
  coeffs2[0] = -GUIR_GOURD_RESON2 * 2.0 * cos(GUIR_GOURD_FREQ2 * TWO_PI / SRATE);
  coeffs2[1] = GUIR_GOURD_RESON2*GUIR_GOURD_RESON2;
  ratchet = 0;
  guiroScrape = 0;
}

MY_FLOAT guiro_tick() {
  MY_FLOAT data;
  if (my_random(1024) < num_objects) {
    sndLevel += 512 * ratchet * totalEnergy;
  }
  input = sndLevel;
  input *= noise_tick() * ratchet;
  sndLevel *= soundDecay;
		 
  input2 = input;
  input -= output[0]*coeffs[0];
  input -= output[1]*coeffs[1];
  output[1] = output[0];
  output[0] = input;
  input2 -= output2[0]*coeffs2[0];
  input2 -= output2[1]*coeffs2[1];
  output2[1] = output2[0];
  output2[0] = input2;
     
  finalZ[2] = finalZ[1];
  finalZ[1] = finalZ[0];
  finalZ[0] = output[1] + output2[1];
  data = finalZ[0] - finalZ[2];
  return data;
}

/*******************  THE ACTUAL CLASS ITSELF  ***********************/

#include "Shakers.h"
#include "SKINI11.msg"

Shakers :: Shakers() : Instrmnt()
{
  instType = 0;
}

Shakers :: ~Shakers()
{
}

#define MAX_SHAKE 2000.0

void Shakers :: noteOn(MY_FLOAT freq, MY_FLOAT amp)
{
  shakeEnergy = amp * MAX_SHAKE * 0.1;
#if defined(_debug_)        
	printf("Shakers : NoteOn: Freq=%lf Amp=%lf\n",freq,amp);
#endif    
}

void Shakers :: noteOff(MY_FLOAT amp)
{
  shakeEnergy = 0.0;
}

MY_FLOAT Shakers :: tick()
{
  if      (instType==0)    {
    lastOutput = maraca_tick();
  }
  else if (instType==1)    {
    lastOutput = sekere_tick();
  }
  else if (instType==2)    {
    lastOutput = cabasa_tick();
  }
  else if (instType==3)    {
    lastOutput = bamboo_tick();
  }
  else if (instType==4)    {
    lastOutput = wuter_tick();
  }
  else if (instType==5)    {
    lastOutput = tambourine_tick();
  }
  else if (instType==6)    {
    lastOutput = sleighbell_tick();
  }
  else if (instType==7)    {
    if (guiroScrape < 1.0)      {
      guiroScrape += scrapeVel;
      totalEnergy = guiroScrape;
      ratchet -= (ratchetDelta + (0.002*totalEnergy));
      if (ratchet<0.0) ratchet = 1.0;
      lastOutput = guiro_tick();
    }
    else lastOutput = 0.0;
  }

  lastOutput *= 0.0001;

  return lastOutput;
}

void Shakers :: controlChange(int number, MY_FLOAT value)
{
  MY_FLOAT temp;
#if defined(_debug_)        
  printf("Shakers : ControlChange: Number=%i Value=%f\n",number,value);
#endif    
  if (number == __SK_Breath_) {
#if defined(_debug_)        
    printf("shaking \n");
#endif    
    shakeEnergy += value * NORM_7 * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  }
  else if (number == __SK_FootControl_)       {
#if defined(_debug_)        
    printf("setting decay\n");
#endif    
    systemDecay = 0.998 + (value * NORM_7 * 0.002);
  }
  else if (number == __SK_ModFrequency_)    {
#if defined(_debug_)        
    printf("setting number of beans\n");
#endif    
    num_objects = (int) value;
    gain = log(num_objects) * 30.0 / (MY_FLOAT) num_objects;
  }
  else if (number == __SK_ModWheel_)		{
#if defined(_debug_)        
    printf("setting resonance freq.\n");
#endif    
    temp = 900 * pow (1.015,value);
    coeffs[0] = -0.96 * 2.0 * cos(temp * TWO_PI / SRATE);
    coeffs[1] = 0.96*0.96;
  }
  else if (number == __SK_AfterTouch_Cont_)   {
#if defined(_debug_)        
    printf("shaking \n");
#endif    
    shakeEnergy += value * NORM_7 * MAX_SHAKE * 0.1;
    if (shakeEnergy > MAX_SHAKE) shakeEnergy = MAX_SHAKE;
  }
  else  if (number == __SK_ShakerInst_)      {
    instType = (int) (value + 0.5);	//  Just to be safe
    if      (instType==0) maraca_setup();
    else if (instType==1) sekere_setup();
    else if (instType==2) cabasa_setup();
    else if (instType==3) bamboo_setup();
    else if (instType==4) wuter_setup();
    else if (instType==5) tambourine_setup();
    else if (instType==6) sleighbell_setup();
    else if (instType==7) guiro_setup();
    else {
	    instType = 0;
	    maraca_setup();
    }
  }                                       
  else	{
    printf("Shakers : Undefined Control Number!!\n");
  }  
}

