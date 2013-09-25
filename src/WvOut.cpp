/********************************************/
/*  WvOut Abstract Class,                   */
/*  by Tim Stilson, 1996                    */
/*  revised by Gary P. Scavone, 1999-2000   */
/*                                          */
/*  This class can handle multi-channel     */
/*  data via the mtick() method.            */
/*                                          */
/*  Currently, WvOut and its subclasses are */
/*  non-interpolating.  Thus, the output    */
/*  rate is always SRATE (defined in        */
/*  Object.h).  A future upgrade could add  */
/*  interpolation functionality to allow    */
/*  output rates different than the STK     */
/*  internal processing rate (SRATE).       */
/********************************************/

#include "WvOut.h"

WvOut :: WvOut()
{
  data = 0;
  counter = 0;
  totalCount = 0;
}

WvOut :: ~WvOut()
{
  if (data) {
    delete [ ] data;
    data = 0;
  }
}

long WvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT WvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void WvOut :: tick(MY_FLOAT sample)
{
}

void WvOut :: mtick(MY_MULTI samples)
{
}
