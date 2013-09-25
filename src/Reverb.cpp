/***************************************************/
/*! \class Reverb
    \brief STK abstract reverberator parent class.

    This class provides common functionality for
    STK reverberator subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Reverb.h"
#include <math.h>

Reverb :: Reverb()
{
}

Reverb :: ~Reverb()
{
}

void Reverb :: setEffectMix(MY_FLOAT mix)
{
  effectMix = mix;
}

MY_FLOAT Reverb :: lastOut() const
{
  return (lastOutput[0] + lastOutput[1]) * 0.5;
}

MY_FLOAT Reverb :: lastOutLeft() const
{
  return lastOutput[0];
}

MY_FLOAT Reverb :: lastOutRight() const
{
  return lastOutput[1];
}

MY_FLOAT *Reverb :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

bool Reverb :: isPrime(int number)
{
  if (number == 2) return true;
  if (number & 1)	{
	  for (int i=3; i<(int)sqrt((double)number)+1; i+=2)
		  if ( (number % i) == 0) return false;
	  return true; /* prime */
	}
  else return false; /* even */
}
