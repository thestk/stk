/*******************************************/
/*  WavWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open DOS/Windows .wav 16-bit   */
/*  data (signed integer) files for        */
/*  playback.                              */
/*                                         */
/*  .wav files are always little-endian.   */
/*******************************************/

#include "WavWvIn.h"

#ifndef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

WavWvIn :: WavWvIn(char *fileName, char *mode)
{
  // Open the file and get header info
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd)   {
    printf("Couldn't open or find .wav file %s !!!\n",fileName);
    exit(0);
  }

  // Make sure this is a .wav format file
  char wave[4];
  fseek(fd,8,SEEK_SET);   // Locate wave id
  fread(&wave,4,1,fd);
  if (strncmp(wave,"WAVE",4)) {
    printf("This doesn't appear to be a .wav file %s !!!\n",fileName);
    exit(0);
  }

  // Get number of channels from the header
  INT16 temp;
  fseek(fd,22,SEEK_SET);   // Locate channels in header
  fread(&temp,2,1,fd);
#ifndef __LITTLE_ENDIAN__
  temp = SwapINT16(temp);
#endif
  channels = temp;

  // Get length of data from the header
  INT32 bytes;
  fseek(fd,40,SEEK_SET);   // Locate data length in header
  fread(&bytes,4,1,fd);
#ifndef __LITTLE_ENDIAN__
  bytes = SwapINT32(bytes);
#endif
  length = bytes / 2 / channels;   // length in 2-byte samples
  data = (MY_FLOAT *) new MY_FLOAT[(length+1)*channels];

  // Read samples into data[]
  long i = 0;
  while (fread(&temp,2,1,fd)) {
#ifndef __LITTLE_ENDIAN__
    temp = SwapINT16 (temp);
#endif
    data[i++] = (MY_FLOAT) temp;
  }

  // Get file sample rate from the header and set the default rate
  INT32 srate;
  fseek(fd,24,SEEK_SET);   // Locate sample rate in header
  fread(&srate,4,1,fd);
#ifndef __LITTLE_ENDIAN__
  srate = SwapINT32(srate);
#endif
  rate = (MY_FLOAT) (srate/SRATE);    // set default rate based on file sampling rate
  fclose(fd);

  // Setup for looping or one-shot playback
  if (!strcmp(mode,"looping")) {
    looping = 1;
    for (int j=0; j<channels; j++)
      data[length*channels+j] = data[j];            // extra sample for interpolation
  }
  else if (!strcmp(mode,"oneshot")) {
    looping = 0;
    for (int j=0; j<channels; j++)
      data[length*channels+j] = data[(length-1)*channels+j];  // extra sample for interpolation
  }
  else {
    fprintf(stderr,"ERROR: Unsupported WavWvIn mode: %s\n",mode);
    free(data);
    exit(0);
  }

  if (fmod(rate, 1.0) > 0.0) interpolate = 1;
  else interpolate = 0;
  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  finished = 0;
  lastOutput = (MY_FLOAT *) calloc(channels, sizeof(MY_FLOAT));
}

WavWvIn :: ~WavWvIn()
{
}
