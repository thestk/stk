/*******************************************/
/*
  AifWvIn Input Class,
  by Gary P. Scavone, 2000

  This object inherits from WvIn and is
  used to open Audio Interchange File
  Format files with 16-bit data (signed
  integer) for playback.

  .aif files are always bif-endian.
*/
/*******************************************/

#include "AifWvIn.h"

#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

AifWvIn :: AifWvIn(char *fileName, const char *mode)
{
  char msg[256];

  // check mode string
  if ( strcmp(mode,"oneshot") && strcmp(mode,"looping") ) {
    sprintf(msg, "AifWvIn: constructor parameter 'mode' must be oneshot or looping only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // Open the file and get header info
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(msg, "AifWvIn: Couldn't open or find file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }

  // Make sure this is an .aif format file
  char id[4];
  fseek(fd,8,SEEK_SET); // Locate form type
  fread(&id,4,1,fd);
  if (strncmp(id,"AIFF",4)) {
    fclose(fd);
    sprintf(msg, "AifWvIn: %s doesn't appear to be an AIFF file.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Find "common" chunk
  INT32 chunkSize;
  fread(&id,4,1,fd);
  while (strncmp(id,"COMM",4)) {
    fread(&chunkSize,4,1,fd);
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    fseek(fd,chunkSize,SEEK_CUR);
    fread(&id,4,1,fd);
  }

  // Get number of channels from the header
  INT16 temp;
  fseek(fd,4,SEEK_CUR); // Jump over chunk size
  fread(&temp,2,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels = temp;

  // Get length of data from the header
  INT32 frames;
  fread(&frames,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  // length is the number of sample frames
  fileSize = frames;
  bufferSize = fileSize;

  // Verify that the data is 16 bits per sample
  fread(&temp,2,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  if (temp != 16) {
    fclose(fd);
    sprintf(msg, "AifWvIn: STK does not currently support data formats other than 16 bit signed integer.\n");
    throw StkError(msg, StkError::FILE_ERROR);
  }

  /* Get file sample rate from the header and set the default rate.
   * For AIFF files, this value is stored in a 10-byte, IEEE Standard
   * 754 floating point number, so we need to convert it first.
   */
  unsigned char srate[10];
  unsigned char exp;
  unsigned long mantissa;
  unsigned long last = 0;;
  fread(&srate,10,1,fd);
  mantissa = (unsigned long) *(unsigned long *)(srate+2);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&mantissa);
#endif
  exp = 30 - *(srate+1);
  while (exp--) {
    last = mantissa;
    mantissa >>= 1;
  }
  if (last & 0x00000001) mantissa++;
  rate = (MY_FLOAT) (mantissa/SRATE);    // set default rate based on file sampling rate

  // Find "data" chunk
  fread(&id,4,1,fd);
  while (strncmp(id,"SSND",4)) {
    fread(&chunkSize,4,1,fd);
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    fseek(fd,chunkSize,SEEK_CUR);
    fread(&id,4,1,fd);
  }

  // Skip over chunk size, offset, and blocksize fields
  fseek(fd,12,SEEK_CUR);

  if ((fileSize*channels) > MAX_FILE_LOAD_SIZE) {
    printf("\nAifWvIn: The .AIF file (%s) has more than %d samples and\n",
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
  dataOffset = ftell(fd);
  this->getData(0);    // Read samples into data[]

  if (fmod(rate, 1.0) != 0.0) interpolate = 1;
  else interpolate = 0;
  phaseOffset = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();

  // finally, let's normalize the data by default
  this->normalize();
}

AifWvIn :: ~AifWvIn()
{
}

void AifWvIn :: getData(long index)
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

  fseek(fd, dataOffset+(long)(readPointer*channels*2), SEEK_SET);
  long length = bufferSize;
  int end_of_file = (readPointer+bufferSize == fileSize);
  if (!end_of_file) length += 1;

  // Read samples into data[].  Use MY _FLOAT data structure to store INT16 samples
  INT16 *buf = (INT16 *)data;
  fread(buf, 2, length*channels, fd);
  // Convert in place (unpack) to MY_FLOAT from the end of the array
  for (int i=length*channels-1; i>=0; i--) {
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)(buf+i));
#endif
    data[i] = buf[i];
    if (chunking) data[i] *= 0.00003051;
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
