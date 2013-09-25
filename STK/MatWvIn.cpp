/*******************************************/
/*  MatWvIn Input Class,                   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open Matlab MAT-file data      */
/*  (doubles) files for playback.  In      */
/*  order for this class to work, the      */
/*  MAT-file must contain a single array   */
/*  (matrix) of double-precision floating  */
/*  point values (can be multi-channel).   */
/*  It does not work for any other data    */
/*  formats.                               */
/*                                         */
/*  MAT-file data is either big- or        */
/*  little-endian, which can be determined */
/*  from the header.                       */
/*******************************************/

#include "MatWvIn.h"
#include "swapstuf.h"

MatWvIn :: MatWvIn(char *fileName, char *mode)
{
  extern double SwapDouble(double);

  // Open the file and get header info
  FILE *fd;
  fd = fopen(fileName,"rb");
  if (!fd)   {
    printf("Couldn't open or find MAT-file %s !!!\n",fileName);
    exit(0);
  }

  // Make sure this is a version 5 MAT-file format and find its endian-ness
  char head[4];
  fseek(fd,0,SEEK_SET);
  fread(&head,4,1,fd);   // If any of the first 4 characters of the header = 0,
  if (strstr(head,"0")) { // then this is a Version 4 MAT-file.
    printf("This looks like a Version 4 MAT-file.  I don't support\n");
    printf("that at the moment, but you can add the code to MatWvIn if\n");
    printf("you really need it.\n");
    exit(0);
  }
  char mi[2];
  int swap = 0;
  fseek(fd,126,SEEK_SET);   // Locate "M" and "I" characters in header
  fread(&mi,2,1,fd);
#ifdef __LITTLE_ENDIAN__
  if (!strncmp(mi,"MI",2)) {
    swap = 1;
  } else if (strncmp(mi,"IM",2)) {
    printf("This doesn't appear to be a MAT-file %s !!!\n",fileName);
    exit(0);
  }
#else
  if (!strncmp(mi,"IM",2)) {
    swap = 1;
  } else if (strncmp(mi,"MI",2)) {
    printf("This doesn't appear to be a MAT-file %s !!!\n",fileName);
    exit(0);
  }
#endif

  // Check the data element type
  INT32 datatype;
  fread(&datatype,4,1,fd);
  if (swap) datatype = SwapINT32(datatype);
  if (datatype != 14) {
    printf("I'm expecting a single array (or matrix) data element.\n");
    printf("This doesn't appear to be the case with your data.  Sorry!\n");
    exit(0);
  }

  // Check the array data type
  INT32 tmp;
  INT32 size;
  fseek(fd,168,SEEK_SET);
  fread(&tmp,4,1,fd);
  if (swap) tmp = SwapINT32(tmp);
  if (tmp == 1) {  // array name > 4 characters
    fread(&tmp,4,1,fd);  // get array name length
    if (swap) tmp = SwapINT32(tmp);
    size = (INT32) ceil((float)tmp/8);
    fseek(fd,size*8,SEEK_CUR);  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    fseek(fd,4,SEEK_CUR);
  }
  fread(&tmp,4,1,fd);
  if (swap) tmp = SwapINT32(tmp);
  if (tmp != 9) {
    printf("I'm expecting the array data to be in double precision\n");
    printf("floating-point format. This doesn't appear to be the case\n");
    printf("with your data.  Sorry!\n");
    exit(0);
  }

  // Get number of rows from the header
  INT32 rows;
  fseek(fd,160,SEEK_SET);
  fread(&rows,4,1,fd);    // rows
  if (swap) rows = SwapINT32(rows);

  // Get number of columns from the header
  INT32 columns;
  fread(&columns,4,1,fd); // columns
  if (swap) columns = SwapINT32(columns);

  // Make channels = smaller of rows or columns
  if (rows < columns) {
    channels = rows;
    length = columns;
  }
  else {
    channels = columns;
    length = rows;
  }
  data = (MY_FLOAT *) new MY_FLOAT[(length+1)*channels];
  
  // Move read pointer to the data in the file
  INT32 headsize;
  fseek(fd,132,SEEK_SET);
  fread(&headsize,4,1,fd); // file size from 132nd byte
  if (swap) headsize = SwapINT32(headsize);
  headsize -= length * 8 * channels;
  fseek(fd,headsize,SEEK_CUR);

  // Read samples into data[]
  long i = 0;
  double temp;
  if (channels == rows) {
    while (fread(&temp,8,1,fd)) {
      if (swap) temp = SwapDouble(temp);
      data[i++] = (MY_FLOAT) temp;
    }
  } else {
    long j = 0;
    while (fread(&temp,8,1,fd)) {
      if (swap) temp = SwapDouble(temp);
      data[channels*i+j] = (MY_FLOAT) temp;
      i++;
      if (!(i<length)) {
        i = 0;
        j++;
      }
    }
  }
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
    fprintf(stderr,"ERROR: Unsupported MatWvIn mode: %s\n",mode);
    free(data);
    exit(0);
  }

  rate = (MY_FLOAT) 1.0;
  interpolate = 0;
  time = (MY_FLOAT) 0.0;
  phaseOffset = (MY_FLOAT) 0.0;
  finished = 0;
  lastOutput = (MY_FLOAT *) calloc(channels, sizeof(MY_FLOAT));
}

MatWvIn :: ~MatWvIn()
{
}
