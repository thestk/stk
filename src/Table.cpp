/***************************************************/
/*! \class Table
    \brief STK table lookup class.

    This class loads a table of floating-point
    doubles, which are assumed to be in big-endian
    format.  Linear interpolation is performed for
    fractional lookup indexes.

    An StkError will be thrown if the table file
    is not found.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Table.h"
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream.h>

Table :: Table(char *fileName)
{
  char message[256];

  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(message, "Table: Couldn't stat or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }
  length = (long) filestat.st_size / 8;  // length in 8-byte samples

  // Open the file and read samples into data[]
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(message, "Table: Couldn't open or find file (%s).", fileName);
    handleError( message, StkError::FILE_NOT_FOUND );
  }

  data = (MY_FLOAT *) new MY_FLOAT[length];

  // Read samples into data[]
  long i = 0;
  double temp;
  while ( fread(&temp, 8, 1, fd) ) {
#ifdef __LITTLE_ENDIAN__
    swap64((unsigned char *)&temp);
#endif
    data[i++] = (MY_FLOAT) temp;
  }
  fclose(fd);

  lastOutput = 0.0;
}

Table :: ~Table()
{
  delete [ ] data;
}

long Table :: getLength() const
{
  return length;
}

MY_FLOAT Table :: lastOut() const
{
  return lastOutput;
}

MY_FLOAT Table :: tick(MY_FLOAT index)
{
  MY_FLOAT alpha;
  long temp;

  if (index > length-1) {
    cerr << "Table: Index (" << index << ") exceeds table length ... sticking at end!" << endl;
    index = length-1;
  }
  else if (index < 0.0) {
    cerr << "Table: Index (" << index << ") is less than zero ... setting to zero!" << endl;
    index = 0.0;
  }

  // Index in range 0 to length-1
  temp = (long) index;                   // Integer part of index
  alpha = index - (MY_FLOAT) temp;      // Fractional part of index
  if (alpha > 0.0) {                    // Do linear interpolation
    lastOutput = data[temp];
    lastOutput += (alpha*(data[temp+1] - lastOutput));
  }
  else lastOutput = data[temp];

  return lastOutput;
}

MY_FLOAT *Table :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}
