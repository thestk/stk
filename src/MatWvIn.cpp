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
#include "ByteSwap.h"

MatWvIn :: MatWvIn(char *fileName, char *mode)
{
  char msg[256];

  // check mode string
  if ( strcmp(mode,"oneshot") && strcmp(mode,"looping") ) {
    sprintf(msg, "MatWvIn: constructor parameter 'mode' must be oneshot or looping only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // Open the file and get header info
  fd = fopen(fileName,"rb");
  if (!fd)   {
    sprintf(msg, "MatWvIn: Couldn't open or find MAT-file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }

  // Make sure this is a version 5 MAT-file format and find its endian-ness
  char head[4];
  fseek(fd,0,SEEK_SET);
  fread(&head,4,1,fd);   // If any of the first 4 characters of the header = 0,
  if (strstr(head,"0")) { // then this is a Version 4 MAT-file.
    fclose(fd);
    sprintf(msg, "MatWvIn: %s appears to be a Version 4 \nMAT-file, which is not currently supported.\n",
            fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }
  char mi[2];
  doSwap = 0;
  fseek(fd,126,SEEK_SET);   // Locate "M" and "I" characters in header
  fread(&mi,2,1,fd);
#ifdef __LITTLE_ENDIAN__
  if (!strncmp(mi,"MI",2)) {
    doSwap = 1;
  } else if (strncmp(mi,"IM",2)) {
    fclose(fd);
    sprintf(msg, "MatWvIn: %s doesn't appear to be a MAT-file.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }
#else
  if (!strncmp(mi,"IM",2)) {
    doSwap = 1;
  } else if (strncmp(mi,"MI",2)) {
    fclose(fd);
    sprintf(msg, "MatWvIn: %s doesn't appear to be a MAT-file.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }
#endif

  // Check the data element type
  INT32 datatype;
  fread(&datatype,4,1,fd);
  if (doSwap) swap32((unsigned char *)&datatype);
  if (datatype != 14) {
    fclose(fd);
    sprintf(msg, "MatWvIn: I'm expecting a single array (or matrix) data element.\n");
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Check the array data type
  INT32 tmp;
  INT32 size;
  fseek(fd,168,SEEK_SET);
  fread(&tmp,4,1,fd);
  if (doSwap) swap32((unsigned char *)&tmp);
  if (tmp == 1) {  // array name > 4 characters
    fread(&tmp,4,1,fd);  // get array name length
    if (doSwap) swap32((unsigned char *)&tmp);
    size = (INT32) ceil((float)tmp/8);
    fseek(fd,size*8,SEEK_CUR);  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    fseek(fd,4,SEEK_CUR);
  }
  fread(&tmp,4,1,fd);
  if (doSwap) swap32((unsigned char *)&tmp);
  if (tmp != 9) {
    fclose(fd);
    sprintf(msg, "MatWvIn: I'm expecting the array data to be in double precision floating-point format.\n");
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Get number of rows from the header
  INT32 rows;
  fseek(fd,160,SEEK_SET);
  fread(&rows,4,1,fd);
  if (doSwap) swap32((unsigned char *)&rows);

  // Get number of columns from the header
  INT32 columns;
  fread(&columns,4,1,fd); // columns
  if (doSwap) swap32((unsigned char *)&columns);

  // Make channels = smaller of rows or columns
  if (rows < columns) {
    channels = rows;
    fileSize = columns;
    interleaved = 1;
  }
  else {
    channels = columns;
    fileSize = rows;
    interleaved = 0;
  }
  bufferSize = fileSize;

  if ((fileSize*channels) > MAX_FILE_LOAD_SIZE) {
    printf("\nMatWvIn: The MAT-file (%s) has more than %d samples and\n",
           fileName, MAX_FILE_LOAD_SIZE);
    printf("will be loaded incrementally from disk.  Normalization will be disabled.\n");
    chunking = 1;
    bufferSize = LOAD_BUFFER_SIZE;
  }

  // Setup for looping or one-shot playback
  if (!strcmp(mode,"looping"))
    looping = 1;
  else // default = oneshot
    looping = 0;

  data = (MY_FLOAT *) new MY_FLOAT[(bufferSize+1)*channels];

  // Move read pointer to the data in the file
  INT32 headsize;
  fseek(fd,132,SEEK_SET);
  fread(&headsize,4,1,fd); // file size from 132nd byte
  if (doSwap) swap32((unsigned char *)&headsize);
  headsize -= fileSize * 8 * channels;
  fseek(fd,headsize,SEEK_CUR);
  dataOffset = ftell(fd);

  this->getData(0);    // Read samples into data[]

  rate = (MY_FLOAT) 1.0;
  interpolate = 0;
  phaseOffset = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();
}

MatWvIn :: ~MatWvIn()
{
}

void MatWvIn :: getData(long index)
{
  /* Compare index to current readPointer and modify as needed.
   * The following while() loops will only execute on calls subsequent
   * to class instantiation ... and thus, only when "chunking".
   */
  while (index < readPointer) {
    readPointer -= LOAD_BUFFER_SIZE;
    bufferSize = LOAD_BUFFER_SIZE;
    if (readPointer < 0) {
      bufferSize += readPointer;
      readPointer = 0;
    }
  }
  while (index >= readPointer+bufferSize) {
    readPointer += LOAD_BUFFER_SIZE;
    bufferSize = LOAD_BUFFER_SIZE;
    if (readPointer+LOAD_BUFFER_SIZE >= fileSize) {
      bufferSize = fileSize - readPointer;
    }
  }

  long length = bufferSize;
  int end_of_file = (readPointer+bufferSize == fileSize);
  if (!end_of_file) length += 1;

  // Read samples into data[].  Use MY _FLOAT data structure to store doubles
  double *buf = (double *)data;
  if (interleaved) {
    fseek(fd, dataOffset+(long)(readPointer*channels*8), SEEK_SET);
    fread(data, 8, length*channels, fd);
    for (int i=length*channels-1; i>=0; i--) {
      if (doSwap)
        swap64((unsigned char *)(buf+i));
      data[i] = buf[i];
    }
  }
  else {
    long i = 0;
    long j = 0;
    double temp;
    fseek(fd, dataOffset+(long)(readPointer*8), SEEK_SET);
    while (j < channels) {
      fread(&temp,8,1,fd);
      if (doSwap) swap64((unsigned char *)&temp);
      data[channels*i+j] = (MY_FLOAT) temp;
      i++;
      if (i>=length) {
        i = 0;
        j++;
        fseek(fd, dataOffset+(long)(((j*fileSize)+readPointer)*8), SEEK_SET);
      }
    }
  }

  // fill in the extra sample frame for interpolation
  if (end_of_file) {
    for (int j=0; j<channels; j++)
      if (looping)
        data[bufferSize*channels+j] = data[j];
      else
        data[bufferSize*channels+j] = data[(bufferSize-1)*channels+j];
  }

  if (!chunking) {
    fclose(fd);
    fd = 0;
  }
}
