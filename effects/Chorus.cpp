/******************************************/  
/*  Chorus Effect Applied to Soundfile    */
/*  by Perry Cook, 1996                   */
/******************************************/

#include "Chorus.h"

Chorus :: Chorus(MY_FLOAT baseDelay)
{
  delayLine[0] = new DLineL((long) (baseDelay * 1.414) + 2);
  delayLine[1] = new DLineL((long) (baseDelay) + 2);
  delayLine[0]->setDelay(baseDelay);
  delayLine[1]->setDelay(baseDelay);
  baseLength = baseDelay;

  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char temp[128];
  strcpy(temp, RAWWAVE_PATH);
  mods[0] = new RawWvIn(strcat(temp,"rawwaves/sinewave.raw"),"looping");
  strcpy(temp, RAWWAVE_PATH);
  mods[1] = new RawWvIn(strcat(temp,"rawwaves/sinewave.raw"),"looping");
  mods[0]->normalize();
  mods[1]->normalize();
  mods[0]->setFreq(0.2);
  mods[1]->setFreq(0.222222);
  modDepth = 0.05;
  effectMix = (MY_FLOAT) 0.5;
  this->clear();
}

Chorus :: ~Chorus()
{
  delete delayLine[0];
  delete delayLine[1];
  delete mods[0];
  delete mods[1];
}

void Chorus :: clear()
{
  delayLine[0]->clear();
  delayLine[1]->clear();
  lastOutL = (MY_FLOAT) 0.0;
  lastOutR = (MY_FLOAT) 0.0;
}

void Chorus :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

void Chorus :: setModDepth(MY_FLOAT depth)
{
  modDepth = depth;
}

void Chorus :: setModFreq(MY_FLOAT freq)
{
  mods[0]->setFreq(freq);
  mods[1]->setFreq(freq*1.1111);
}

MY_FLOAT Chorus :: lastOutput()
{
  return (lastOutL + lastOutR) * (MY_FLOAT) 0.5;
}

MY_FLOAT Chorus :: lastOutputL()
{
  return lastOutL;
}

MY_FLOAT Chorus :: lastOutputR()
{
  return lastOutR;
}

MY_FLOAT Chorus :: tick(MY_FLOAT input)
{
  delayLine[0]->setDelay(baseLength * 0.707 * (1.0 + mods[0]->tick()));
  delayLine[1]->setDelay(baseLength  * 0.5 *  (1.0 - mods[1]->tick()));
  lastOutL = input * (1.0 - effectMix);
  lastOutL += effectMix * delayLine[0]->tick(input);
  lastOutR = input * (1.0 - effectMix);
  lastOutR += effectMix * delayLine[1]->tick(input);
  return (lastOutL + lastOutR) * (MY_FLOAT) 0.5;
}

/**************  Test Main Program  *********************/
/*
int main(int argc, char *argv[])
{
    FILE *soundIn,*soundOut;
    short data;
    float efMix,maxDel;
    Chorus *effect;    

    if (argc==5)        {
        soundIn = fopen(argv[3],"rb");
        soundOut = fopen(argv[4],"wb");
	if (soundIn && soundOut)	{
	    efMix = atof(argv[1]);
            maxDel = atof(argv[2]);
            effect = new Chorus(maxDel);
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
        printf("useage: Chorus        mix maxDelay soundIn.snd soundOut.snd\n");
        printf("0.0 <= mix <= 1.0\n");
        printf("maxDelay is in samples\n");
        printf("soundfiles are 16 bit linear mono or stereo\n");
    }
}
*/
