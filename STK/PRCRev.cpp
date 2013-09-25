/*******************************************/  
/* PRCRev, a simple reverb unit            */
/* by Perry Cook, 1996.                    */
/* Incorporated into the Reverb superclass */
/* by Gary Scavone, 1998.                  */
/*                                         */
/* This is based on some of the famous     */
/* Stanford CCRMA reverbs (NRev, KipRev)   */
/* all based on the the Chowning/Moorer/   */
/* Schroeder reverberators, which use      */
/* networks of simple allpass and comb     */
/* delay filters.  This particular         */
/* structure consists of 2 allpass units   */
/* in series followed by 2 comb filters in */
/* parallel.                               */
/*******************************************/

#include "PRCRev.h"

PRCRev :: PRCRev(MY_FLOAT T60)
{
  int lens[4]={353,1097,1777,2137};
  double srscale = SRATE / 44100.0;
  int val, i;

  if (SRATE < 44100.0) {
	for (i=0; i<4; i++)	{
	  val = (int) floor(srscale * lens[i]);
	  if ((val & 1) == 0) val++;
	  while (!this->isprime(val)) val += 2;
	  lens[i] = val;
	}
  }
  for (i=0; i<2; i++)
	{
	  APdelayLine[i] = new DLineN(lens[i] + 2);
	  APdelayLine[i]->setDelay(lens[i]);
	  CdelayLine[i] = new DLineN(lens[i+2] + 2);
	  CdelayLine[i]->setDelay(lens[i+2]);
	  combCoeff[i] = pow(10,(-3 * lens[i+2] / (T60 * SRATE)));
	}

  allPassCoeff = (MY_FLOAT) 0.7;
  effectMix = (MY_FLOAT) 0.5;
  this->clear();
}

PRCRev :: ~PRCRev()
{
  delete APdelayLine[0];
  delete APdelayLine[1];
  delete CdelayLine[0];
  delete CdelayLine[1];
}

void PRCRev :: clear()
{
  APdelayLine[0]->clear();
  APdelayLine[1]->clear();
  CdelayLine[0]->clear();
  CdelayLine[1]->clear();
  lastOutL = (MY_FLOAT) 0.0;
  lastOutR = (MY_FLOAT) 0.0;
}

void PRCRev :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT PRCRev :: lastOutput()
{
  return (lastOutL + lastOutR) * (MY_FLOAT) 0.5;
}

MY_FLOAT PRCRev :: lastOutputL()
{
  return lastOutL;
}

MY_FLOAT PRCRev :: lastOutputR()
{
  return lastOutR;
}

MY_FLOAT PRCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp,temp0,temp1,temp2,temp3;

  temp = APdelayLine[0]->lastOut();
  temp0 = allPassCoeff * temp;
  temp0 += input;
  APdelayLine[0]->tick(temp0);
  temp0 = -(allPassCoeff * temp0) + temp;
    
  temp = APdelayLine[1]->lastOut();
  temp1 = allPassCoeff * temp;
  temp1 += temp0;
  APdelayLine[1]->tick(temp1);
  temp1 = -(allPassCoeff * temp1) + temp;
    
  temp2 = temp1 + (combCoeff[0] * CdelayLine[0]->lastOut());
  temp3 = temp1 + (combCoeff[1] * CdelayLine[1]->lastOut());

  lastOutL = effectMix * (CdelayLine[0]->tick(temp2));
  lastOutR = effectMix * (CdelayLine[1]->tick(temp3));
  temp = (MY_FLOAT) (1.0 - effectMix) * input;
  lastOutL += temp;
  lastOutR += temp;
    
  return (lastOutL + lastOutR) * (MY_FLOAT) 0.5;

}
