/*******************************************/
/*  Wave file Output Class,                */
/*  by Perry R. Cook, 1995-96              */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This Object opens a DOS/Windows .wav   */
/*  16bit data (signed integers) file, and */
/*  poke buffers of samples into it.       */
/*******************************************/

#include "WavWvOut.h"
#ifndef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

/********  Wav Soundfile Header Struct   *******/
struct wavehdr {
  char  riff[4];          // "RIFF"
  INT32  file_size;       // in bytes
  char  wave[4];          // "WAVE"
  char  fmt[4];           // "fmt "
  INT32  block_size;      // in bytes (generally 16)
  INT16 format_tag;       // 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM
  INT16 num_chans;        // 1=mono, 2=stereo
  INT32  srate;
  INT32  bytes_per_sec;
  INT16 bytes_per_samp;   // 2=16-bit mono, 4=16-bit stereo
  INT16 bits_per_samp;
  char  data[4];          // "data"
  INT32  dlength;         // in bytes
};

FILE *openWAVFile(int chans,char *fileName)	{
  struct wavehdr hdr = {"RIF",44,"WAV","fmt",
						16,1,1,(INT32) SRATE,(INT32) SRATE*2,2,16,"dat",0};
  char tempName[128];
  FILE *fd;

  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
    
  strcpy(tempName,fileName);
  if (strstr(tempName,".wav") == NULL) strcat(tempName,".wav");
  hdr.num_chans = chans;
  hdr.bytes_per_sec = (long) SRATE*2*chans;
  hdr.bytes_per_samp = 2*chans;
  hdr.bits_per_samp = 16;
  fd = fopen(tempName,"wb");
  if (!fd)   {
    printf("Couldn't create soundfile %s  !!!!!!!!\n",fileName);
    exit(0);
  }

#ifndef __LITTLE_ENDIAN__
  hdr.file_size = SwapINT32(hdr.file_size);
  hdr.block_size = SwapINT32(hdr.block_size);
  hdr.format_tag = SwapINT16(hdr.format_tag);
  hdr.num_chans = SwapINT16(hdr.num_chans);
  hdr.srate = SwapINT32(hdr.srate);
  hdr.bytes_per_sec = SwapINT32(hdr.bytes_per_sec);
  hdr.bytes_per_samp = SwapINT16(hdr.bytes_per_samp);
  hdr.bits_per_samp = SwapINT16(hdr.bits_per_samp);
#endif

  printf("\nCreating soundfile %s\n\n", tempName);
  fwrite(&hdr,4,11,fd);
  return fd;
}

WavWvOut :: WavWvOut(char *fileName)
{
  channels = 1;
  fd = openWAVFile(channels,fileName);
  counter = 0;
  totalCount = 0;
}

WavWvOut :: WavWvOut(int chans, char *fileName)
{
  if (chans > 2) {
    fprintf(stderr,"WavWvOut: Unsupported # of channels: %d\n", chans);
    exit(0);
  }
  channels = chans;
  fd = openWAVFile(chans,fileName);
  counter = 0;
  totalCount = 0;
}

WavWvOut :: ~WavWvOut()
{
  MY_FLOAT time;
  INT32 bytes;

  fwrite(data,2,counter,fd);
  fseek(fd,40,SEEK_SET);
  time = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n",time);
  bytes = totalCount*2*channels;
#ifndef __LITTLE_ENDIAN__
  bytes = SwapINT32(bytes);
#endif
  fwrite(&bytes,4,1,fd);
  bytes = totalCount*2*channels + 44;
#ifndef __LITTLE_ENDIAN__
  bytes = SwapINT32(bytes);
#endif
  fseek(fd,4,SEEK_SET);
  fwrite(&bytes,4,1,fd);
  fclose(fd);
}

long WavWvOut :: getCounter()
{
  return totalCount;
}

MY_FLOAT WavWvOut :: getTime()
{
  return (MY_FLOAT) totalCount * ONE_OVER_SRATE;
}

void WavWvOut :: tick(MY_FLOAT sample)
{
  INT16 isample;

  isample = (INT16) (sample * 32000.0);
#ifndef __LITTLE_ENDIAN__
  isample = SwapINT16 (isample);
#endif
  for (int i=0;i<channels;i++)
    data[counter++] = isample;

  totalCount++;
  if (counter == WAV_BUFFER_SIZE) {
    fwrite(data,2,WAV_BUFFER_SIZE,fd);
    counter = 0;
  }
}

void WavWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++) {
    data[counter] = (INT16) (*samples++ * 32000.0);
#ifndef __LITTLE_ENDIAN__
    data[counter] = SwapINT16 (data[counter]);
#endif
    counter++;
  }

  totalCount++;
  if (counter == WAV_BUFFER_SIZE) {
    fwrite(data,2,WAV_BUFFER_SIZE,fd);
    counter = 0;
  }
}
