/******************************************/  
/*  NRev Reverb Subclass                  */
/*  by Tim Stilson, 1998                  */
/*    based on CLM NRev                   */
/*  Integrated into STK by Gary Scavone   */
/*                                        */
/*  This is based on some of the famous   */
/*  Stanford CCRMA reverbs (NRev, KipRev) */
/*  all based on the the Chowning/Moorer/ */
/*  Schroeder reverberators, which use    */
/*  networks of simple allpass and comb   */
/*  delay filters.  This particular       */
/*  arrangement consists of 6 comb        */
/*  filters in parallel, followed by 3    */
/*  allpass filters, a lowpass filter,    */
/*  and another allpass in series,        */
/*  followed by two allpass filters in    */
/*  parallel with corresponding right and */
/*  left outputs.                         */
/******************************************/

#include "NRev.h"

NRev :: NRev(MY_FLOAT T60)
{
  int lens[15]={1433,1601,1867,2053,2251,2399,347,113,37,59,53,43,37,29,19};
  double srscale= SRATE / 25641.0;
  int val;
  int i;

  for (i=0; i<15; i++)
	{
	  val = (int)floor(srscale*lens[i]);
	  if ((val & 1) == 0) val++;
	  while (!this->isprime(val)) val+=2;
	  lens[i]=val;
	}
  for (i=0; i<6; i++)
	{
	  CdelayLine[i] = new DLineN((long) (lens[i]) + 2);
	  CdelayLine[i]->setDelay((long) (lens[i]));
	  combCoef[i] = pow(10,(-3 * lens[i] / (T60 * SRATE)));
	}
  for (i=0; i<8; i++)
	{
	  APdelayLine[i] = new DLineN((long) (lens[i+6]) + 2);
	  APdelayLine[i]->setDelay((long) (lens[i+6]));
	}
	
  allPassCoeff = 0.7;
  effectMix = 0.3;
  this->clear();
}

NRev :: ~NRev()
{
	int i;

	for (i=0; i<6; i++)  delete CdelayLine[i];
	for (i=0; i<8; i++)  delete APdelayLine[i];
}

void NRev :: clear()
{
	int i;

	for (i=0; i<6; i++)  CdelayLine[i]->clear();
	for (i=0; i<8; i++)  APdelayLine[i]->clear();
    lastOutL = 0.0;
    lastOutR = 0.0;
	lpLastout = 0.0;
}

void NRev :: setEffectMix(MY_FLOAT mix)
{
    effectMix = mix;
}

MY_FLOAT NRev :: lastOutput()
{
    return (lastOutL + lastOutR) * 0.5;
}

MY_FLOAT NRev :: lastOutputL()
{
    return lastOutL;
}

MY_FLOAT NRev :: lastOutputR()
{
    return lastOutR;
}

MY_FLOAT NRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp,temp0,temp1,temp2,temp3;
  int i;

  temp0 = 0.0;
  for (i=0; i<6; i++)
	{
	  temp = input + (combCoef[i] * CdelayLine[i]->lastOut());
	  temp0 += CdelayLine[i]->tick(temp);
	}
  for (i=0; i<3; i++)
	{
	  temp = APdelayLine[i]->lastOut();
	  temp1 = allPassCoeff * temp;
	  temp1 += temp0;
	  APdelayLine[i]->tick(temp1);
	  temp0 = -(allPassCoeff * temp1) + temp;
	}
  lpLastout = 0.7*lpLastout + 0.3*temp0;	// onepole LP filter
  temp = APdelayLine[3]->lastOut();
  temp1 = allPassCoeff * temp;
  temp1 += lpLastout;
  APdelayLine[3]->tick(temp1);
  temp1 = -(allPassCoeff * temp1) + temp;
    
  temp = APdelayLine[4]->lastOut();
  temp2 = allPassCoeff * temp;
  temp2 += temp1;
  APdelayLine[4]->tick(temp2);
  lastOutL = effectMix*(-(allPassCoeff * temp2) + temp);
    
  temp = APdelayLine[5]->lastOut();
  temp3 = allPassCoeff * temp;
  temp3 += temp1;
  APdelayLine[5]->tick(temp3);
  lastOutR = effectMix*(-(allPassCoeff * temp3) + temp);

  temp = (1.0 - effectMix) * input;
  lastOutL += temp;
  lastOutR += temp;
    
  return (lastOutL + lastOutR) * 0.5;

}
