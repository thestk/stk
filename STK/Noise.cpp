/*******************************************/
/*  Noise Generator Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#include "Noise.h"    
#ifdef __OS_NeXT_
  #include <libc.h>
#endif

#if defined(__OS_Win_) /* For Windoze */
  #define ONE_OVER_RANDLIMIT 0.00006103516
#else /* This is for Linux, NeXT and SGI */
  #define ONE_OVER_RANDLIMIT 0.00000000093132258
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
  lastOutput = (MY_FLOAT) (rand() - 16383);
#else /* This is for Linux, NeXT and SGI */
  lastOutput = (MY_FLOAT) random() - 1073741823.0;
#endif

  lastOutput *= (MY_FLOAT) ONE_OVER_RANDLIMIT;
  return lastOutput;
}

MY_FLOAT Noise :: lastOut()
{
  return lastOutput;
}

/************  Test Main  ************************/
/*
void main()
{
    long i;
    Noise test;
    for (i=0;i<20;i++) printf("%lf\n",test.tick());
}
*/
