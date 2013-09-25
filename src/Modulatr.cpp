/*******************************************/
/*  Modulator Class, Perry R. Cook, 1995-96*/ 
/*  This Object combines random and        */
/*  periodic modulations to give a nice    */
/*  natural human modulation function.     */  
/*******************************************/

#define POLE_POS  (MY_FLOAT) 0.999
#define RND_SCALE (MY_FLOAT) 10.0

#include "Modulatr.h"

Modulatr :: Modulatr()
{
  // Concatenate the STK RAWWAVE_PATH to the rawwave file
  char file[128];
  strcpy(file, RAWWAVE_PATH);
  vibwave = new RawWvIn(strcat(file,"rawwaves/sinewave.raw"),"looping");
  vibwave->setFreq((MY_FLOAT) 6.0);
  vibAmt = (MY_FLOAT)  0.04;
  noise = new SubNoise(330);
  rndAmt = (MY_FLOAT)  0.005;
  onepole = new OnePole;
  onepole->setPole(POLE_POS);
  onepole->setGain(rndAmt * RND_SCALE);
}

Modulatr :: ~Modulatr()
{
  delete vibwave;
  delete noise;
  delete onepole;
}

void Modulatr :: reset()
{
  lastOutput = (MY_FLOAT)  0.0;
}

void Modulatr :: setVibFreq(MY_FLOAT vibFreq)
{
  vibwave->setFreq(vibFreq);
}

void Modulatr :: setVibAmt(MY_FLOAT vibAmount)
{
  vibAmt = vibAmount;
}

void Modulatr :: setRndAmt(MY_FLOAT rndAmount)
{
  rndAmt = rndAmount;
  onepole->setGain(RND_SCALE * rndAmt);
}

MY_FLOAT Modulatr ::  tick()
{
  lastOutput = vibAmt * vibwave->tick();       /*  Compute periodic and */
  lastOutput += onepole->tick(noise->tick());  /*   random modulations  */
  return lastOutput;                        
}

MY_FLOAT Modulatr :: lastOut()
{
  return lastOutput;
}

/************   Test Main Program   *****************/
/*
void main()
{
    Modulatr testMod;
    FILE *fd;
    short data;
    long i;
    
    fd = fopen("test.raw","wb");
    
    for (i=0;i<20000;i++) {
	data = testMod.tick() * 32000.0;
	fwrite(&data,2,1,fd);
    }
    fclose(fd);
}
*/
