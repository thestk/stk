/*******************************************/
/*  WavWvIn Input Class,                   */
/*  by Gary P. Scavone, 2000               */
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
  #include "ByteSwap.h"
#endif

WavWvIn :: WavWvIn(char *fileName, const char *mode)
{
  char msg[256];

  // check mode string
  if ( strcmp(mode,"oneshot") && strcmp(mode,"looping") ) {
    sprintf(msg, "WavWvIn: constructor parameter 'mode' must be oneshot or looping only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // Open the file and get header info
  fd = fopen(fileName,"rb");
  if (!fd) {
    sprintf(msg, "WavWvIn: Couldn't open or find file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }

  // Make sure this is a .wav format file
  char id[4];
  fseek(fd,8,SEEK_SET);   // Locate wave id
  fread(&id,4,1,fd);
  if (strncmp(id,"WAVE",4)) {
    fclose(fd);
    sprintf(msg, "WavWvIn: %s doesn't appear to be a WAV file.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Find "format" chunk ... it must come before the "data" chunk
  INT32 chunkSize;
  fread(&id,4,1,fd);
  while (strncmp(id,"fmt ",4)) {
    fread(&chunkSize,4,1,fd);
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    fseek(fd,chunkSize,SEEK_CUR);
    fread(&id,4,1,fd);
  }
  
  // Check that the data is not compressed
  INT16 format_tag;
  fseek(fd,4,SEEK_CUR);   // Locate format tag ... jump over chunkSize
  fread(&format_tag,2,1,fd);
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&format_tag);
#endif
  if (format_tag != 1) { // PCM = 1
    fclose(fd);
    sprintf(msg, "WavWvIn: %s contains compressed data, which is not supported.\n", fileName);
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Get number of channels from the header
  INT16 temp;
  fread(&temp,2,1,fd);
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels = temp;

  // Get file sample rate from the header and set the default rate
  INT32 srate;
  fread(&srate,4,1,fd);
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  rate = (MY_FLOAT) (srate/SRATE);    // set default rate based on file sampling rate

  // Verify that the data is 16 bits per sample
  fseek(fd,6,SEEK_CUR);   // Locate bits_per_sample info
  fread(&temp,2,1,fd);
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  if (temp != 16) {
    fclose(fd);
    sprintf(msg, "WavWvIn: STK does not currently support data formats other than 16 bit signed integer.\n");
    throw StkError(msg, StkError::FILE_ERROR);
  }

  // Find "data" chunk ... it must come after the "fmt" chunk
  fread(&id,4,1,fd);
  while (strncmp(id,"data",4)) {
    fread(&chunkSize,4,1,fd);
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    fseek(fd,chunkSize,SEEK_CUR);
    fread(&id,4,1,fd);
  }

  // Get length of data from the header
  INT32 bytes;
  fread(&bytes,4,1,fd);
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  // length is the number of sample frames
  fileSize = bytes / 2 / channels;
  bufferSize = fileSize;

  if ((fileSize*channels) > MAX_FILE_LOAD_SIZE) {
    printf("\nWavWvIn: The .WAV file (%s) has more than %d samples and\n",
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

WavWvIn :: ~WavWvIn()
{
}

void WavWvIn :: getData(long index)
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
#ifndef __LITTLE_ENDIAN__
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
