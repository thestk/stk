/*************************************************/
/*
  STK Error Handling Class
  by Gary P. Scavone, 2000.
  
  This is a fairly abstract exception handling
  class.  There could be sub-classes to take
  care of more specific error conditions ... or not.
*/
/*************************************************/

#include "StkError.h"

StkError :: StkError(const char *p, TYPE tipe)
{
  type = tipe;
  strncpy(errormsg, p, 256);
}

StkError :: ~StkError()
{
}

void StkError :: printMessage()
{
  printf("\n%s\n", errormsg);
}

