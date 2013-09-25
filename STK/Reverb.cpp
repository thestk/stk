/********************************************/
/*  Reverb Abstract Class,                  */
/*  by Tim Stilson, 1998                    */
/*                                          */
/*  Integrated into STK by Gary Scavone     */
/*  with T60 argument.                      */
/********************************************/

#include "Reverb.h"

Reverb :: Reverb()
{
}

MY_FLOAT Reverb :: tick(MY_FLOAT sample)
{
  printf("Warning: Using virtual function Reverb :: tick()\n");
  return 0;
}

void Reverb :: setEffectMix(MY_FLOAT mix)
{
}

int Reverb :: isprime(int val)
{
  int i;

  if (val == 2) return 1;
  if (val & 1)
	{
	  for (i=3; i<(int)sqrt((double)val)+1; i+=2)
		{
		  if ((val%i) == 0) return 0;
		}
	  return 1; /* prime */
	}
  else return 0; /* even */
}
