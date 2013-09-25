/*******************************************/
/*  Noise Generator Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#include "Noise.h"    
#ifdef __OS_NeXT_
  #include <libc.h>
#endif

Noise :: Noise() : Object()
{    
  lastOutput = (MY_FLOAT) 0.0;
}

Noise :: ~Noise()
{
}

MY_FLOAT Noise :: tick()
{
#if defined(__OS_Win_) /* For Windoze */
  lastOutput = (MY_FLOAT) (rand() - (int)RANDLIMIT_OVER_TWO);
#else /* This is for Linux, NeXT and SGI */
  lastOutput = (MY_FLOAT) (random() - (int)RANDLIMIT_OVER_TWO);
#endif

  lastOutput *= (MY_FLOAT) ONE_OVER_RANDLIMIT;
  return lastOutput;
}

MY_FLOAT Noise :: lastOut()
{
  return lastOutput;
}

