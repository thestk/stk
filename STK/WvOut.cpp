/********************************************/
/*  WvOut Abstract Class,                   */
/*  by Tim Stilson, 1996                    */
/*  revised by Gary P. Scavone, 1999        */
/*                                          */
/*  This class can handle multi-channel     */
/*  data via the mtick() method.            */
/********************************************/

#include "WvOut.h"
#include <stdio.h>

WvOut :: WvOut()
{
}

WvOut :: ~WvOut()
{
}

void WvOut :: tick(MY_FLOAT sample)
{
}

void WvOut :: mtick(MY_MULTI samples)
{
}
