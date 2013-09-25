/*******************************************/
/*
  AifWvOut Output Class,
  by Gary P. Scavone, 2000

  This object inherits from WvOut and is
  used to write Audio Interchange File
  Format files with 16-bit data (signed
  integer).

  .aif files are always bif-endian.
*/
/*******************************************/

#include "AifWvOut.h"
#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

/********  Aiff Soundfile Header Struct   *******/
struct aiffhdr {
  char form[4];                  // "FORM"
  INT32 form_size;               // in bytes
  char aiff[4];                  // "AIFF"
  char comm[4];                  // "COMM"
  INT32 comm_size;               // "COMM" chunk size (should be 18)
  INT16 num_chans;               // number of channels
  unsigned long sample_frames;    // sample frames of audio data
  INT16 sample_size;             // always 16 for STK
  unsigned char srate[10];        // IEEE 754 floating point format
  char  ssnd[4];                 // "SSND"
  INT32 ssnd_size;               // "SSND" chunk size
  unsigned long offset;           // data offset in data block (should be 0)
  unsigned long block_size;       // not used by STK (should be 0)
};

FILE *openAifFile(int chans, char *fileName)	{
  struct aiffhdr hdr = {"FOR",46,"AIF","COM",18,0,0,16,"0","SSN",8,0,0};
  char tempName[128];
  FILE *fd;
  char msg[256];
  INT16 i;
  unsigned long exp;
  unsigned long rate = (unsigned long) SRATE;

  hdr.form[3] = 'M';
  hdr.aiff[3] = 'F';
  hdr.comm[3] = 'M';
  hdr.ssnd[3] = 'D';

  hdr.num_chans = chans;

  /*
   * For AIFF files, the sample reate is stored in a 10-byte, IEEE Standard
   * 754 floating point number, so we need to convert to that.
   */
  memset(hdr.srate, 0, 10);
  exp = rate;
  for (i=0; i<32; i++) {
    exp >>= 1;
    if (!exp) break;
  }
  i += 16383;
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&i);
#endif
  *(INT16 *)(hdr.srate) = (INT16) i;

  for (i=32; i; i--) {
    if (rate & 0x80000000) break;
    rate <<= 1;
  }

#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&rate);
#endif
  *(unsigned long *)(hdr.srate+2) = (unsigned long) rate;
    
  strcpy(tempName,fileName);
  if (strstr(tempName,".aif") == NULL) strcat(tempName,".aif");
  fd = fopen(tempName,"wb");
  if (!fd) {
    sprintf(msg, "AifWvOut: Could not create soundfile: %s\n", tempName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&hdr.form_size);
  swap32((unsigned char *)&hdr.comm_size);
  swap16((unsigned char *)&hdr.num_chans);
  swap16((unsigned char *)&hdr.sample_size);
  swap32((unsigned char *)&hdr.ssnd_size);
  swap32((unsigned char *)&hdr.offset);
  swap32((unsigned char *)&hdr.block_size);
#endif

  printf("\nCreating soundfile: %s\n", tempName);

  /* I found it necessary to break the fwrite() calls as
   * follows ... a single write of 54 bytes didn't work.
   */
  fwrite(&hdr,4,5,fd);
  fwrite(&hdr.num_chans,2,1,fd);
  fwrite(&hdr.sample_frames,4,1,fd);
  fwrite(&hdr.sample_size,2,1,fd);
  fwrite(&hdr.srate,10,1,fd);
  fwrite(&hdr.ssnd,4,4,fd);

  return fd;
}

AifWvOut :: AifWvOut(char *fileName, int chans)
{
  char msg[256];
  if (chans < 1) {
    sprintf(msg, "AifWvOut: number of channels = %d not supported!\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }
  channels = chans;
  fd = openAifFile(chans,fileName);
  data_length = FILE_BUFFER_SIZE*channels;
  data = (INT16 *) new INT16[data_length];
}

AifWvOut :: ~AifWvOut()
{
  MY_FLOAT time;
  unsigned long bytes;
  unsigned long frames;

  fwrite(data,2,counter,fd);
  time = (double) totalCount * ONE_OVER_SRATE;
  printf("%f Seconds Computed\n\n", time);

  frames = (unsigned long) totalCount;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  fseek(fd,22,SEEK_SET); // jump to "COMM" sample_frames
  fwrite(&frames,4,1,fd);

  bytes = totalCount*2*channels + 46;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd,4,SEEK_SET); // jump to file size
  fwrite(&bytes,4,1,fd);

  bytes = totalCount*2*channels + 8;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd,42,SEEK_SET); // jump to "SSND" chunk size
  fwrite(&bytes,4,1,fd);

  fclose(fd);
}

void AifWvOut :: tick(MY_FLOAT sample)
{
  INT16 isample;

  isample = (INT16) (sample * 32000.0);
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&isample);
#endif
  for (int i=0;i<channels;i++)
    data[counter++] = isample;

  totalCount++;
  if (counter == data_length) {
    fwrite(data,2,data_length,fd);
    counter = 0;
  }
}

void AifWvOut :: mtick(MY_MULTI samples)
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
