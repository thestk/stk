/*******************************************/
/*  SndWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open NeXT/Sun .snd 16-bit data */
/*  (signed integer) files for playback.   */
/*                                         */
/*  .snd files are always big-endian.      */
/*******************************************/

#include "SndWvIn.h"

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

SndWvIn :: SndWvIn(char *fileName, char *mode)
{
  // Open the file and get header info
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd)   {
    printf("Couldn't open or find .snd file %s !!!\n",fileName);
    exit(0);
  }

  // Make sure this is a .snd format file
  char magic[4];
  fseek(fd,0,SEEK_SET);   // Locate magic number in header
  fread(magic,4,1,fd);
  if (strncmp(magic,".snd",4)) {
    printf("This doesn't appear to be a .snd file %s !!!\n",fileName);
    exit(0);
  }

  // Get number of channels from the header
  INT32 int32temp;
  fseek(fd,20,SEEK_SET);
  fread(&int32temp,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  int32temp = SwapINT32(int32temp);
#endif
  channels = int32temp;

  // Get length of data from the header
  fseek(fd,8,SEEK_SET);
  fread(&int32temp,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  int32temp = SwapINT32(int32temp);
#endif
  length = int32temp / 2 / channels;  // channel length in 2-byte samples
  data = (MY_FLOAT *) new MY_FLOAT[(length+1)*channels];

  // Read samples into data[]
  INT16 temp;
  long i = 0;
  fseek(fd,40,SEEK_SET);
  while (fread(&temp,2,1,fd)) {
#ifdef __LITTLE_ENDIAN__
    temp = SwapINT16 (temp);
#endif
    data[i++] = (MY_FLOAT) temp;
  }

  // Get file sample rate from the header and set the default rate
  fseek(fd,16,SEEK_SET);
  fread(&int32temp,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  int32temp = SwapINT32(int32temp);
#endif
  rate = (MY_FLOAT) (int32temp/SRATE);
  fclose(fd);

  // Setup for looping or one-shot playback
  if (!strcmp(mode,"looping")) {
    looping = 1;
    for (int j=0; j<channels; j++)
      data[length*channels+j] = data[j];     // extra sample for interpolation
  }
  else if (!strcmp(mode,"oneshot")) {
    looping = 0;
    for (int j=0; j<channels; j++)
      data[length*channels+j] = data[(length-1)*channels+j];  // extra sample for interpolation
  }
  else {
    fprintf(stderr,"ERROR: Unsupported SndWvIn mode: %s\n",mode);
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

SndWvIn :: ~SndWvIn()
{
}
