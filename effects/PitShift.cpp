/*********************************************/  
/* PitchShift Effect                         */
/*  by Perry Cook, 1996                      */
/*********************************************/

#include "PitShift.h"

PitShift :: PitShift()
{
  delayLine[0] = new DLineL((long) 1024);
  delayLine[1] = new DLineL((long) 1024);
  delay[0] = 12;
  delay[1] = 512;
  delayLine[0]->setDelay(delay[0]);
  delayLine[1]->setDelay(delay[1]);
  effectMix = (MY_FLOAT) 0.5;
  rate = 1.0;
}

PitShift :: ~PitShift()
{
  delete delayLine[0];
  delete delayLine[1];
}

void PitShift :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

void PitShift :: setShift(MY_FLOAT shift)
{
  if (shift < 1.0)    {
    rate = 1.0 - shift; 
  }
  else if (shift > 1.0)       {
    rate = 1.0 - shift;
  }
  else {
    rate = 0.0;
    delay[0] = 512;
  }
}

MY_FLOAT PitShift :: lastOutput()
{
  return lastOut;
}

MY_FLOAT PitShift :: tick(MY_FLOAT input)
{
  delay[0] = delay[0] + rate;
  while (delay[0] > 1012) delay[0] -= 1000;
  while (delay[0] < 12) delay[0] += 1000;
  delay[1] = delay[0] + 500;
  while (delay[1] > 1012) delay[1] -= 1000;
  while (delay[1] < 12) delay[1] += 1000;
  delayLine[0]->setDelay(delay[0]);
  delayLine[1]->setDelay(delay[1]);
  env[1] = fabs(delay[0] - 512) * 0.002;
  env[0] = 1.0 - env[1];
  lastOut =  env[0] * delayLine[0]->tick(input);
  lastOut += env[1] * delayLine[1]->tick(input);
  lastOut *= effectMix;
  lastOut += (1.0 - effectMix) * input;
  return lastOut;
}

/**************  Test Main Program  *********************/
/*
int main(int argc, char *argv[])
{
    FILE *soundIn,*soundOut;
    short data;
    float efMix,pitchshift;
    PitShift *effect;    

    if (argc==5)        {
        soundIn = fopen(argv[3],"rb");
        soundOut = fopen(argv[4],"wb");
	if (soundIn && soundOut)	{
	    efMix = atof(argv[1]);
            pitchshift = atof(argv[2]);
            effect = new PitShift();
            effect->setShift(pitchshift);
            effect->setEffectMix(efMix);
            while (fread(&data,2,1,soundIn))     {
                data = effect->tick(data);
                fwrite(&data,2,1,soundOut);
            }
	    delete effect;
	    fclose(soundIn);
	    fclose(soundOut);
	}
        else	{
            printf("Can't open one of the files\n");
        }
    }
    else	{
        printf("useage: pitshift        mix shiftRate soundIn.snd soundOut.snd\n");
        printf("0.0 <= mix <= 1.0\n");
        printf("maxDelay is in samples\n");
        printf("soundfiles are 16 bit linear mono or stereo\n");
    }
}
*/
