/***************************************************/
/*! \class PRCRev
    \brief Perry's simple reverberator class.

    This class is based on some of the famous
    Stanford/CCRMA reverbs (NRev, KipRev), which
    were based on the Chowning/Moorer/Schroeder
    reverberators using networks of simple allpass
    and comb delay filters.  This class implements
    two series allpass units and two parallel comb
    filters.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "PRCRev.h"
#include <math.h>

PRCRev :: PRCRev(MY_FLOAT T60)
{
  // Delay lengths for 44100 Hz sample rate.
  int lengths[4]= {353, 1097, 1777, 2137};
  double scaler = Stk::sampleRate() / 44100.0;

  // Scale the delay lengths if necessary.
  int delay, i;
  if ( scaler != 1.0 ) {
    for (i=0; i<4; i++)	{
      delay = (int) floor(scaler * lengths[i]);
      if ( (delay & 1) == 0) delay++;
      while ( !this->isPrime(delay) ) delay += 2;
      lengths[i] = delay;
    }
  }

  for (i=0; i<2; i++)	{
    allpassDelays[i] = new Delay( lengths[i], lengths[i] );
    combDelays[i] = new Delay( lengths[i+2], lengths[i+2] );
    combCoefficient[i] = pow(10.0,(-3 * lengths[i+2] / (T60 * Stk::sampleRate())));
  }

  allpassCoefficient = 0.7;
  effectMix = 0.5;
  this->clear();
}

PRCRev :: ~PRCRev()
{
  delete allpassDelays[0];
  delete allpassDelays[1];
  delete combDelays[0];
  delete combDelays[1];
}

void PRCRev :: clear()
{
  allpassDelays[0]->clear();
  allpassDelays[1]->clear();
  combDelays[0]->clear();
  combDelays[1]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
}

MY_FLOAT PRCRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3;

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
    
  temp2 = temp1 + (combCoefficient[0] * combDelays[0]->lastOut());
  temp3 = temp1 + (combCoefficient[1] * combDelays[1]->lastOut());

  lastOutput[0] = effectMix * (combDelays[0]->tick(temp2));
  lastOutput[1] = effectMix * (combDelays[1]->tick(temp3));
  temp = (MY_FLOAT) (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * (MY_FLOAT) 0.5;

}
