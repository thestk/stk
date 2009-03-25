/***************************************************/
/*! \class WvIn
    \brief STK audio data input base class.

    This class provides input support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvIn loads the contents of an audio file for
    subsequent output.  Linear interpolation is
    used for fractional "read rates".

    WvIn supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample
    frames.  For single-channel data, these
    methods return equivalent values.

    Small files are completely read into local
    memory during instantiation.  Large files are
    read incrementally from disk.  The file size
    threshold and the increment size values are
    defined in WvIn.h.

    When the end of a file is reached, subsequent
    calls to the tick() functions return the data
    values at the end of the file.

    WvIn currently supports WAV, AIFF, SND (AU),
    MAT-file (Matlab), and STK RAW file formats.
    Signed integer (8-, 16-, and 32-bit) and floating-
    point (32- and 64-bit) data types are supported.
    Compressed data types are not supported.  If using
    MAT-files, data should be saved in an array with
    each data channel filling a matrix row.  The sample
    rate for MAT-files is assumed to be 44100 Hz.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "WvIn.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

WvIn :: WvIn()
{
  init();
}

WvIn :: WvIn( std::string fileName, bool raw, bool doNormalize )
{
  init();
  openFile( fileName, raw, doNormalize );
}

WvIn :: ~WvIn()
{
  if (fd_)
    fclose(fd_);

  if (data_)
    delete [] data_;

  if (lastOutputs_)
    delete [] lastOutputs_;
}

void WvIn :: init( void )
{
  fd_ = 0;
  data_ = 0;
  lastOutputs_ = 0;
  chunking_ = false;
  finished_ = true;
  interpolate_ = false;
  bufferSize_ = 0;
  channels_ = 0;
  time_ = 0.0;
}

void WvIn :: closeFile( void )
{
  if ( fd_ ) fclose( fd_ );
  finished_ = true;
}

void WvIn :: openFile( std::string fileName, bool raw, bool doNormalize )
{
  closeFile();

  // Try to open the file.
  fd_ = fopen(fileName.c_str(), "rb");
  if (!fd_) {
    errorString_ << "WvIn::openFile: could not open or find file (" << fileName << ")!";
    handleError( StkError::FILE_NOT_FOUND );
  }

  unsigned long lastChannels = channels_;
  unsigned long samples, lastSamples = (bufferSize_+1)*channels_;
  bool result = false;
  if ( raw )
    result = getRawInfo( fileName.c_str() );
  else {
    char header[12];
    if ( fread(&header, 4, 3, fd_) != 3 ) goto error;
    if ( !strncmp(header, "RIFF", 4) &&
         !strncmp(&header[8], "WAVE", 4) )
      result = getWavInfo( fileName.c_str() );
    else if ( !strncmp(header, ".snd", 4) )
      result = getSndInfo( fileName.c_str() );
    else if ( !strncmp(header, "FORM", 4) &&
              (!strncmp(&header[8], "AIFF", 4) || !strncmp(&header[8], "AIFC", 4) ) )
      result = getAifInfo( fileName.c_str() );
    else {
      if ( fseek(fd_, 126, SEEK_SET) == -1 ) goto error;
      if ( fread(&header, 2, 1, fd_) != 1 ) goto error;
      if (!strncmp(header, "MI", 2) ||
          !strncmp(header, "IM", 2) )
        result = getMatInfo( fileName.c_str() );
      else {
        errorString_ << "WvIn::openFile: file (" << fileName << ") format unknown.";
        handleError( StkError::FILE_UNKNOWN_FORMAT );
      }
    }
  }

  if ( result == false )
    handleError( StkError::FILE_ERROR );

  if ( fileSize_ == 0 ) {
    errorString_ << "WvIn::openFile: file (" << fileName << ") data size is zero!";
    handleError( StkError::FILE_ERROR );
  }

  // Allocate new memory if necessary.
  samples = (bufferSize_+1)*channels_;
  if ( lastSamples < samples ) {
    if ( data_ ) delete [] data_;
    data_ = (StkFloat *) new StkFloat[samples];
  }
  if ( lastChannels < channels_ ) {
    if ( lastOutputs_ ) delete [] lastOutputs_;
    lastOutputs_ = (StkFloat *) new StkFloat[channels_];
  }

  if ( fmod(rate_, 1.0) != 0.0 ) interpolate_ = true;
  chunkPointer_ = 0;
  reset();
  readData( 0 );  // Load file data.
  if ( doNormalize ) normalize();
  finished_ = false;
  return;

 error:
  errorString_ << "WvIn::openFile: error reading file (" << fileName << ")!";
  handleError( StkError::FILE_ERROR );
}

bool WvIn :: getRawInfo( const char *fileName )
{
  // Use the system call "stat" to determine the file length.
  struct stat filestat;
  if ( stat(fileName, &filestat) == -1 ) {
    errorString_ << "WvIn: Could not stat RAW file (" << fileName << ").";
    return false;
  }

  fileSize_ = (long) filestat.st_size / 2;  // length in 2-byte samples
  bufferSize_ = fileSize_;
  if (fileSize_ > CHUNK_THRESHOLD) {
    chunking_ = true;
    bufferSize_ = CHUNK_SIZE;
    gain_ = 1.0 / 32768.0;
  }

  // STK rawwave files have no header and are assumed to contain a
  // monophonic stream of 16-bit signed integers in big-endian byte
  // order with a sample rate of 22050 Hz.
  channels_ = 1;
  dataOffset_ = 0;
  rate_ = (StkFloat) 22050.0 / Stk::sampleRate();
  fileRate_ = 22050.0;
  interpolate_ = false;
  dataType_ = STK_SINT16;
  byteswap_ = false;
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
#endif

  return true;
}

bool WvIn :: getWavInfo( const char *fileName )
{
  // Find "format" chunk ... it must come before the "data" chunk.
  char id[4];
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  while ( strncmp(id, "fmt ", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd_, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  }

  // Check that the data is not compressed.
  SINT16 format_tag;
  if ( fread(&chunkSize, 4, 1, fd_) != 1 ) goto error; // Read fmt chunk size.
  if ( fread(&format_tag, 2, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&format_tag);
  swap32((unsigned char *)&chunkSize);
#endif
  if (format_tag != 1 && format_tag != 3 ) { // PCM = 1, FLOAT = 3
    errorString_ << "WvIn: "<< fileName << " contains an unsupported data format type (" << format_tag << ").";
    return false;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fread(&temp, 2, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels_ = (unsigned int ) temp;

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  fileRate_ = (StkFloat) srate;

  // Set default rate based on file sampling rate.
  rate_ = (StkFloat) ( srate / Stk::sampleRate() );

  // Determine the data type.
  dataType_ = 0;
  if ( fseek(fd_, 6, SEEK_CUR) == -1 ) goto error;   // Locate bits_per_sample info.
  if ( fread(&temp, 2, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  if ( format_tag == 1 ) {
    if (temp == 8)
      dataType_ = STK_SINT8;
    else if (temp == 16)
      dataType_ = STK_SINT16;
    else if (temp == 32)
      dataType_ = STK_SINT32;
  }
  else if ( format_tag == 3 ) {
    if (temp == 32)
      dataType_ = STK_FLOAT32;
    else if (temp == 64)
      dataType_ = STK_FLOAT64;
  }
  if ( dataType_ == 0 ) {
    errorString_ << "WvIn: " << temp << " bits per sample with data format " << format_tag << "  are not supported (" << fileName << ").";
    return false;
  }

  // Jump over any remaining part of the "fmt" chunk.
  if ( fseek(fd_, chunkSize-16, SEEK_CUR) == -1 ) goto error;

  // Find "data" chunk ... it must come after the "fmt" chunk.
  if ( fread(&id, 4, 1, fd_) != 1 ) goto error;

  while ( strncmp(id, "data", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd_, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  }

  // Get length of data from the header.
  SINT32 bytes;
  if ( fread(&bytes, 4, 1, fd_) != 1 ) goto error;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fileSize_ = 8 * bytes / temp / channels_;  // sample frames
  bufferSize_ = fileSize_;
  if (fileSize_ > CHUNK_THRESHOLD) {
    chunking_ = true;
    bufferSize_ = CHUNK_SIZE;
  }

  dataOffset_ = ftell(fd_);
  byteswap_ = false;
#ifndef __LITTLE_ENDIAN__
  byteswap_ = true;
#endif

  return true;

 error:
  errorString_ << "WvIn: error reading WAV file (" << fileName << ").";
  return false;
}

bool WvIn :: getSndInfo( const char *fileName )
{
  // Determine the data type.
  SINT32 format;
  if ( fseek(fd_, 12, SEEK_SET) == -1 ) goto error;   // Locate format
  if ( fread(&format, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&format);
#endif
  if (format == 2) dataType_ = STK_SINT8;
  else if (format == 3) dataType_ = STK_SINT16;
  else if (format == 5) dataType_ = STK_SINT32;
  else if (format == 6) dataType_ = STK_FLOAT32;
  else if (format == 7) dataType_ = STK_FLOAT64;
  else {
    errorString_ << "WvIn: data format in file " << fileName << " is not supported.";
    return false;
  }

  // Get file sample rate from the header.
  SINT32 srate;
  if ( fread(&srate, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&srate);
#endif
  fileRate_ = (StkFloat) srate;

  // Set default rate based on file sampling rate.
  rate_ = (StkFloat) ( srate / sampleRate() );

  // Get number of channels from the header.
  SINT32 chans;
  if ( fread(&chans, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&chans);
#endif
  channels_ = chans;

  if ( fseek(fd_, 4, SEEK_SET) == -1 ) goto error;
  if ( fread(&dataOffset_, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&dataOffset_);
#endif

  // Get length of data from the header.
  if ( fread(&fileSize_, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&fileSize_);
#endif
  fileSize_ /= 2 * channels_;  // Convert to sample frames.
  bufferSize_ = fileSize_;
  if (fileSize_ > CHUNK_THRESHOLD) {
    chunking_ = true;
    bufferSize_ = CHUNK_SIZE;
  }

  byteswap_ = false;
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
#endif

  return true;

 error:
  errorString_ << "WvIn: Error reading SND file (" << fileName << ").";
  return false;
}

bool WvIn :: getAifInfo( const char *fileName )
{
  bool aifc = false;
  char id[4];

  // Determine whether this is AIFF or AIFC.
  if ( fseek(fd_, 8, SEEK_SET) == -1 ) goto error;
  if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  if ( !strncmp(id, "AIFC", 4) ) aifc = true;

  // Find "common" chunk
  SINT32 chunkSize;
  if ( fread(&id, 4, 1, fd_) != 1) goto error;
  while ( strncmp(id, "COMM", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd_, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  }

  // Get number of channels from the header.
  SINT16 temp;
  if ( fseek(fd_, 4, SEEK_CUR) == -1 ) goto error; // Jump over chunk size
  if ( fread(&temp, 2, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&temp);
#endif
  channels_ = temp;

  // Get length of data from the header.
  SINT32 frames;
  if ( fread(&frames, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  fileSize_ = frames; // sample frames
  bufferSize_ = fileSize_;
  if (fileSize_ > CHUNK_THRESHOLD) {
    chunking_ = true;
    bufferSize_ = CHUNK_SIZE;
  }

  // Read the number of bits per sample.
  if ( fread(&temp, 2, 1, fd_) != 1 ) goto error;
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
  if ( fread(&srate, 10, 1, fd_) != 1 ) goto error;
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
  fileRate_ = (StkFloat) mantissa;

  // Set default rate based on file sampling rate.
  rate_ = (StkFloat) ( fileRate_ / sampleRate() );

  // Determine the data format.
  dataType_ = 0;
  if ( aifc == false ) {
    if ( temp == 8 ) dataType_ = STK_SINT8;
    else if ( temp == 16 ) dataType_ = STK_SINT16;
    else if ( temp == 32 ) dataType_ = STK_SINT32;
  }
  else {
    if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
    if ( (!strncmp(id, "fl32", 4) || !strncmp(id, "FL32", 4)) && temp == 32 ) dataType_ = STK_FLOAT32;
    else if ( (!strncmp(id, "fl64", 4) || !strncmp(id, "FL64", 4)) && temp == 64 ) dataType_ = STK_FLOAT64;
  }
  if ( dataType_ == 0 ) {
    errorString_ << "WvIn: " << temp << " bits per sample in file " << fileName << " are not supported.";
    return false;
  }

  // Start at top to find data (SSND) chunk ... chunk order is undefined.
  if ( fseek(fd_, 12, SEEK_SET) == -1 ) goto error;

  // Find data (SSND) chunk
  if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  while ( strncmp(id, "SSND", 4) ) {
    if ( fread(&chunkSize, 4, 1, fd_) != 1 ) goto error;
#ifdef __LITTLE_ENDIAN__
    swap32((unsigned char *)&chunkSize);
#endif
    if ( fseek(fd_, chunkSize, SEEK_CUR) == -1 ) goto error;
    if ( fread(&id, 4, 1, fd_) != 1 ) goto error;
  }

  // Skip over chunk size, offset, and blocksize fields
  if ( fseek(fd_, 12, SEEK_CUR) == -1 ) goto error;

  dataOffset_ = ftell(fd_);
  byteswap_ = false;
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
#endif

  return true;

 error:
  errorString_ << "WvIn: Error reading AIFF file (" << fileName << ").";
  return false;
}

bool WvIn :: getMatInfo( const char *fileName )
{
  // Verify this is a version 5 MAT-file format.
  char head[4];
  if ( fseek(fd_, 0, SEEK_SET) == -1 ) goto error;
  if ( fread(&head, 4, 1, fd_) != 1 ) goto error;
  // If any of the first 4 characters of the header = 0, then this is
  // a Version 4 MAT-file.
  if ( strstr(head, "0") ) {
    errorString_ << "WvIn: " << fileName << " appears to be a Version 4 MAT-file, which is not currently supported.";
    return false;
  }

  // Determine the endian-ness of the file.
  char mi[2];
  byteswap_ = false;
  // Locate "M" and "I" characters in header.
  if ( fseek(fd_, 126, SEEK_SET) == -1 ) goto error;
  if ( fread(&mi, 2, 1, fd_) != 1) goto error;
#ifdef __LITTLE_ENDIAN__
  if ( !strncmp(mi, "MI", 2) )
    byteswap_ = true;
  else if ( strncmp(mi, "IM", 2) ) goto error;
#else
  if ( !strncmp(mi, "IM", 2))
    byteswap_ = true;
  else if ( strncmp(mi, "MI", 2) ) goto error;
#endif

  // Check the data element type
  SINT32 datatype;
  if ( fread(&datatype, 4, 1, fd_) != 1 ) goto error;
  if ( byteswap_ ) swap32((unsigned char *)&datatype);
  if (datatype != 14) {
    errorString_ << "WvIn: The file does not contain a single Matlab array (or matrix) data element.";
    return false;
  }

  // Determine the array data type.
  SINT32 tmp;
  SINT32 size;
  if ( fseek(fd_, 168, SEEK_SET) == -1 ) goto error;
  if ( fread(&tmp, 4, 1, fd_) != 1 ) goto error;
  if (byteswap_) swap32((unsigned char *)&tmp);
  if (tmp == 1) {  // array name > 4 characters
    if ( fread(&tmp, 4, 1, fd_) != 1 ) goto error;  // get array name length
    if (byteswap_) swap32((unsigned char *)&tmp);
    size = (SINT32) ceil((float)tmp / 8);
    if ( fseek(fd_, size*8, SEEK_CUR) == -1 ) goto error;  // jump over array name
  }
  else { // array name <= 4 characters, compressed data element
    if ( fseek(fd_, 4, SEEK_CUR) == -1 ) goto error;
  }
  if ( fread(&tmp, 4, 1, fd_) != 1 ) goto error;
  if (byteswap_) swap32((unsigned char *)&tmp);
  if ( tmp == 1 ) dataType_ = STK_SINT8;
  else if ( tmp == 3 ) dataType_ = STK_SINT16;
  else if ( tmp == 5 ) dataType_ = STK_SINT32;
  else if ( tmp == 7 ) dataType_ = STK_FLOAT32;
  else if ( tmp == 9 ) dataType_ = STK_FLOAT64;
  else {
    errorString_ << "WvIn: The MAT-file array data format (" << tmp << ") is not supported.";
    return false;
  }

  // Get number of rows from the header.
  SINT32 rows;
  if ( fseek(fd_, 160, SEEK_SET) == -1 ) goto error;
  if ( fread(&rows, 4, 1, fd_) != 1 ) goto error;
  if (byteswap_) swap32((unsigned char *)&rows);

  // Get number of columns from the header.
  SINT32 columns;
  if ( fread(&columns,4, 1, fd_) != 1 ) goto error;
  if (byteswap_) swap32((unsigned char *)&columns);

  // Assume channels = smaller of rows or columns.
  if (rows < columns) {
    channels_ = rows;
    fileSize_ = columns;
  }
  else {
    errorString_ << "WvIn: Transpose the MAT-file array so that audio channels fill matrix rows (not columns).";
    return false;
  }
  bufferSize_ = fileSize_;
  if (fileSize_ > CHUNK_THRESHOLD) {
    chunking_ = true;
    bufferSize_ = CHUNK_SIZE;
  }

  // Move read pointer to the data in the file.
  SINT32 headsize;
  if ( fseek(fd_, 132, SEEK_SET) == -1 ) goto error;
  if ( fread(&headsize, 4, 1, fd_) != 1 ) goto error; // file size from 132nd byte
  if (byteswap_) swap32((unsigned char *)&headsize);
  headsize -= fileSize_ * 8 * channels_;
  if ( fseek(fd_, headsize, SEEK_CUR) == -1 ) goto error;
  dataOffset_ = ftell(fd_);

  // Assume MAT-files have 44100 Hz sample rate.
  fileRate_ = 44100.0;

  // Set default rate based on file sampling rate.
  rate_ = (StkFloat) ( fileRate_ / sampleRate() );

  return true;

 error:
  errorString_ << "WvIn: Error reading MAT-file (" << fileName << ").";
  return false;
}

void WvIn :: readData( unsigned long index )
{
  while (index < (unsigned long)chunkPointer_) {
    // Negative rate.
    chunkPointer_ -= CHUNK_SIZE;
    bufferSize_ = CHUNK_SIZE;
    if (chunkPointer_ < 0) {
      bufferSize_ += chunkPointer_;
      chunkPointer_ = 0;
    }
  }
  while (index >= chunkPointer_+bufferSize_) {
    // Positive rate.
    chunkPointer_ += CHUNK_SIZE;
    bufferSize_ = CHUNK_SIZE;
    if ( (unsigned long)chunkPointer_+CHUNK_SIZE >= fileSize_) {
      bufferSize_ = fileSize_ - chunkPointer_;
    }
  }

  long i, length = bufferSize_;
  bool endfile = (chunkPointer_+bufferSize_ == fileSize_);
  if ( !endfile ) length += 1;

  // Read samples into data[].  Use StkFloat data structure
  // to store samples.
  if ( dataType_ == STK_SINT16 ) {
    SINT16 *buf = (SINT16 *)data_;
    if (fseek(fd_, dataOffset_+(long)(chunkPointer_*channels_*2), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels_, 2, fd_) != 2 ) goto error;
    if ( byteswap_ ) {
      SINT16 *ptr = buf;
      for (i=length*channels_-1; i>=0; i--)
        swap16((unsigned char *)(ptr++));
    }
    for (i=length*channels_-1; i>=0; i--)
      data_[i] = buf[i];
  }
  else if ( dataType_ == STK_SINT32 ) {
    SINT32 *buf = (SINT32 *)data_;
    if (fseek(fd_, dataOffset_+(long)(chunkPointer_*channels_*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels_, 4, fd_) != 4 ) goto error;
    if ( byteswap_ ) {
      SINT32 *ptr = buf;
      for (i=length*channels_-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels_-1; i>=0; i--)
      data_[i] = buf[i];
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *)data_;
    if (fseek(fd_, dataOffset_+(long)(chunkPointer_*channels_*4), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels_, 4, fd_) != 4 ) goto error;
    if ( byteswap_ ) {
      FLOAT32 *ptr = buf;
      for (i=length*channels_-1; i>=0; i--)
        swap32((unsigned char *)(ptr++));
    }
    for (i=length*channels_-1; i>=0; i--)
      data_[i] = buf[i];
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *)data_;
    if (fseek(fd_, dataOffset_+(long)(chunkPointer_*channels_*8), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels_, 8, fd_) != 8 ) goto error;
    if ( byteswap_ ) {
      FLOAT64 *ptr = buf;
      for (i=length*channels_-1; i>=0; i--)
        swap64((unsigned char *)(ptr++));
    }
    for (i=length*channels_-1; i>=0; i--)
      data_[i] = buf[i];
  }
  else if ( dataType_ == STK_SINT8 ) {
    unsigned char *buf = (unsigned char *)data_;
    if (fseek(fd_, dataOffset_+(long)(chunkPointer_*channels_), SEEK_SET) == -1) goto error;
    if (fread(buf, length*channels_, 1, fd_) != 1 ) goto error;
    for (i=length*channels_-1; i>=0; i--)
      data_[i] = buf[i] - 128.0;  // 8-bit WAV data is unsigned!
  }

  // If at end of file, repeat last sample frame for interpolation.
  if ( endfile ) {
    for (unsigned int j=0; j<channels_; j++)
      data_[bufferSize_*channels_+j] = data_[(bufferSize_-1)*channels_+j];
  }

  if (!chunking_) {
    fclose(fd_);
    fd_ = 0;
  }

  return;

 error:
  errorString_ << "WvIn: Error reading file data.";
  handleError( StkError::FILE_ERROR);
}

void WvIn :: reset(void)
{
  time_ = (StkFloat) 0.0;
  for ( unsigned int i=0; i<channels_; i++ )
    lastOutputs_[i] = 0.0;
  finished_ = false;
}

void WvIn :: normalize(void)
{
  this->normalize( 1.0 );
}

// Normalize all channels equally by the greatest magnitude in all of the data.
void WvIn :: normalize( StkFloat peak )
{
  if (chunking_) {
    if ( dataType_ == STK_SINT8 ) gain_ = peak / 128.0;
    else if ( dataType_ == STK_SINT16 ) gain_ = peak / 32768.0;
    else if ( dataType_ == STK_SINT32 ) gain_ = peak / 2147483648.0;
    else if ( dataType_ == STK_FLOAT32 || dataType_ == STK_FLOAT64 ) gain_ = peak;

    return;
  }

  unsigned long i;
  StkFloat max = 0.0;

  for (i=0; i<channels_*bufferSize_; i++) {
    if (fabs(data_[i]) > max)
      max = (StkFloat) fabs((double) data_[i]);
  }

  if (max > 0.0) {
    max = 1.0 / max;
    max *= peak;
    for (i=0;i<=channels_*bufferSize_;i++)
	    data_[i] *= max;
  }
}

unsigned long WvIn :: getSize(void) const
{
  return fileSize_;
}

unsigned int WvIn :: getChannels(void) const
{
  return channels_;
}

StkFloat WvIn :: getFileRate(void) const
{
  return fileRate_;
}

void WvIn :: setRate(StkFloat aRate)
{
  rate_ = aRate;

  // If negative rate and at beginning of sound, move pointer to end
  // of sound.
  if ( (rate_ < 0) && (time_ == 0.0) ) time_ += rate_ + fileSize_;

  if (fmod(rate_, 1.0) != 0.0) interpolate_ = true;
  else interpolate_ = false;
}

void WvIn :: addTime(StkFloat aTime)   
{
  // Add an absolute time in samples 
  time_ += aTime;

  if (time_ < 0.0) time_ = 0.0;
  if (time_ >= fileSize_) {
    time_ = fileSize_;
    finished_ = true;
  }
}

void WvIn :: setInterpolate(bool doInterpolate)
{
  interpolate_ = doInterpolate;
}

bool WvIn :: isFinished(void) const
{
  return finished_;
}

const StkFloat *WvIn :: lastFrame(void) const
{
  return lastOutputs_;
}

StkFloat WvIn :: lastOut(void) const
{
  if ( channels_ == 1 )
    return *lastOutputs_;

  StkFloat output = 0.0;
  for ( unsigned int i=0; i<channels_; i++ ) {
    output += lastOutputs_[i];
  }
  return output / channels_;
}

StkFloat WvIn :: tick(void)
{
  this->tickFrame();
  return this->lastOut();
}

StkFloat *WvIn :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for ( unsigned int i=0; i<vectorSize; i++ )
    vector[i] = tick();

  return vector;
}

StkFrames& WvIn :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "WvIn::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick();
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick();
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick();
  }

  return frames;
}

const StkFloat *WvIn :: tickFrame(void)
{
  if (finished_) return lastOutputs_;

  register StkFloat tyme = time_;
  if (chunking_) {
    // Check the time address vs. our current buffer limits.
    if ( (tyme < chunkPointer_) || (tyme >= chunkPointer_+bufferSize_) )
      this->readData((long) tyme);
    // Adjust index for the current buffer.
    tyme -= chunkPointer_;
  }

  // Integer part of time address.
  register unsigned long index = (unsigned long) tyme;

  register unsigned long i;
  if (interpolate_) {
    // Linear interpolation ... fractional part of time address.
    register StkFloat alpha = tyme - (StkFloat) index;
    index *= channels_;
    for (i=0; i<channels_; i++) {
      lastOutputs_[i] = data_[index];
      lastOutputs_[i] += (alpha * (data_[index+channels_] - lastOutputs_[i]));
      index++;
    }
  }
  else {
    index *= channels_;
    for (i=0; i<channels_; i++)
      lastOutputs_[i] = data_[index++];
  }

  if (chunking_) {
    // Scale outputs by gain.
    for (i=0; i<channels_; i++)  lastOutputs_[i] *= gain_;
  }

  // Increment time, which can be negative.
  time_ += rate_;
  if ( time_ < 0.0 || time_ >= fileSize_ )
    finished_ = true;

  return lastOutputs_;
}

StkFloat *WvIn :: tickFrame(StkFloat *frameVector, unsigned int frames)
{
  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    this->tickFrame();
    for ( j=0; j<channels_; j++ )
      frameVector[i*channels_+j] = lastOutputs_[j];
  }

  return frameVector;
}

StkFrames& WvIn :: tickFrame( StkFrames& frames )
{
  if ( channels_ != frames.channels() ) {
    errorString_ << "WvIn::tickFrame(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned int j;
  if ( channels_ > 1 && frames.interleaved() == false ) {
    unsigned int jump = frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      this->tickFrame();
      for ( j=0; j<channels_; j++ )
        frames[i*channels_+j*jump] = lastOutputs_[j];
    }
  }
  else {
    unsigned int counter = 0;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      this->tickFrame();
      for ( j=0; j<channels_; j++ )
        frames[counter++] = lastOutputs_[j];
    }
  }

  return frames;
}
