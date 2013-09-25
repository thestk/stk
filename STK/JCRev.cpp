/*******************************************/  
/*  JVRev Reverb Subclass                  */
/*  by Tim Stilson, 1998                   */
/*    based on CLM JCRev                   */
/*  Integrated into STK by Gary Scavone    */
/*                                         */
/*  This is based on some of the famous    */
/*  Stanford CCRMA reverbs (NRev, KipRev)  */
/*  all based on the Chowning/Moorer/      */
/*  Schroeder reverberators, which use     */
/*  networks of simple allpass and comb    */
/*  delay filters.  This particular        */
/*  arrangement consists of 3 allpass      */
/*  filters in series, followed by 4 comb  */
/*  filters in parallel, an optional       */
/*  lowpass filter, and two decorrelation  */
/*  delay lines in parallel at the output. */
/*******************************************/

#include "JCRev.h"

//#define LOWPASS

JCRev :: JCRev(MY_FLOAT T60)
{
  /* These are the values from CLM's JCRev.ins ... I found that the
	 impulse response sounded better with the shorter delay lengths.
	 --Gary Scavone, 2/1998
	 int lens[9] = {4799,4999,5399,5801,1051,337,113,573,487};
   */
  int lens[9] = {1777,1847,1993,2137,389,127,43,211,179};
  int val, i;

  if (SRATE < 44100.0) {
    double srscale = SRATE / 44100.0;
    for (i=0; i<9; i++)	{
      val = (int) floor(srscale * lens[i]);
      if ((val & 1) == 0) val++;
      while (!this->isprime(val)) val += 2;
      lens[i] = val;
    }
  }

  for (i=0; i<3; i++)
	{
	  APdelayLine[i] = new DLineN(lens[i+4] + 2);
	  APdelayLine[i]->setDelay(lens[i+4]);
	}
  for (i=0; i<4; i++)
	{
	  CdelayLine[i] = new DLineN(lens[i] + 2);
	  CdelayLine[i]->setDelay(lens[i]);
	  combCoeff[i] = pow(10,(-3 * lens[i] / (T60 * SRATE)));
	  //	  printf("combCoeff[%d] = %f\n", i, combCoeff[i]);
	}
  outLdelayLine = new DLineN(lens[7] + 2);
  outLdelayLine->setDelay(lens[7]);
  outRdelayLine = new DLineN(lens[8] + 2);
  outRdelayLine->setDelay(lens[8]);
  allPassCoeff = 0.7;
  effectMix = 0.3;
  this->clear();
}

JCRev :: ~JCRev()
{
    delete APdelayLine[0];
    delete APdelayLine[1];
    delete APdelayLine[2];
    delete CdelayLine[0];
    delete CdelayLine[1];
    delete CdelayLine[2];
    delete CdelayLine[3];
    delete outLdelayLine;
    delete outRdelayLine;
}

void JCRev :: clear()
{
  APdelayLine[0]->clear();
  APdelayLine[1]->clear();
  APdelayLine[2]->clear();
  CdelayLine[0]->clear();
  CdelayLine[1]->clear();
  CdelayLine[2]->clear();
  CdelayLine[3]->clear();
  outRdelayLine->clear();
  outLdelayLine->clear();
  lastOutL = 0.0;
  lastOutR = 0.0;
  combsum1=0.0;
  combsum2=0.0;
  combsum=0.0;
}

void JCRev :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT JCRev :: lastOutput()
{
  return (lastOutL + lastOutR) * 0.5;
}

MY_FLOAT JCRev :: lastOutputL()
{
  return lastOutL;
}

MY_FLOAT JCRev :: lastOutputR()
{
  return lastOutR;
}

MY_FLOAT JCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp,temp0,temp1,temp2,temp3,temp4,temp5,temp6;
  MY_FLOAT filtout;

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
    
  temp = APdelayLine[2]->lastOut();
  temp2 = allPassCoeff * temp;
  temp2 += temp1;
  APdelayLine[2]->tick(temp2);
  temp2 = -(allPassCoeff * temp2) + temp;
    
  temp3 = temp2 + (combCoeff[0] * CdelayLine[0]->lastOut());
  temp4 = temp2 + (combCoeff[1] * CdelayLine[1]->lastOut());
  temp5 = temp2 + (combCoeff[2] * CdelayLine[2]->lastOut());
  temp6 = temp2 + (combCoeff[3] * CdelayLine[3]->lastOut());

  CdelayLine[0]->tick(temp3);
  CdelayLine[1]->tick(temp4);
  CdelayLine[2]->tick(temp5);
  CdelayLine[3]->tick(temp6);

#ifdef LOWPASS
  combsum2=combsum1;	
  combsum1=combsum;	
  combsum = temp3+temp4+temp5+temp6;
  filtout= 0.5*combsum1+0.25*(combsum+combsum2);
#else
  filtout = temp3+temp4+temp5+temp6;
#endif

  lastOutL = effectMix * (outLdelayLine->tick(filtout));
  lastOutR = effectMix * (outRdelayLine->tick(filtout));
  temp = (1.0 - effectMix) * input;
  lastOutL += temp;
  lastOutR += temp;
    
  return (lastOutL + lastOutR) * 0.5;

}
