/*******************************************/
/*  RawWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open raw 16-bit data (signed   */
/*  integer) files for playback.           */
/*                                         */
/*  STK RawWave files are assumed to be    */
/*  monaural and big-endian.               */
/*******************************************/

#include "RawWvIn.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

RawWvIn :: RawWvIn(char *fileName, char *mode)
{
  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if (stat(fileName, &filestat) == -1)
    { /* Opening file failed */
      fprintf(stderr,"Cannot access or find rawwave file: %s !!!\n",fileName);
      exit(0);
    }
  length = (long) filestat.st_size / 2;  // length in 2-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd)   {
    printf("Couldn't open or find rawwave file %s !!!\n",fileName);
    exit(0);
  }

  channels = 1;                     // All STK rawwave files are mono
  data = (MY_FLOAT *) new MY_FLOAT[(length+1)*channels];

  long i = 0;
  INT16 temp;
  fseek(fd,0,SEEK_SET);  // Only here to bypass bug in Linux glibc 2.1x (RedHat 6.0)
  while (fread(&temp,2,1,fd)) {
#ifdef __LITTLE_ENDIAN__
    temp = SwapINT16 (temp);
#endif
    data[i++] = (MY_FLOAT) temp;
  }
  fclose(fd);

  // Setup for looping or one-shot playback
  if (!strcmp(mode,"looping")) {
    looping = 1;
    data[length] = data[0];          // extra sample for interpolation
  }
  else if (!strcmp(mode,"oneshot")) {
    looping = 0;
    data[length] = data[length-1];  // extra sample for interpolation
  }
  else {
    fprintf(stderr,"ERROR: Unsupported RawWvIn mode: %s\n",mode);
    free(data);
    exit(0);
  }

  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 1.0;
  interpolate = 0;
  finished = 0;
  lastOutput = (MY_FLOAT *) calloc(channels, sizeof(MY_FLOAT));
}

RawWvIn :: ~RawWvIn()
{
}
