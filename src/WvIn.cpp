/***************************************************/
/*! \class WvIn
    \brief STK audio data input base class.

    This class provides input support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvIn loads the contents of an audio file for
    subsequent output.  Linear interpolation is
    used for fractional "read rates".

    WvIn supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which return samples produced
    by averaging across sample frames, from the 
    tickFrame() methods, which return pointers to
    multi-channel sample frames.  For single-channel
    data, these methods return equivalent values.

    Small files are completely read into local memory
    during instantiation.  Large files are read
    incrementally from disk.  The file size threshold
    and the increment size values are defined in
    WvIn.h.

    WvIn currently supports WAV, AIFF, SND (AU),
    MAT-file (Matlab), and STK RAW file formats.
    Signed integer (8-, 16-, and 32-bit) and floating-
    point (32- and 64-bit) data types are supported.
    Uncompressed data types are not supported.  If
    using MAT-files, data should be saved in an array
    with each data channel filling a matrix row.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "WvIn.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>

#include <iostream>

WvIn :: WvIn()
{
  init();
}

WvIn :: WvIn( const char *fileName, bool raw, bool doNormalize )
{
  init();
  openFile( fileName, raw );
}

WvIn :: ~WvIn()
{
  if (fd)
    fclose(fd);

  if (data)
    delete [] data;

  if (lastOutput)
    delete [] lastOutput;
}

void WvIn :: init( void )
{
  fd = 0;
  data = 0;
  lastOutput = 0;
  chunking = false;
  finished = true;
  interpolate = false;
  bufferSize = 0;
  channels = 0;
  time = 0.0;
}

void WvIn :: closeFile( void )
{
  if ( fd ) fclose( fd );
  finished = true;
}

void WvIn :: openFile( const char *fileName, bool raw, bool doNormalize )
{
  closeFile();

  // Try to open the file.
  fd = fopen(fileName, "rb");
  if (!fd) {
    sprintf(msg, "WvIn: Could not open or find file (%s).", fileName);
    handleError(msg, StkError::FILE_NOT_FOUND);
  }

  unsigned long lastChannels = channels;
  unsigned long samples, lastSamples = (bufferSize+1)*channels;
  bool result = false;
  if ( raw )
    result = getRawInfo( fileName );
  else {
    char header[12];
    if ( fread(&header, 4, 3, fd) != 3 ) goto error;
    if ( !strncmp(header, "RIFF", 4) &&
         !strncmp(&header[8], "WAVE", 4) )
      result = getWavInfo( fileName );
    else if ( !strncmp(header, ".snd", 4) )
      result = getSndInfo( fileName );
    else if ( !strncmp(header, "FORM", 4) &&
              (!strncmp(&header[8], "AIFF", 4) || !strncmp(&header[8], "AIFC", 4) ) )
      result = getAifInfo( fileName );
    else {
      if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
      if ( fread(&header, 2, 1, fd) != 1 ) goto error;
      if (!strncmp(header, "MI", 2) ||
          !strncmp(header, "IM", 2) )
        result = getMatInfo( fileName );
      else {
        sprintf(msg, "WvIn: File (%s) format unknown.", fileName);
        handleError(msg, StkError::FILE_UNKNOWN_FORMAT);
      }
    }
  }

  if ( result == false )
    handleError(msg, StkError::FILE_ERROR);

  if ( fileSize == 0 ) {
    sprintf(msg, "WvIn: File (%s) data size is zero!", fileName);
    handleError(msg, StkError::FILE_ERROR);
  }

  // Allocate new memory if necessary.
  samples = (bufferSize+1)*channels;
  if ( lastSamples < samples ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[samples];
  }
  if ( lastChannels < channels ) {
    if ( lastOutput ) delete [] lastOutput;
    lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  }

  if ( fmod(rate, 1.0) != 0.0 ) interpolate = true;
  chunkPointer = 0;
  reset();
  readData( 0 );  // Load file data.
  if ( doNormalize ) normalize();
  finished = false;
  return;

 error:
  sprintf(msg, "WvIn: Error reading file (%s).", fileName);
  handleError(msg, StkError::FILE_ERROR);
}

bool WvIn :: getRawInfo( const char *fileName )
{
  // Use the system call "stat" to determine the file length.
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    sprintf(msg, "WvIn: Could not stat RAW file (%s).", fileName);
    return false;
  }

  fileSize = (long) filestat.st_size / 2;  // length in 2-byte samples
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
    gain = 1.0 / 32768.0;
  }

  // STK rawwave files have no header and are assumed to contain a
  // monophonic stream of 16-bit signed integers in big-endian byte
  // order with a sample rate of 22050 Hz.
  channels = 1;
  dataOffset = 0;
  rate = (MY_FLOAT) 22050.0 / Stk::sampleRate();
  fileRate = 22050.0;
  interpolate = false;
  dataType = STK_SINT16;
  byteswap = false;
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
#endif

  return true;
}

bool WvIn :: getWavInfo( const char *fileName )
{
  // Find "format" chunk ... it must come before the "data" chunk.
  char id[4];
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "fmt ", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Check that the data is not compressed.
  SINT16 format_tag;
  if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error; // Read fmt chunk size.
  if ( fread(&format_tag, 2, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&format_tag);
  swap32((unsigned char *)&chunkSize);
#endif
  if (format_tag != 1 && format_tag != 3 ) { // PCM = 1, FLOAT = 3
    sprintf(msg, "WvIn: %s contains an unsupported data format type (%d).", fileName, format_tag);
    return false;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels = (unsigned int ) temp;

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / Stk::sampleRate() );

  // Determine the data type.
  dataType = 0;
  if ( fseek(fd, 6, SEEK_CUR) == -1 ) goto error;   // Locate bits_per_sample info.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  if ( format_tag == 1 ) {
    if (temp == 8)
      dataType = STK_SINT8;
    else if (temp == 16)
      dataType = STK_SINT16;
    else if (temp == 32)
      dataType = STK_SINT32;
  }
  else if ( format_tag == 3 ) {
    if (temp == 32)
      dataType = MY_FLOAT32;
    else if (temp == 64)
      dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "WvIn: %d bits per sample with data format %d are not supported (%s).", temp, format_tag, fileName);
    return false;
  }

  // Jump over any remaining part of the "fmt" chunk.
  if ( fseek(fd, chunkSize-16, SEEK_CUR) == -1 ) goto error;

  // Find "data" chunk ... it must come after the "fmt" chunk.
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;

  while ( strncmp(id, "data", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get length of data from the header.
  SINT32 bytes;
  if ( fread(&bytes, 4, 1, fd) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fileSize = 8 * bytes / temp / channels;  // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  dataOffset = ftell(fd);
  byteswap = false;
#ifndef __LITTLE_ENDIAN__
  byteswap = true;
#endif

  return true;

 error:
  sprintf(msg, "WvIn: Error reading WAV file (%s).", fileName);
  return false;
}

bool WvIn :: getSndInfo( const char *fileName )
{
  // Determine the data type.
  SINT32 format;
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;   // Locate format
  if ( fread(&format, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&format);
#endif
  if (format == 2) dataType = STK_SINT8;
  else if (format == 3) dataType = STK_SINT16;
  else if (format == 5) dataType = STK_SINT32;
  else if (format == 6) dataType = MY_FLOAT32;
  else if (format == 7) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "WvIn: data format in file %s is not supported.", fileName);
    return false;
  }

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  fileRate = (MY_FLOAT) srate;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( srate / sampleRate() );

  // Get number of channels from the header.
  SINT32 chans;
  if ( fread(&chans, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&chans);
#endif
  channels = chans;

  if ( fseek(fd, 4, SEEK_SET) == -1 ) goto error;
  if ( fread(&dataOffset, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&dataOffset);
#endif

  // Get length of data from the header.
  if ( fread(&fileSize, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&fileSize);
#endif
  fileSize /= 2 * channels;  // Convert to sample frames.
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  byteswap = false;
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
#endif

  return true;

 error:
  sprintf(msg, "WvIn: Error reading SND file (%s).", fileName);
  return false;
}

bool WvIn :: getAifInfo( const char *fileName )
{
  bool aifc = false;
  char id[4];

  // Determine whether this is AIFF or AIFC.
  if ( fseek(fd, 8, SEEK_SET) == -1 ) goto error;
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  if ( !strncmp(id, "AIFC", 4) ) aifc = true;

  // Find "common" chunk
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd) != 1) goto error;
  while ( strncmp(id, "COMM", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error; // Jump over chunk size
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels = temp;

  // Get length of data from the header.
  SINT32 frames;
  if ( fread(&frames, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  fileSize = frames; // sample frames
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Read the number of bits per sample.
  if ( fread(&temp, 2, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif

  // Get file sample rate from the header.  For AIFF files, this value
  // is stored in a 10-byte, IEEE Standard 754 floating point number,
  // so we need to convert it first.
  unsigned char srate[10];
  unsigned char exp;
  unsigned long mantissa;
  unsigned long last;
  if ( fread(&srate, 10, 1, fd) != 1 ) goto error;
  mantissa = (unsigned long) *(unsigned long *)(srate+2);
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&mantissa);
#endif
  exp = 30 - *(srate+1);
  last = 0;
  while (exp--) {
    last = mantissa;
    mantissa >>= 1;
  }
  if (last & 0x00000001) mantissa++;
  fileRate = (MY_FLOAT) mantissa;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  // Determine the data format.
  dataType = 0;
  if ( aifc == false ) {
    if ( temp == 8 ) dataType = STK_SINT8;
    else if ( temp == 16 ) dataType = STK_SINT16;
    else if ( temp == 32 ) dataType = STK_SINT32;
  }
  else {
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
    if ( (!strncmp(id, "fl32", 4) || !strncmp(id, "FL32", 4)) && temp == 32 ) dataType = MY_FLOAT32;
    else if ( (!strncmp(id, "fl64", 4) || !strncmp(id, "FL64", 4)) && temp == 64 ) dataType = MY_FLOAT64;
  }
  if ( dataType == 0 ) {
    sprintf(msg, "WvIn: %d bits per sample in file %s are not supported.", temp, fileName);
    return false;
  }

  // Start at top to find data (SSND) chunk ... chunk order is undefined.
  if ( fseek(fd, 12, SEEK_SET) == -1 ) goto error;

  // Find data (SSND) chunk
  if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  while ( strncmp(id, "SSND", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd) != 1 ) goto error;
  }

  // Skip over chunk size, offset, and blocksize fields
  if ( fseek(fd, 12, SEEK_CUR) == -1 ) goto error;

  dataOffset = ftell(fd);
  byteswap = false;
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
#endif

  return true;

 error:
  sprintf(msg, "WvIn: Error reading AIFF file (%s).", fileName);
  return false;
}

bool WvIn :: getMatInfo( const char *fileName )
{
  // Verify this is a version 5 MAT-file format.
  char head[4];
  if ( fseek(fd, 0, SEEK_SET) == -1 ) goto error;
  if ( fread(&head, 4, 1, fd) != 1 ) goto error;
  // If any of the first 4 characters of the header = 0, then this is
  // a Version 4 MAT-file.
  if ( strstr(head, "0") ) {
    sprintf(msg, "WvIn: %s appears to be a Version 4 MAT-file, which is not currently supported.",
            fileName);
    return false;
  }

  // Determine the endian-ness of the file.
  char mi[2];
  byteswap = false;
  // Locate "M" and "I" characters in header.
  if ( fseek(fd, 126, SEEK_SET) == -1 ) goto error;
  if ( fread(&mi, 2, 1, fd) != 1) goto error;
#ifdef __LITTLE_ENDIAN__
  if ( !strncmp(mi, "MI", 2) )
    byteswap = true;
  else if ( strncmp(mi, "IM", 2) ) goto error;
#else
  if ( !strncmp(mi, "IM", 2))
    byteswap = true;
  else if ( strncmp(mi, "MI", 2) ) goto error;
#endif

  // Check the data element type
  SINT32 datatype;
  if ( fread(&datatype, 4, 1, fd) != 1 ) goto error;
  if ( byteswap ) swap32((unsigned char *)&datatype);
  if (datatype != 14) {
    sprintf(msg, "WvIn: The file does not contain a single Matlab array (or matrix) data element.");
    return false;
  }

  // Determine the array data type.
  SINT32 tmp;
  SINT32 size;
  if ( fseek(fd, 168, SEEK_SET) == -1 ) goto error;
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if (tmp == 1) {  // array name > 4 characters
    if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;  // get array name length
    if (byteswap) swap32((unsigned char *)&tmp);
    size = (SINT32) ceil((float)tmp / 8);
    if ( fseek(fd, size*8, SEEK_CUR) == -1 ) goto error;  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    if ( fseek(fd, 4, SEEK_CUR) == -1 ) goto error;
  }
  if ( fread(&tmp, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&tmp);
  if ( tmp == 1 ) dataType = STK_SINT8;
  else if ( tmp == 3 ) dataType = STK_SINT16;
  else if ( tmp == 5 ) dataType = STK_SINT32;
  else if ( tmp == 7 ) dataType = MY_FLOAT32;
  else if ( tmp == 9 ) dataType = MY_FLOAT64;
  else {
    sprintf(msg, "WvIn: The MAT-file array data format (%d) is not supported.", tmp);
    return false;
  }

  // Get number of rows from the header.
  SINT32 rows;
  if ( fseek(fd, 160, SEEK_SET) == -1 ) goto error;
  if ( fread(&rows, 4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&rows);

  // Get number of columns from the header.
  SINT32 columns;
  if ( fread(&columns,4, 1, fd) != 1 ) goto error;
  if (byteswap) swap32((unsigned char *)&columns);

  // Assume channels = smaller of rows or columns.
  if (rows < columns) {
    channels = rows;
    fileSize = columns;
  }
  else {
    sprintf(msg, "WvIn: Transpose the MAT-file array so that audio channels fill matrix rows (not columns).");
    return false;
  }
  bufferSize = fileSize;
  if (fileSize > CHUNK_THRESHOLD) {
    chunking = true;
    bufferSize = CHUNK_SIZE;
  }

  // Move read pointer to the data in the file.
  SINT32 headsize;
  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fread(&headsize, 4, 1, fd) != 1 ) goto error; // file size from 132nd byte
  if (byteswap) swap32((unsigned char *)&headsize);
  headsize -= fileSize * 8 * channels;
  if ( fseek(fd, headsize, SEEK_CUR) == -1 ) goto error;
  dataOffset = ftell(fd);

  // Assume MAT-files have 44100 Hz sample rate.
  fileRate = 44100.0;

  // Set default rate based on file sampling rate.
  rate = (MY_FLOAT) ( fileRate / sampleRate() );

  return true;

 error:
  sprintf(msg, "WvIn: Error reading MAT-file (%s).", fileName);
  return false;
}

void WvIn :: readData( unsigned long index )
{
  while (index < (unsigned long)chunkPointer) {
    // Negative rate.
    chunkPointer -= CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if (chunkPointer < 0) {
      bufferSize += chunkPointer;
      chunkPointer = 0;
    }
  }
  while (index >= chunkPointer+bufferSize) {
    // Positive rate.
    chunkPointer += CHUNK_SIZE;
    bufferSize = CHUNK_SIZE;
    if ( (unsigned long)chunkPointer+CHUNK_SIZE >= fileSize) {
      bufferSize = fileSize - chunkPointer;
    }
  }

  long i, length = bufferSize;
  bool endfile = (chunkPointer+bufferSize == fileSize);
  if ( !endfile ) length += 1;

  // Read samples into data[].  Use MY_FLOAT data structure
  // to store samples.
  if ( dataType == STK_SINT16 ) {
    SINT16 *buf = (SINT16 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*2), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 2, fd) != 2 ) goto error;
    if ( byteswap ) {
      SINT16 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap16((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 *buf = (SINT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      SINT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 4, fd) != 4 ) goto error;
    if ( byteswap ) {
      FLOAT32 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels*8), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 8, fd) != 8 ) goto error;
    if ( byteswap ) {
      FLOAT64 *ptr = buf;
      for (i=length*channels-1; i>=0; i--)
        swap64((unsigned char *)(ptr++));
    }
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i];
  }
  else if ( dataType == STK_SINT8 ) {
    unsigned char *buf = (unsigned char *)data;
    if (fseek(fd, dataOffset+(long)(chunkPointer*channels), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels, 1, fd) != 1 ) goto error;
    for (i=length*channels-1; i>=0; i--)
      data[i] = buf[i] - 128.0;  // 8-bit WAV data is unsigned!
  }

  // If at end of file, repeat last sample frame for interpolation.
  if ( endfile ) {
    for (unsigned int j=0; j<channels; j++)
      data[bufferSize*channels+j] = data[(bufferSize-1)*channels+j];
  }

  if (!chunking) {
    fclose(fd);
    fd = 0;
  }

  return;

 error:
  sprintf(msg, "WvIn: Error reading file data.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvIn :: reset(void)
{
  time = (MY_FLOAT) 0.0;
  for (unsigned int i=0; i<channels; i++)
    lastOutput[i] = (MY_FLOAT) 0.0;
  finished = false;
}

void WvIn :: normalize(void)
{
  this->normalize((MY_FLOAT) 1.0);
}

// Normalize all channels equally by the greatest magnitude in all of the data.
void WvIn :: normalize(MY_FLOAT peak)
{
  if (chunking) {
    if ( dataType == STK_SINT8 ) gain = peak / 128.0;
    else if ( dataType == STK_SINT16 ) gain = peak / 32768.0;
    else if ( dataType == STK_SINT32 ) gain = peak / 2147483648.0;
    else if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) gain = peak;

    return;
  }

  unsigned long i;
  MY_FLOAT max = (MY_FLOAT) 0.0;

  for (i=0; i<channels*bufferSize; i++) {
    if (fabs(data[i]) > max)
      max = (MY_FLOAT) fabs((double) data[i]);
  }

  if (max > 0.0) {
    max = (MY_FLOAT) 1.0 / max;
    max *= peak;
    for (i=0;i<=channels*bufferSize;i++)
	    data[i] *= max;
  }
}

unsigned long WvIn :: getSize(void) const
{
  return fileSize;
}

unsigned int WvIn :: getChannels(void) const
{
  return channels;
}

MY_FLOAT WvIn :: getFileRate(void) const
{
  return fileRate;
}

bool WvIn :: isFinished(void) const
{
  return finished;
}

void WvIn :: setRate(MY_FLOAT aRate)
{
  rate = aRate;

  // If negative rate and at beginning of sound, move pointer to end
  // of sound.
  if ( (rate < 0) && (time == 0.0) ) time += rate + fileSize;

  if (fmod(rate, 1.0) != 0.0) interpolate = true;
  else interpolate = false;
}

void WvIn :: addTime(MY_FLOAT aTime)   
{
  // Add an absolute time in samples 
  time += aTime;

  if (time < 0.0) time = 0.0;
  if (time >= fileSize) {
    time = fileSize;
    finished = true;
  }
}

void WvIn :: setInterpolate(bool doInterpolate)
{
  interpolate = doInterpolate;
}

const MY_FLOAT *WvIn :: lastFrame(void) const
{
  return lastOutput;
}

MY_FLOAT WvIn :: lastOut(void) const
{
  if ( channels == 1 )
    return *lastOutput;

  MY_FLOAT output = 0.0;
  for (unsigned int i=0; i<channels; i++ ) {
    output += lastOutput[i];
  }
  return output / channels;
}

MY_FLOAT WvIn :: tick(void)
{
  tickFrame();
  return lastOut();
}

MY_FLOAT *WvIn :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick();

  return vector;
}

const MY_FLOAT *WvIn :: tickFrame(void)
{
  register MY_FLOAT tyme, alpha;
  register unsigned long i, index;

  if (finished) return lastOutput;

  tyme = time;
  if (chunking) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer) || (tyme >= chunkPointer+bufferSize) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer;
  }

  // Integer part of time address.
  index = (long) tyme;

  if (interpolate) {
    // Linear interpolation ... fractional part of time address.
    alpha = tyme - (MY_FLOAT) index;
    index *= channels;
    for (i=0; i<channels; i++) {
      lastOutput[i] = data[index];
      lastOutput[i] += (alpha * (data[index+channels] - lastOutput[i]));
      index++;
    }
  }
  else {
    index *= channels;
    for (i=0; i<channels; i++)
      lastOutput[i] = data[index++];
  }

  if (chunking) {
    // Scale outputs by gain.
    for (i=0; i<channels; i++)  lastOutput[i] *= gain;
  }

  // Increment time, which can be negative.
  time += rate;
  if ( time < 0.0 || time >= fileSize ) finished = true;

  return lastOutput;
}

MY_FLOAT *WvIn :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    tickFrame();
    for ( j=0; j<channels; j++ )
      frameVector[i*channels+j] = lastOutput[j];
  }

  return frameVector;
}
