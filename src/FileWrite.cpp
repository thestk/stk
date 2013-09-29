/***************************************************/
/*! \class FileWrite
    \brief STK audio file output class.

    This class provides output support for various
    audio file formats.

    FileWrite writes samples to an audio file.  It supports
    multi-channel data.

    FileWrite currently supports uncompressed WAV, AIFF, AIFC, SND
    (AU), MAT-file (Matlab), and STK RAW file formats.  Signed integer
    (8-, 16-, and 32-bit) and floating- point (32- and 64-bit) data
    types are supported.  STK RAW files use 16-bit integers by
    definition.  MAT-files will always be written as 64-bit floats.
    If a data type specification does not match the specified file
    type, the data type will automatically be modified.  Compressed
    data types are not supported.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2009.
*/
/***************************************************/

#include "FileWrite.h"
#include <string.h>
#include <cmath>

namespace stk {

const FileWrite::FILE_TYPE FileWrite :: FILE_RAW = 1;
const FileWrite::FILE_TYPE FileWrite :: FILE_WAV = 2;
const FileWrite::FILE_TYPE FileWrite :: FILE_SND = 3;
const FileWrite::FILE_TYPE FileWrite :: FILE_AIF = 4;
const FileWrite::FILE_TYPE FileWrite :: FILE_MAT = 5;

// WAV header structure. See ftp://ftp.isi.edu/in-notes/rfc2361.txt
// for information regarding format codes.
struct wavhdr {
  char riff[4];           // "RIFF"
  SINT32 file_size;       // in bytes
  char wave[4];           // "WAVE"
  char fmt[4];            // "fmt "
  SINT32 chunk_size;      // in bytes (16 for PCM)
  SINT16 format_tag;      // 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
  SINT16 num_chans;       // 1=mono, 2=stereo
  SINT32 sample_rate;
  SINT32 bytes_per_sec;
  SINT16 bytes_per_samp;  // 2=16-bit mono, 4=16-bit stereo
  SINT16 bits_per_samp;
  char data[4];           // "data"
  SINT32 data_length;     // in bytes
};

// SND (AU) header structure (NeXT and Sun).
struct sndhdr {
  char pref[4];
  SINT32 hdr_length;
  SINT32 data_length;
  SINT32 format;
  SINT32 sample_rate;
  SINT32 num_channels;
  char comment[16];
};

// AIFF/AIFC header structure ... only the part common to both
// formats.
struct aifhdr {
  char form[4];                // "FORM"
  SINT32 form_size;            // in bytes
  char aiff[4];                // "AIFF" or "AIFC"
  char comm[4];                // "COMM"
  SINT32 comm_size;            // "COMM" chunk size (18 for AIFF, 24 for AIFC)
  SINT16 num_chans;            // number of channels
  unsigned long sample_frames; // sample frames of audio data
  SINT16 sample_size;          // in bits
  unsigned char srate[10];     // IEEE 754 floating point format
};

struct aifssnd {
  char ssnd[4];               // "SSND"
  SINT32 ssnd_size;           // "SSND" chunk size
  unsigned long offset;       // data offset in data block (should be 0)
  unsigned long block_size;   // not used by STK (should be 0)
};

// MAT-file 5 header structure.
struct mathdr {
  char heading[124];   // Header text field
  SINT16 hff[2];       // Header flag fields
  SINT32 adf[11];      // Array data format fields
  // There's more, but it's of variable length
};

FileWrite :: FileWrite()
  : fd_( 0 )
{
}

FileWrite::FileWrite( std::string fileName, unsigned int nChannels, FILE_TYPE type, Stk::StkFormat format )
  : fd_( 0 )
{
  this->open( fileName, nChannels, type, format );
}

FileWrite :: ~FileWrite()
{
  this->close();
}

void FileWrite :: close( void )
{
  if ( fd_ == 0 ) return;

  if ( fileType_ == FILE_RAW )
    fclose( fd_ );
  else if ( fileType_ == FILE_WAV )
    this->closeWavFile();
  else if ( fileType_ == FILE_SND )
    this->closeSndFile();
  else if ( fileType_ == FILE_AIF )
    this->closeAifFile();
  else if ( fileType_ == FILE_MAT )
    this->closeMatFile();

  fd_ = 0;
}

bool FileWrite :: isOpen( void )
{
  if ( fd_ ) return true;
  else return false;
}

void FileWrite :: open( std::string fileName, unsigned int nChannels, FileWrite::FILE_TYPE type, Stk::StkFormat format )
{
  // Call close() in case another file is already open.
  this->close();

  if ( nChannels < 1 ) {
    errorString_ << "FileWrite::open: then channels argument must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  channels_ = nChannels;
  fileType_ = type;

  if ( format != STK_SINT8 && format != STK_SINT16 &&
       format != STK_SINT32 && format != STK_FLOAT32 && 
       format != STK_FLOAT64 ) {
    errorString_ << "FileWrite::open: unknown data type (" << format << ") specified!";
    handleError( StkError::FUNCTION_ARGUMENT );
  } 
  dataType_ = format;

  bool result = false;
  if ( fileType_ == FILE_RAW ) {
    if ( channels_ != 1 ) {
      errorString_ << "FileWrite::open: STK RAW files are, by definition, always monaural (channels = " << nChannels << " not supported)!";
      handleError( StkError::FUNCTION_ARGUMENT );
    }
    result = setRawFile( fileName.c_str() );
  }
  else if ( fileType_ == FILE_WAV )
    result = setWavFile( fileName.c_str() );
  else if ( fileType_ == FILE_SND )
    result = setSndFile( fileName.c_str() );
  else if ( fileType_ == FILE_AIF )
    result = setAifFile( fileName.c_str() );
  else if ( fileType_ == FILE_MAT )
    result = setMatFile( fileName.c_str() );
  else {
    errorString_ << "FileWrite::open: unknown file type (" << fileType_ << ") specified!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( result == false )
    handleError( StkError::FILE_ERROR );

  frameCounter_ = 0;
}

bool FileWrite :: setRawFile( const char *fileName )
{
  char name[8192];
  strncpy(name, fileName, 8192);
  if ( strstr(name, ".raw") == NULL) strcat(name, ".raw");
  fd_ = fopen(name, "wb");
  if ( !fd_ ) {
    errorString_ << "FileWrite: could not create RAW file: " << name << '.';
    return false;
  }

  if ( dataType_ != STK_SINT16 ) {
    dataType_ = STK_SINT16;
    errorString_ << "FileWrite: using 16-bit signed integer data format for file " << name << '.';
    handleError( StkError::DEBUG_WARNING );
  }

  byteswap_ = false;
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
#endif

  errorString_ << "FileWrite: creating RAW file: " << name;
  handleError( StkError::STATUS );
  return true;
}

bool FileWrite :: setWavFile( const char *fileName )
{
  char name[8192];
  strncpy(name, fileName, 8192);
  if ( strstr(name, ".wav") == NULL) strcat(name, ".wav");
  fd_ = fopen(name, "wb");
  if ( !fd_ ) {
    errorString_ << "FileWrite: could not create WAV file: " << name;
    return false;
  }

  struct wavhdr hdr = {"RIF", 44, "WAV", "fmt", 16, 1, 1,
                        (SINT32) Stk::sampleRate(), 0, 2, 16, "dat", 0};
  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
  hdr.num_chans = (SINT16) channels_;
  if ( dataType_ == STK_SINT8 )
    hdr.bits_per_samp = 8;
  else if ( dataType_ == STK_SINT16 )
    hdr.bits_per_samp = 16;
  else if ( dataType_ == STK_SINT32 )
    hdr.bits_per_samp = 32;
  else if ( dataType_ == STK_FLOAT32 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 32;
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 64;
  }
  hdr.bytes_per_samp = (SINT16) (channels_ * hdr.bits_per_samp / 8);
  hdr.bytes_per_sec = (SINT32) (hdr.sample_rate * hdr.bytes_per_samp);

  byteswap_ = false;
#ifndef __LITTLE_ENDIAN__
  byteswap_ = true;
  swap32((unsigned char *)&hdr.file_size);
  swap32((unsigned char *)&hdr.chunk_size);
  swap16((unsigned char *)&hdr.format_tag);
  swap16((unsigned char *)&hdr.num_chans);
  swap32((unsigned char *)&hdr.sample_rate);
  swap32((unsigned char *)&hdr.bytes_per_sec);
  swap16((unsigned char *)&hdr.bytes_per_samp);
  swap16((unsigned char *)&hdr.bits_per_samp);
#endif

  if ( fwrite(&hdr, 4, 11, fd_) != 11 ) {
    errorString_ << "FileWrite: could not write WAV header for file " << name << '.';
    return false;
  }

  errorString_ << "FileWrite: creating WAV file: " << name;
  handleError( StkError::STATUS );
  return true;
}

void FileWrite :: closeWavFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType_ == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType_ == STK_SINT32 || dataType_ == STK_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType_ == STK_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = frameCounter_ * channels_ * bytes_per_sample;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd_, 40, SEEK_SET); // jump to data length
  fwrite(&bytes, 4, 1, fd_);

  bytes = frameCounter_ * channels_ * bytes_per_sample + 44;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd_, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd_);
  fclose( fd_ );
}

bool FileWrite :: setSndFile( const char *fileName )
{
  char name[8192];
  strncpy(name, fileName, 8192);
  if ( strstr(name, ".snd") == NULL) strcat(name, ".snd");
  fd_ = fopen(name, "wb");
  if ( !fd_ ) {
    errorString_ << "FileWrite: could not create SND file: " << name;
    return false;
  }

  struct sndhdr hdr = {".sn", 40, 0, 3, (SINT32) Stk::sampleRate(), 1, "Created by STK"};
  hdr.pref[3] = 'd';
  hdr.num_channels = channels_;
  if ( dataType_ == STK_SINT8 )
    hdr.format = 2;
  else if ( dataType_ == STK_SINT16 )
    hdr.format = 3;
  else if ( dataType_ == STK_SINT32 )
    hdr.format = 5;
  else if ( dataType_ == STK_FLOAT32 )
    hdr.format = 6;
  else if ( dataType_ == STK_FLOAT64 )
    hdr.format = 7;

  byteswap_ = false;
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
  swap32 ((unsigned char *)&hdr.hdr_length);
  swap32 ((unsigned char *)&hdr.format);
  swap32 ((unsigned char *)&hdr.sample_rate);
  swap32 ((unsigned char *)&hdr.num_channels);
#endif

  if ( fwrite(&hdr, 4, 10, fd_) != 10 ) {
    errorString_ << "FileWrite: Could not write SND header for file " << name << '.';
    return false;
  }

  errorString_ << "FileWrite: creating SND file: " << name;
  handleError( StkError::STATUS );
  return true;
}

void FileWrite :: closeSndFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType_ == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType_ == STK_SINT32 )
    bytes_per_sample = 4;
  else if ( dataType_ == STK_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType_ == STK_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = frameCounter_ * bytes_per_sample * channels_;
#ifdef __LITTLE_ENDIAN__
  swap32 ((unsigned char *)&bytes);
#endif
  fseek(fd_, 8, SEEK_SET); // jump to data size
  fwrite(&bytes, 4, 1, fd_);
  fclose(fd_);
}

bool FileWrite :: setAifFile( const char *fileName )
{
  char name[8192];
  strncpy(name, fileName, 8192);
  if ( strstr(name, ".aif") == NULL) strcat(name, ".aif");
  fd_ = fopen(name, "wb");
  if ( !fd_ ) {
    errorString_ << "FileWrite: could not create AIF file: " << name;
    return false;
  }

  // Common parts of AIFF/AIFC header.
  struct aifhdr hdr = {"FOR", 46, "AIF", "COM", 18, 0, 0, 16, "0"};
  struct aifssnd ssnd = {"SSN", 8, 0, 0};
  hdr.form[3] = 'M';
  hdr.aiff[3] = 'F';
  hdr.comm[3] = 'M';
  ssnd.ssnd[3] = 'D';
  hdr.num_chans = channels_;
  if ( dataType_ == STK_SINT8 )
    hdr.sample_size = 8;
  else if ( dataType_ == STK_SINT16 )
    hdr.sample_size = 16;
  else if ( dataType_ == STK_SINT32 )
    hdr.sample_size = 32;
  else if ( dataType_ == STK_FLOAT32 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 32;
    hdr.comm_size = 24;
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 64;
    hdr.comm_size = 24;
  }

  // For AIFF files, the sample rate is stored in a 10-byte,
  // IEEE Standard 754 floating point number, so we need to
  // convert to that.
  SINT16 i;
  unsigned long exp;
  unsigned long rate = (unsigned long) Stk::sampleRate();
  memset(hdr.srate, 0, 10);
  exp = rate;
  for (i=0; i<32; i++) {
    exp >>= 1;
    if (!exp) break;
  }
  i += 16383;
#ifdef __LITTLE_ENDIAN__
  swap16((unsigned char *)&i);
#endif
  *(SINT16 *)(hdr.srate) = (SINT16) i;

  for (i=32; i; i--) {
    if (rate & 0x80000000) break;
    rate <<= 1;
  }

#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&rate);
#endif
  *(unsigned long *)(hdr.srate+2) = (unsigned long) rate;

  byteswap_ = false;  
#ifdef __LITTLE_ENDIAN__
  byteswap_ = true;
  swap32((unsigned char *)&hdr.form_size);
  swap32((unsigned char *)&hdr.comm_size);
  swap16((unsigned char *)&hdr.num_chans);
  swap16((unsigned char *)&hdr.sample_size);
  swap32((unsigned char *)&ssnd.ssnd_size);
  swap32((unsigned char *)&ssnd.offset);
  swap32((unsigned char *)&ssnd.block_size);
#endif

  // The structure boundaries don't allow a single write of 54 bytes.
  if ( fwrite(&hdr, 4, 5, fd_) != 5 ) goto error;
  if ( fwrite(&hdr.num_chans, 2, 1, fd_) != 1 ) goto error;
  if ( fwrite(&hdr.sample_frames, 4, 1, fd_) != 1 ) goto error;
  if ( fwrite(&hdr.sample_size, 2, 1, fd_) != 1 ) goto error;
  if ( fwrite(&hdr.srate, 10, 1, fd_) != 1 ) goto error;

  if ( dataType_ == STK_FLOAT32 ) {
    char type[4] = {'f','l','3','2'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd_) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd_) != 1 ) goto error;
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    char type[4] = {'f','l','6','4'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd_) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd_) != 1 ) goto error;
  }
  
  if ( fwrite(&ssnd, 4, 4, fd_) != 4 ) goto error;

  errorString_ << "FileWrite: creating AIF file: " << name;
  handleError( StkError::STATUS );
  return true;

 error:
  errorString_ << "FileWrite: could not write AIF header for file: " << name;
  return false;
}

void FileWrite :: closeAifFile( void )
{
  unsigned long frames = (unsigned long) frameCounter_;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  fseek(fd_, 22, SEEK_SET); // jump to "COMM" sample_frames
  fwrite(&frames, 4, 1, fd_);

  int bytes_per_sample = 1;
  if ( dataType_ == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType_ == STK_SINT32 || dataType_ == STK_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType_ == STK_FLOAT64 )
    bytes_per_sample = 8;

  unsigned long bytes = frameCounter_ * bytes_per_sample * channels_ + 46;
  if ( dataType_ == STK_FLOAT32 || dataType_ == STK_FLOAT64 ) bytes += 6;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd_, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd_);

  bytes = frameCounter_ * bytes_per_sample * channels_ + 8;
  if ( dataType_ == STK_FLOAT32 || dataType_ == STK_FLOAT64 ) bytes += 6;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  if ( dataType_ == STK_FLOAT32 || dataType_ == STK_FLOAT64 )
    fseek(fd_, 48, SEEK_SET); // jump to "SSND" chunk size
  else
    fseek(fd_, 42, SEEK_SET); // jump to "SSND" chunk size
  fwrite(&bytes, 4, 1, fd_);

  fclose( fd_ );
}

bool FileWrite :: setMatFile( const char *fileName )
{
  char name[8192];
  strncpy(name, fileName, 8192);
  if ( strstr(name, ".mat") == NULL) strcat(name, ".mat");
  fd_ = fopen(name, "w+b");
  if ( !fd_ ) {
    errorString_ << "FileWrite: could not create MAT file: " << name;
    return false;
  }

  if ( dataType_ != STK_FLOAT64 ) {
    dataType_ = STK_FLOAT64;
    errorString_ << "FileWrite: using 64-bit floating-point data format for file " << name << '.';
    handleError( StkError::DEBUG_WARNING );
  }

  struct mathdr hdr;
  strcpy(hdr.heading,"MATLAB 5.0 MAT-file, Generated using the Synthesis ToolKit in C++ (STK). By Perry R. Cook and Gary P. Scavone.");

  int i;
  for (i=strlen(hdr.heading);i<124;i++) hdr.heading[i] = ' ';

  // Header Flag Fields
  hdr.hff[0] = (SINT16) 0x0100;   // Version field
  hdr.hff[1] = (SINT16) 'M';      // Endian indicator field ("MI")
  hdr.hff[1] <<= 8;
  hdr.hff[1] += 'I';

  hdr.adf[0] = (SINT32) 14;       // Matlab array data type value
  hdr.adf[1] = (SINT32) 0;        // Size of file after this point to end (in bytes)
                                 // Don't know size yet.

  // Numeric Array Subelements (4):
  // 1. Array Flags
  hdr.adf[2] = (SINT32) 6;        // Matlab 32-bit unsigned integer data type value
  hdr.adf[3] = (SINT32) 8;        // 8 bytes of data to follow
  hdr.adf[4] = (SINT32) 6;        // Double-precision array, no array flags set
  hdr.adf[5] = (SINT32) 0;        // 4 bytes undefined
  // 2. Array Dimensions
  hdr.adf[6] = (SINT32) 5;        // Matlab 32-bit signed integer data type value
  hdr.adf[7] = (SINT32) 8;        // 8 bytes of data to follow (2D array)
  hdr.adf[8] = (SINT32) channels_; // This is the number of rows
  hdr.adf[9] = (SINT32) 0;        // This is the number of columns

  // 3. Array Name
  // We'll use fileName for the matlab array name (as well as the file name).
  // If fileName is 4 characters or less, we have to use a compressed data element
  // format for the array name data element.  Otherwise, the array name must
  // be formatted in 8-byte increments (up to 31 characters + NULL).
  SINT32 namelength = (SINT32) strlen(fileName);
  if (strstr(fileName, ".mat")) namelength -= 4;
  if (namelength > 31) namelength = 31; // Truncate name to 31 characters.
  char arrayName[64];
  strncpy(arrayName, fileName, namelength);
  arrayName[namelength] = '\0';
  if (namelength > 4) {
    hdr.adf[10] = (SINT32) 1;        // Matlab 8-bit signed integer data type value
  }
  else { // Compressed data element format
    hdr.adf[10] = namelength;
    hdr.adf[10] <<= 16;
    hdr.adf[10] += 1;
  }
  SINT32 headsize = 40;        // Number of bytes in data element so far.

  // Write the fixed portion of the header
  if ( fwrite(&hdr, 172, 1, fd_) != 1 ) goto error;

  // Write MATLAB array name
  SINT32 tmp;
  if (namelength > 4) {
    if ( fwrite(&namelength, 4, 1, fd_) != 1) goto error;
    if ( fwrite(arrayName, namelength, 1, fd_) != 1 ) goto error;
    tmp = (SINT32) ceil((float)namelength / 8);
    if ( fseek(fd_, tmp*8-namelength, SEEK_CUR) == -1 ) goto error;
    headsize += tmp * 8;
  }
  else { // Compressed data element format
    if ( fwrite(arrayName, namelength, 1, fd_) != 1 ) goto error;
    tmp = 4 - namelength;
    if ( fseek(fd_, tmp, SEEK_CUR) == -1 ) goto error;
  }

  // Finish writing known header information
  tmp = 9;        // Matlab IEEE 754 double data type
  if ( fwrite(&tmp, 4, 1, fd_) != 1 ) goto error;
  tmp = 0;        // Size of real part subelement in bytes (8 per sample)
  if ( fwrite(&tmp, 4, 1, fd_) != 1 ) goto error;
  headsize += 8;  // Total number of bytes in data element so far

  if ( fseek(fd_, 132, SEEK_SET) == -1 ) goto error;
  if ( fwrite(&headsize, 4, 1, fd_) != 1 ) goto error; // Write header size ... will update at end
  if ( fseek(fd_, 0, SEEK_END) == -1 ) goto error;

  byteswap_ = false;
  errorString_ << "FileWrite: creating MAT-file (" << name << ") containing MATLAB array: " << arrayName;
  handleError( StkError::STATUS );

  return true;

 error:
  errorString_ << "FileWrite: could not write MAT-file header for file " << name << '.';
  return false;
}

void FileWrite :: closeMatFile( void )
{
  fseek(fd_, 164, SEEK_SET); // jump to number of columns
  fwrite(&frameCounter_, 4, 1, fd_);

  SINT32 headsize, temp;
  fseek(fd_, 132, SEEK_SET);  // jump to header size
  fread(&headsize, 4, 1, fd_);
  temp = headsize;
  headsize += (SINT32) (frameCounter_ * 8 * channels_);
  fseek(fd_, 132, SEEK_SET);
  // Write file size (minus some header info)
  fwrite(&headsize, 4, 1, fd_);

  fseek(fd_, temp+132, SEEK_SET); // jumpt to data size (in bytes)
  temp = frameCounter_ * 8 * channels_;
  fwrite(&temp, 4, 1, fd_);

  fclose(fd_);
}

void FileWrite :: write( StkFrames& buffer )
{
  if ( fd_ == 0 ) {
    errorString_ << "FileWrite::write(): a file has not yet been opened!";
    handleError( StkError::WARNING );
    return;
  }

  if ( buffer.channels() != channels_ ) {
    errorString_ << "FileWrite::write(): number of channels in the StkFrames argument does not match that specified to open() function!";
    handleError( StkError::FUNCTION_ARGUMENT );
    return;
  }

  unsigned long nSamples = buffer.size();
  if ( dataType_ == STK_SINT16 ) {
    SINT16 sample;
    for ( unsigned long k=0; k<nSamples; k++ ) {
      sample = (SINT16) (buffer[k] * 32767.0);
      //sample = ((SINT16) (( buffer[k] + 1.0 ) * 32767.5 + 0.5)) - 32768;
      if ( byteswap_ ) swap16( (unsigned char *)&sample );
      if ( fwrite(&sample, 2, 1, fd_) != 1 ) goto error;
    }
  }
  else if ( dataType_ == STK_SINT8 ) {
    if ( fileType_ == FILE_WAV ) { // 8-bit WAV data is unsigned!
      unsigned char sample;
      for ( unsigned long k=0; k<nSamples; k++ ) {
        sample = (unsigned char) (buffer[k] * 127.0 + 128.0);
        if ( fwrite(&sample, 1, 1, fd_) != 1 ) goto error;
      }
    }
    else {
      signed char sample;
      for ( unsigned long k=0; k<nSamples; k++ ) {
        sample = (signed char) (buffer[k] * 127.0);
        //sample = ((signed char) (( buffer[k] + 1.0 ) * 127.5 + 0.5)) - 128;
        if ( fwrite(&sample, 1, 1, fd_) != 1 ) goto error;
      }
    }
  }
  else if ( dataType_ == STK_SINT32 ) {
    SINT32 sample;
    for ( unsigned long k=0; k<nSamples; k++ ) {
      sample = (SINT32) (buffer[k] * 2147483647.0);
      //sample = ((SINT32) (( buffer[k] + 1.0 ) * 2147483647.5 + 0.5)) - 2147483648;
      if ( byteswap_ ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd_) != 1 ) goto error;
    }
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 sample;
    for ( unsigned long k=0; k<nSamples; k++ ) {
      sample = (FLOAT32) (buffer[k]);
      if ( byteswap_ ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd_) != 1 ) goto error;
    }
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 sample;
    for ( unsigned long k=0; k<nSamples; k++ ) {
      sample = (FLOAT64) (buffer[k]);
      if ( byteswap_ ) swap64( (unsigned char *)&sample );
      if ( fwrite(&sample, 8, 1, fd_) != 1 ) goto error;
    }
  }

  frameCounter_ += buffer.frames();
  return;

 error:
  errorString_ << "FileWrite::write(): error writing data to file!";
  handleError( StkError::FILE_ERROR );
}

} // stk namespace
