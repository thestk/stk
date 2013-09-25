/*******************************************/
/*  NeXT Soundfile Output Class            */
/*  by Perry R. Cook, 1996                 */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This one opens a NeXT .snd file, and   */
/*  even knows how to byte-swap!           */
/*******************************************/

#include "SndWvOut.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
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

FILE *openNeXTFile(int chans,char *fileName)	{
  struct headerform hdr = {".sn",40,0,3,(INT32) SRATE,1,"Created by STK"};
  char tempName[128];
  FILE *fd;
    
  hdr.pref[3] = 'd';

  strcpy(tempName,fileName);
  if (strstr(tempName,".snd") == NULL) strcat(tempName,".snd");
  hdr.num_channels = chans;
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }
#ifdef __LITTLE_ENDIAN__
  hdr.hdr_length = SwapINT32 (hdr.hdr_length);
  hdr.file_length = SwapINT32 (hdr.file_length);
  hdr.mode = SwapINT32 (hdr.mode);
  hdr.samp_rate = SwapINT32 (hdr.samp_rate);
  hdr.num_channels = SwapINT32 (hdr.num_channels);
#endif
  printf("\nCreating soundfile %s\n\n", tempName);
  fwrite(&hdr,4,10,fd);
  return fd;
}

SndWvOut :: SndWvOut(char *fileName)
{
  channels = 1;
  fd = openNeXTFile(channels,fileName);
  counter = 0;
  totalCount = 0;
}

SndWvOut :: SndWvOut(int chans, char *fileName)
{
  if (chans > 24) {
    fprintf(stderr,"SndWvOut: Unsupported # of channels: %d\n", chans);
    exit(0);
  }
  channels = chans;
  fd = openNeXTFile(channels,fileName);
  counter = 0;
  totalCount = 0;
}

SndWvOut :: ~SndWvOut()
{
  double temp;

  fwrite(data,2,counter,fd);
  fseek(fd,8,SEEK_SET);
  temp = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n",temp);
  totalCount *= 2*channels;
#ifdef __LITTLE_ENDIAN__
  totalCount = SwapINT32 (totalCount);
#endif
  fwrite(&totalCount,4,1,fd);
  fclose(fd);
}

long SndWvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT SndWvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void SndWvOut :: tick(MY_FLOAT sample)
{
  INT16 isample;
    
  isample = (INT16) (sample * 32000.0);
#ifdef __LITTLE_ENDIAN__
  isample = SwapINT16 (isample);
#endif
  for (int i=0;i<channels;i++)
    data[counter++] = isample;

  totalCount++;
  if (counter == SND_BUFFER_SIZE) {
    fwrite(data,2,SND_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void SndWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++) {
    data[counter] = (INT16) (*samples++ * 32000.0);
#ifdef __LITTLE_ENDIAN__
    data[counter] = SwapINT16 (data[counter]);
#endif
    counter++;
  }
  
  totalCount++;
  if (counter == SND_BUFFER_SIZE) {
    fwrite(data,2,SND_BUFFER_SIZE,fd);
    counter = 0;
  }
}
