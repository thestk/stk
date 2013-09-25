/***************************************************/
/*! \class NRev
    \brief CCRMA's NRev reverberator class.

    This class is derived from the CLM NRev
    function, which is based on the use of
    networks of simple allpass and comb delay
    filters.  This particular arrangement consists
    of 6 comb filters in parallel, followed by 3
    allpass filters, a lowpass filter, and another
    allpass in series, followed by two allpass
    filters in parallel with corresponding right
    and left outputs.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "NRev.h"
#include <math.h>

NRev :: NRev(MY_FLOAT T60)
{
  int lengths[15] = {1433, 1601, 1867, 2053, 2251, 2399, 347, 113, 37, 59, 53, 43, 37, 29, 19};
  double scaler = Stk::sampleRate() / 25641.0;

  int delay, i;
  for (i=0; i<15; i++) {
    delay = (int) floor(scaler * lengths[i]);
    if ( (delay & 1) == 0) delay++;
    while ( !this->isPrime(delay) ) delay += 2;
    lengths[i] = delay;
  }

  for (i=0; i<6; i++) {
    combDelays[i] = new Delay( lengths[i], lengths[i]);
    combCoefficient[i] = pow(10, (-3 * lengths[i] / (T60 * Stk::sampleRate())));
  }

  for (i=0; i<8; i++)
    allpassDelays[i] = new Delay(lengths[i+6], lengths[i+6]);

  allpassCoefficient = 0.7;
  effectMix = 0.3;
  this->clear();
}

NRev :: ~NRev()
{
  int i;
  for (i=0; i<6; i++)  delete combDelays[i];
  for (i=0; i<8; i++)  delete allpassDelays[i];
}

void NRev :: clear()
{
  int i;
  for (i=0; i<6; i++) combDelays[i]->clear();
  for (i=0; i<8; i++) allpassDelays[i]->clear();
  lastOutput[0] = 0.0;
  lastOutput[1] = 0.0;
  lowpassState = 0.0;
}

MY_FLOAT NRev :: tick(MY_FLOAT input)
{
  MY_FLOAT temp, temp0, temp1, temp2, temp3;
  int i;

  temp0 = 0.0;
  for (i=0; i<6; i++) {
    temp = input + (combCoefficient[i] * combDelays[i]->lastOut());
    temp0 += combDelays[i]->tick(temp);
  }
  for (i=0; i<3; i++)	{
    temp = allpassDelays[i]->lastOut();
    temp1 = allpassCoefficient * temp;
    temp1 += temp0;
    allpassDelays[i]->tick(temp1);
    temp0 = -(allpassCoefficient * temp1) + temp;
  }

	// One-pole lowpass filter.
  lowpassState = 0.7*lowpassState + 0.3*temp0;
  temp = allpassDelays[3]->lastOut();
  temp1 = allpassCoefficient * temp;
  temp1 += lowpassState;
  allpassDelays[3]->tick(temp1);
  temp1 = -(allpassCoefficient * temp1) + temp;
    
  temp = allpassDelays[4]->lastOut();
  temp2 = allpassCoefficient * temp;
  temp2 += temp1;
  allpassDelays[4]->tick(temp2);
  lastOutput[0] = effectMix*(-(allpassCoefficient * temp2) + temp);
    
  temp = allpassDelays[5]->lastOut();
  temp3 = allpassCoefficient * temp;
  temp3 += temp1;
  allpassDelays[5]->tick(temp3);
  lastOutput[1] = effectMix*(-(allpassCoefficient * temp3) + temp);

  temp = (1.0 - effectMix) * input;
  lastOutput[0] += temp;
  lastOutput[1] += temp;
    
  return (lastOutput[0] + lastOutput[1]) * 0.5;

}
