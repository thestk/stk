/*******************************************/
/*  Noise Generator Class,                 */
/*  by Perry R. Cook, 1995-96              */ 
/*  White noise as often as you like.      */
/*******************************************/

#include "Noise.h"    

Noise :: Noise() : Object()
{    
    lastOutput = 0.0;
}

Noise :: ~Noise()
{

}

/*     THIS IS FOR TURBO C                      */
/*    Constant = 1.0 / 16384.0                  */
/*    #define ONE_OVER_RANDLIMIT 0.00006103516  */


/*     THIS IS FOR UNIX, NeXT and SGI    */
    #define ONE_OVER_RANDLIMIT 0.00000000093132258

MY_FLOAT Noise :: tick()
{
/*        THIS ONE IS TURBO C  */
/*      lastOutput = (MY_FLOAT) random(32767) - 16384.0;  */

/*     THIS IS FOR UNIX, NeXT and SGI    */

    lastOutput = (MY_FLOAT) random() - 1073741823.0;
    lastOutput *= ONE_OVER_RANDLIMIT;
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
