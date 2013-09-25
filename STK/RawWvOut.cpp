/*******************************************/
/*  RawWvOut Output Class                  */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object spits samples into a raw   */
/*  16-bit data (signed integer) file.     */
/*                                         */
/*  STK RawWave files are assumed to be    */
/*  monaural and big-endian.               */
/*******************************************/

#include "RawWvOut.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

RawWvOut :: RawWvOut(char *fileName)
{
  char tempName[128];

  channels = 1;
  strcpy(tempName,fileName);
  if (strstr(tempName,".raw") == NULL) strcat(tempName,".raw");
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }
  printf("\nCreating rawwave file %s\n\n", tempName);

  counter = 0;
  totalCount = 0;
}

RawWvOut :: RawWvOut(int chans, char *fileName)
{
  char tempName[128];

  if (chans > 1) {
    fprintf(stderr,"RawWvOut: Unsupported # of channels: %d\n", chans);
    exit(0);
  }
  channels = chans;
  strcpy(tempName,fileName);
  strcat(tempName,".raw");
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }
  printf("Creating soundfile %s.\n", tempName);

  counter = 0;
  totalCount = 0;
}

RawWvOut :: ~RawWvOut()
{
  double temp;

  fwrite(data,2,counter,fd);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n",temp);
  fclose(fd);
}

long RawWvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT RawWvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void RawWvOut :: tick(MY_FLOAT sample)
{
  data[counter] = (INT16) (sample * 32000.0);    
#ifdef __LITTLE_ENDIAN__
  data[counter] = SwapINT16 (data[counter]);
#endif
  counter++;

  totalCount++;
  if (counter == RAW_BUFFER_SIZE) {
    fwrite(data,2,RAW_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void RawWvOut :: mtick(MY_MULTI samples)
{
  data[counter] = (INT16) (*samples * 32000.0);
#ifdef __LITTLE_ENDIAN__
  data[counter] = SwapINT16 (data[counter]);
#endif
  counter++;
  
  totalCount++;
  if (counter == RAW_BUFFER_SIZE) {
    fwrite(data,2,RAW_BUFFER_SIZE,fd);
    counter = 0;
  }
}
