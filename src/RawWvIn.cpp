/*******************************************/
/*  RawWvIn Input Class,                   */
/*  by Gary P. Scavone, 2000               */
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
  #include "ByteSwap.h"
#endif

RawWvIn :: RawWvIn(char *fileName, const char *mode)
{
  char msg[256];

  // check mode string
  if ( strcmp(mode,"oneshot") && strcmp(mode,"looping") ) {
    sprintf(msg, "RawWvIn: constructor parameter 'mode' must be oneshot or looping only.\n");
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  // Use the system call "stat" to determine the file length
  struct stat filestat;
  if (stat(fileName, &filestat) == -1) {
    // Opening file failed
    sprintf(msg, "RawWvIn: Couldn't stat or find file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }
  fileSize = (long) filestat.st_size / 2;  // length in 2-byte samples
  bufferSize = fileSize;

  if (fileSize > MAX_FILE_LOAD_SIZE) {
    printf("\nRawWvIn: The .WAV file (%s) has more than %d samples and\n",
           fileName, MAX_FILE_LOAD_SIZE);
    printf("will be loaded incrementally from disk.  Normalization will be disabled.\n");
    chunking = 1;
    bufferSize = LOAD_BUFFER_SIZE;
  }

  // Open the file and read samples into data[]
  fd = fopen(fileName,"rb");
  if (!fd)   {
    sprintf(msg, "RawWvIn: Couldn't open or find file (%s).\n", fileName);
    throw StkError(msg, StkError::FILE_NOT_FOUND);
  }

  // Setup for looping or one-shot playback
  if (!strcmp(mode,"looping"))
    looping = 1;
  else // default = oneshot
    looping = 0;

  channels = 1;                     // All STK rawwave files are mono
  data = (MY_FLOAT *) new MY_FLOAT[(bufferSize+1)*channels];

  fseek(fd,0,SEEK_SET);
  dataOffset = 0;
  this->getData(0);    // Read samples into data[]

  phaseOffset = (MY_FLOAT) 0.0;
  rate = (MY_FLOAT) 1.0;
  interpolate = 0;
  lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  this->reset();

  // finally, let's normalize the data by default
  this->normalize();
}

RawWvIn :: ~RawWvIn()
{
}

void RawWvIn :: getData(long index)
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

  fseek(fd, (long)(readPointer*2), SEEK_SET);
  long length = bufferSize;
  int end_of_file = (readPointer+bufferSize == fileSize);
  if (!end_of_file) length += 1;

  // Read samples into data[].  Use MY _FLOAT data structure to store INT16 samples
  INT16 *buf = (INT16 *)data;
  fread(buf, length, 2, fd);
  // Convert in place (unpack) to MY_FLOAT from the end of the array
  for (int i=length-1; i>=0; i--) {
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)(buf+i));
#endif
    data[i] = buf[i];
    if (chunking) data[i] *= 0.00003051;
  }

  // fill in the extra sample frame for interpolation
  if (end_of_file) {
    if (looping)
      data[bufferSize] = data[0];
    else
      data[bufferSize] = data[(bufferSize-1)];
  }

  if (!chunking) {
    fclose(fd);
    fd = 0;
  }
}
