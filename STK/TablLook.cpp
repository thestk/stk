/********************************************/
/*  Table Lookup Class,                     */
/*  by Gary P. Scavone, 1999                */ 
/*                                          */
/*  This class loads a table of floating    */
/*  point doubles, which are assumed to be  */
/*  in big-endian format.  Linear           */
/*  interpolation is performed for          */
/*  fractional lookup indexes.              */
/********************************************/

#include "TablLook.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifdef __LITTLE_ENDIAN__
  #include "swapstuf.h"
#endif

TablLook :: TablLook(char *fileName)
{
  extern double SwapDouble(double);

  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if (stat(fileName, &filestat) == -1)
    { /* Opening file failed */
      fprintf(stderr,"TablLook - Cannot access or find table lookup file: %s !!!\n",fileName);
      exit(0);
    }
  length = (long) filestat.st_size / 8;  // length in 8-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd)   {
    printf("TablLook - Couldn't open or find table lookup file %s !!!\n",fileName);
    exit(0);
  }

  data = (MY_FLOAT *) new MY_FLOAT[length];

  // Read samples into data[]
  long i = 0;
  double temp;
  while (fread(&temp,8,1,fd)) {
#ifdef __LITTLE_ENDIAN__
    temp = SwapDouble(temp);
#endif
    data[i++] = (MY_FLOAT) temp;
  }
  fclose(fd);

  lastOutput = 0.0;
}

TablLook :: ~TablLook()
{
  delete [ ] data;
}

long TablLook :: getLength()
{
  return length;
}

MY_FLOAT TablLook :: tick(MY_FLOAT index)
{
  static MY_FLOAT alpha;
  static long temp;

  if (index > length-1) {
    fprintf(stderr,"TablLook: Index (%f) exceeds table length ... sticking at end!\n", index);
    index = length-1;
  }
  if (index < 0.0) {
    fprintf(stderr,"TablLook: Index (%f) is less than zero ... setting to zero\n", index);
    index = 0.0;
  }

  // Index OK (in range 0 to length-1)
  temp = (long) index;                  // Integer part of index
  alpha = index - (MY_FLOAT) temp;      // Fractional part of index
  if (alpha > 0.0) {                    // Do linear interpolation
    lastOutput = data[temp];
    lastOutput += (alpha*(data[temp+1] - lastOutput));
  }
  else lastOutput = data[temp];

  return lastOutput;
}

MY_FLOAT TablLook :: lastOut()
{
  return lastOutput;
}
