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
  #include "ByteSwap.h"
#endif

RawWvOut :: RawWvOut(char *fileName, int chans)
{
  char tempName[128];
  char msg[128];

  if (chans != 1) {
    sprintf(msg, "RawWvOut: STK rawwave files are always monaural (channels = %d not supported)!\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }
  channels = chans;
  strcpy(tempName,fileName);
  if (strstr(tempName,".raw") == NULL) strcat(tempName,".raw");
  fd = fopen(tempName,"wb");
  if (!fd)   {
    sprintf(msg, "RawWvOut: Could not create soundfile: %s\n", tempName);
    throw StkError(msg, StkError::FILE_ERROR);
  }
  printf("\nCreating soundfile: %s\n", tempName);
  data_length = FILE_BUFFER_SIZE*channels;
  data = (INT16 *) new INT16[data_length];
}

RawWvOut :: ~RawWvOut()
{
  double temp;

  fwrite(data,2,counter,fd);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n\n", temp);
  fclose(fd);
}

void RawWvOut :: tick(MY_FLOAT sample)
{
  data[counter] = (INT16) (sample * 32000.0);    
#ifdef __LITTLE_ENDIAN__
  swap16 ((unsigned char *)&data[counter]);
#endif
  counter++;

  totalCount++;
  if (counter == data_length) {
    fwrite(data,2,data_length,fd);
    counter = 0;
  }
}

void RawWvOut :: mtick(MY_MULTI samples)
{
  data[counter] = (INT16) (*samples * 32000.0);
#ifdef __LITTLE_ENDIAN__
  swap16 ((unsigned char *)&data[counter]);
#endif
  counter++;
  
  totalCount++;
  if (counter == data_length) {
    fwrite(data,2,data_length,fd);
    counter = 0;
  }
}
