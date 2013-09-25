/*******************************************/
/*  SndWvIn Input Class,                   */
/*  by Gary P. Scavone, 2000               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to open NeXT/Sun .snd 16-bit data */
/*  (signed integer) files for playback.   */
/*                                         */
/*  .snd files are always big-endian.      */
/*******************************************/

#include "SndWvIn.h"

#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

SndWvIn :: SndWvIn(char *fileName, const char *mode)
{
  char msg[256];

  // check mode string
  if ( strcmp(mode,"oneshot") && strcmp(mode,"looping") ) {
    sprintf(msg, "SndWvIn: constructor parameter 'mode' must be oneshot or looping only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // Open the file and get header info
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(msg, "SndWvIn: Couldn't open or find file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }

  // Make sure this is a .snd format file
  char magic[4];
  fseek(fd,0,SEEK_SET);   // Locate magic number in header
  fread(magic,4,1,fd);
  if (strncmp(magic,".snd",4)) {
    fclose(fd);
    sprintf(msg, "SndWvIn: %s doesn't appear to be an SND file.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Check that the data is not compressed
  INT32 format;
  fseek(fd,12,SEEK_SET);   // Locate format
  fread(&format,4,1,fd);
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&format);
#endif
  if (format != 3) { // 16-bit linear PCM = 3
    fclose(fd);
    sprintf(msg, "SndWvIn: STK does not currently support data formats other than 16 bit signed integer.\n");
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Get file sample rate from the header and set the default rate
  INT32 srate;
  fread(&srate,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  rate = (MY_FLOAT) (srate/SRATE);

  // Get number of channels from the header
  INT32 chans;
  fread(&chans,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&chans);
#endif
  channels = chans;

  fseek(fd,4,SEEK_SET);
  fread(&dataOffset,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&dataOffset);
#endif

  // Get length of data from the header
  fread(&fileSize,4,1,fd);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&fileSize);
#endif
  // fileSize is the number of sample frames
  fileSize /= 2 * channels;
  bufferSize = fileSize;

  if ((fileSize*channels) > MAX_FILE_LOAD_SIZE) {
    printf("\nSndWvIn: The .SND file (%s) has more than %d samples and\n",
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
  this->getData(0);    // Read samples into data[]

  if (fmod(rate, 1.0) != 0.0) interpolate = 1;
  else interpolate = 0;
  phaseOffset = (MY_FLOAT) 0.0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();

  // finally, let's normalize the data by default
  this->normalize();
}

SndWvIn :: ~SndWvIn()
{
}

void SndWvIn :: getData(long index)
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
  fread(buf, length*channels, 2, fd);
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
