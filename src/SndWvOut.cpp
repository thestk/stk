/***********************************************/
/*
  NeXT (.snd) and Sun (.au) Soundfile Output Class
  by Perry R. Cook, 1996
  Revised by Gary P. Scavone, 1999-2000

  This one opens a NeXT .snd file, and
  even knows how to byte-swap!
*/
/***********************************************/

#include "SndWvOut.h"
#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

/********  NeXT Soundfile Header Struct   *******/
struct headerform {
  char pref[4];
  INT32 hdr_length;
  INT32 file_length;
  INT32 mode;
  INT32 samp_rate;
  INT32 num_channels;
  char comment[16];
};

FILE *openNeXTFile(int chans, char *fileName)	{
  struct headerform hdr = {".sn",40,0,3,(INT32) SRATE,1,"Created by STK"};
  char tempName[128];
  FILE *fd;
  char msg[256];
    
  hdr.pref[3] = 'd';

  strcpy(tempName,fileName);
  if (strstr(tempName,".snd") == NULL) strcat(tempName,".snd");
  hdr.num_channels = chans;
  fd = fopen(tempName,"wb");
  if (!fd) {
    sprintf(msg, "SndWvOut: Could not create soundfile: %s\n", tempName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

#ifdef __LITTLE_ENDIAN__
  swap32 ((unsigned char *)&hdr.hdr_length);
  swap32 ((unsigned char *)&hdr.file_length);
  swap32 ((unsigned char *)&hdr.mode);
  swap32 ((unsigned char *)&hdr.samp_rate);
  swap32 ((unsigned char *)&hdr.num_channels);
#endif

  printf("\nCreating soundfile: %s\n", tempName);
  fwrite(&hdr,4,10,fd);
  return fd;
}

SndWvOut :: SndWvOut(char *fileName, int chans)
{
  char msg[256];
  if (chans < 1) {
    sprintf(msg, "SndWvOut: number of channels = %d not supported!\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }
  channels = chans;
  fd = openNeXTFile(channels,fileName);
  data_length = FILE_BUFFER_SIZE*channels;
  data = (INT16 *) new INT16[data_length];
}

SndWvOut :: ~SndWvOut()
{
  double temp;

  fwrite(data,2,counter,fd);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n\n", temp);

  totalCount *= 2*channels;
#ifdef __LITTLE_ENDIAN__
  swap32 ((unsigned char *)&totalCount);
#endif
  fseek(fd,8,SEEK_SET); // jump to data size
  fwrite(&totalCount,4,1,fd);
  fclose(fd);
}

void SndWvOut :: tick(MY_FLOAT sample)
{
  INT16 isample;
    
  isample = (INT16) (sample * 32000.0);
#ifdef __LITTLE_ENDIAN__
  swap16 ((unsigned char *)&isample);
#endif
  for (int i=0;i<channels;i++)
    data[counter++] = isample;

  totalCount++;
  if (counter == data_length) {
    fwrite(data,2,data_length,fd);
    counter = 0;
  }
}

void SndWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++) {
    data[counter] = (INT16) (*samples++ * 32000.0);
#ifdef __LITTLE_ENDIAN__
    swap16 ((unsigned char *)&data[counter]);
#endif
    counter++;
  }
  
  totalCount++;
  if (counter == data_length) {
    fwrite(data,2,data_length,fd);
    counter = 0;
  }
}
