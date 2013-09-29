/***************************************************/
/*! \class JCRev
    \brief John Chowning's reverberator class.

    This class is derived from the CLM JCRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This class implements three series
    allpass units, followed by four parallel comb
    filters, and two decorrelation delay lines in
    parallel at the output.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "JCRev.h"
#include <math.h>

JCRev :: JCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[9] = {1777, 1847, 1993, 2137, 389, 127, 43, 211, 179};
  double scaler = Stk::sampleRate() / 44100.0;

  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<9; i++) {
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<3; i++)
	  allpassDelays[i] = new Delay(lengths[i+4], lengths[i+4]);

  for (i=0; i<4; i++)	{
    combDelays[i] = new Delay(lengths[i], lengths[i]);
    combCoefficient[i] = pow(10.0,(-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  outLeftDelay = new Delay(lengths[7], lengths[7]);
  outRightDelay = new Delay(lengths[8], lengths[8]);
  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

JCRev :: ~JCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete allpassDelays[2];
  delete combDelays[0];
  delete combDelays[1];
  delete combDelays[2];
  delete combDelays[3];
  delete outLeftDelay;
  delete outRightDelay;
}

void JCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  allpassDelays[2]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  combDelays[2]->clear();
  combDelays[3]->clear();
  outRightDelay->clear();
  outLeftDelay->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT JCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3, temp4, temp5, temp6;
  MY_FLOAT filtout;

  temp = allpassDelays[0]->lastOut();
  temp0 = allpassCoefficient * temp;
  temp0 += input;
  allpassDelays[0]->tick(temp0);
  temp0 = -(allpassCoefficient * temp0) + temp;
    
  temp = allpassDelays[1]->lastOut();
  temp1 = allpassCoefficient * temp;
  temp1 += temp0;
  allpassDelays[1]->tick(temp1);
  temp1 = -(allpassCoefficient * temp1) + temp;
    
  temp = allpassDelays[2]->lastOut();
  temp2 = allpassCoefficient * temp;
  temp2 += temp1;
  allpassDelays[2]->tick(temp2);
  temp2 = -(allpassCoefficient * temp2) + temp;
    
  temp3 = temp2 + (combCoefficient[0] * combDelays[0]->lastOut());
  temp4 = temp2 + (combCoefficient[1] * combDelays[1]->lastOut());
  temp5 = temp2 + (combCoefficient[2] * combDelays[2]->lastOut());
  temp6 = temp2 + (combCoefficient[3] * combDelays[3]->lastOut());

  combDelays[0]->tick(temp3);
  combDelays[1]->tick(temp4);
  combDelays[2]->tick(temp5);
  combDelays[3]->tick(temp6);

  filtout = temp3 + temp4 + temp5 + temp6;

  lastOutput[0] = effectMix * (outLeftDelay->tick(filtout));
  lastOutput[1] = effectMix * (outRightDelay->tick(filtout));
  temp = (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * 0.5;
}
