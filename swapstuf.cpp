int SwapInt (int inf)
{
  int o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[3]; outp[1]=inp[2]; outp[2]=inp[1]; outp[3]=inp[0];
  return(o);
}

short SwapShort (short inf)
{
  short o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[1]; outp[1]=inp[0];
  return(o);
}

float SwapFloat (float inf)
{
  float o;
  unsigned char *inp,*outp; 
  inp=(unsigned char *)&inf; 
  outp=(unsigned char *)&o;
  outp[0]=inp[3]; outp[1]=inp[2]; outp[2]=inp[1]; outp[3]=inp[0];
  return(o);
}
