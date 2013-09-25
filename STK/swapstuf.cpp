#include "swapstuf.h"

INT32 SwapINT32(INT32 inf)
{
  INT32 o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[3]; outp[1]=inp[2]; outp[2]=inp[1]; outp[3]=inp[0];
  return(o);
}

INT16 SwapINT16(INT16 inf)
{
  INT16 o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[1]; outp[1]=inp[0];
  return(o);
}

float SwapFloat(float inf)
{
  float o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[3]; outp[1]=inp[2]; outp[2]=inp[1]; outp[3]=inp[0];
  return(o);
}

double SwapDouble(double inf)
{
  double o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[7]; outp[1]=inp[6]; outp[2]=inp[5]; outp[3]=inp[4];
  outp[4]=inp[3]; outp[5]=inp[2]; outp[6]=inp[1]; outp[7]=inp[0];
  return(o);
}
