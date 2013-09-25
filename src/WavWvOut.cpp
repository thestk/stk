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
  #include "ByteSwap.h"
#endif

/********  Wav Soundfile Header Struct   *******/
struct wavehdr {
  char  riff[4];          // "RIFF"
  INT32  file_size;       // in bytes
  char  wave[4];          // "WAVE"
  char  fmt[4];           // "fmt "
  INT32  block_size;      // in bytes (16 for PCM)
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
  char msg[256];

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
  if (!fd) {
    sprintf(msg, "WavWvOut: Could not create soundfile: %s\n", tempName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&hdr.file_size);
  swap32((unsigned char *)&hdr.block_size);
  swap16((unsigned char *)&hdr.format_tag);
  swap16((unsigned char *)&hdr.num_chans);
  swap32((unsigned char *)&hdr.srate);
  swap32((unsigned char *)&hdr.bytes_per_sec);
  swap16((unsigned char *)&hdr.bytes_per_samp);
  swap16((unsigned char *)&hdr.bits_per_samp);
#endif

  printf("\nCreating soundfile: %s\n", tempName);
  fwrite(&hdr,4,11,fd);
  return fd;
}

WavWvOut :: WavWvOut(char *fileName, int chans)
{
  char msg[256];
  if (chans < 1) {
    sprintf(msg, "WavWvOut: number of channels = %d not supported!\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }
  channels = chans;
  fd = openWAVFile(chans,fileName);
  data_length = FILE_BUFFER_SIZE*channels;
  data = (INT16 *) new INT16[data_length];
}

WavWvOut :: ~WavWvOut()
{
  MY_FLOAT time;
  INT32 bytes;

  fwrite(data,2,counter,fd);
  time = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n\n", time);

  bytes = totalCount*2*channels;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd,40,SEEK_SET); // jump to data length
  fwrite(&bytes,4,1,fd);

  bytes = totalCount*2*channels + 44;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd,4,SEEK_SET); // jump to file size
  fwrite(&bytes,4,1,fd);
  fclose(fd);
}

void WavWvOut :: tick(MY_FLOAT sample)
{
  static INT16 isample;

  isample = (INT16) (sample * 32000.0);
#ifndef __LITTLE_ENDIAN__
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

void WavWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++) {
    data[counter] = (INT16) (*samples++ * 32000.0);
#ifndef __LITTLE_ENDIAN__
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
