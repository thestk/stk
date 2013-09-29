/***************************************************/
/*! \class WvOut
    \brief STK audio data output base class.

    This class provides output support for various
    audio file formats.  It also serves as a base
    class for "realtime" streaming subclasses.

    WvOut writes samples to an audio file.  It
    supports multi-channel data in interleaved
    format.  It is important to distinguish the
    tick() methods, which output single samples
    to all channels in a sample frame, from the
    tickFrame() method, which takes a pointer
    to multi-channel sample frame data.

    WvOut currently supports WAV, AIFF, AIFC, SND
    (AU), MAT-file (Matlab), and STK RAW file
    formats.  Signed integer (8-, 16-, and 32-bit)
    and floating- point (32- and 64-bit) data types
    are supported.  STK RAW files use 16-bit
    integers by definition.  MAT-files will always
    be written as 64-bit floats.  If a data type
    specification does not match the specified file
    type, the data type will automatically be
    modified.  Uncompressed data types are not
    supported.

    Currently, WvOut is non-interpolating and the
    output rate is always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "WvOut.h"
#include "string.h"
#include "math.h"

const WvOut::FILE_TYPE WvOut :: WVOUT_RAW = 1;
const WvOut::FILE_TYPE WvOut :: WVOUT_WAV = 2;
const WvOut::FILE_TYPE WvOut :: WVOUT_SND = 3;
const WvOut::FILE_TYPE WvOut :: WVOUT_AIF = 4;
const WvOut::FILE_TYPE WvOut :: WVOUT_MAT = 5;

// WAV header structure. See ftp://ftp.isi.edu/in-notes/rfc2361.txt
// for information regarding format codes.
struct wavhdr {
  char riff[4];           // "RIFF"
  SINT32 file_size;      // in bytes
  char wave[4];           // "WAVE"
  char fmt[4];            // "fmt "
  SINT32 chunk_size;     // in bytes (16 for PCM)
  SINT16 format_tag;     // 1=PCM, 2=ADPCM, 3=IEEE float, 6=A-Law, 7=Mu-Law
  SINT16 num_chans;      // 1=mono, 2=stereo
  SINT32 sample_rate;
  SINT32 bytes_per_sec;
  SINT16 bytes_per_samp; // 2=16-bit mono, 4=16-bit stereo
  SINT16 bits_per_samp;
  char data[4];           // "data"
  SINT32 data_length;    // in bytes
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
  char form[4];               // "FORM"
  SINT32 form_size;          // in bytes
  char aiff[4];               // "AIFF" or "AIFC"
  char comm[4];               // "COMM"
  SINT32 comm_size;          // "COMM" chunk size (18 for AIFF, 24 for AIFC)
  SINT16 num_chans;          // number of channels
  unsigned long sample_frames; // sample frames of audio data
  SINT16 sample_size;        // in bits
  unsigned char srate[10];      // IEEE 754 floating point format
};

struct aifssnd {
  char ssnd[4];               // "SSND"
  SINT32 ssnd_size;          // "SSND" chunk size
  unsigned long offset;         // data offset in data block (should be 0)
  unsigned long block_size;     // not used by STK (should be 0)
};

// MAT-file 5 header structure.
struct mathdr {
  char heading[124];   // Header text field
  SINT16 hff[2];      // Header flag fields
  SINT32 adf[11];     // Array data format fields
  // There's more, but it's of variable length
};

WvOut :: WvOut()
{
  init();
}

WvOut::WvOut( const char *fileName, unsigned int nChannels, FILE_TYPE type, Stk::STK_FORMAT format )
{
  init();
  openFile( fileName, nChannels, type, format );
}

WvOut :: ~WvOut()
{
  closeFile();

  if (data)
    delete [] data;
}

void WvOut :: init()
{
  fd = 0;
  data = 0;
  fileType = 0;
  dataType = 0;
  channels = 0;
  counter = 0;
  totalCount = 0;
}

void WvOut :: closeFile( void )
{
  if ( fd ) {
    // If there's an existing file, close it first.
    writeData( counter );

    if ( fileType == WVOUT_RAW )
      fclose( fd );
    else if ( fileType == WVOUT_WAV )
      closeWavFile();
    else if ( fileType == WVOUT_SND )
      closeSndFile();
    else if ( fileType == WVOUT_AIF )
      closeAifFile();
    else if ( fileType == WVOUT_MAT )
      closeMatFile();
    fd = 0;

    printf("%f Seconds Computed\n\n", getTime() );
    totalCount = 0;
  }
}

void WvOut :: openFile( const char *fileName, unsigned int nChannels, WvOut::FILE_TYPE type, Stk::STK_FORMAT format )
{
  closeFile();

  if ( nChannels < 1 ) {
    sprintf(msg, "WvOut: the channels argument must be greater than zero!");
    handleError( msg, StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels;
  channels = nChannels;
  fileType = type;

  if ( format != STK_SINT8 && format != STK_SINT16 &&
       format != STK_SINT32 && format != MY_FLOAT32 && 
       format != MY_FLOAT64 ) {
    sprintf( msg, "WvOut: Unknown data type specified (%ld).", format );
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  } 
  dataType = format;

  bool result = false;
  if ( fileType == WVOUT_RAW ) {
    if ( channels != 1 ) {
      sprintf(msg, "WvOut: STK RAW files are, by definition, always monaural (channels = %d not supported)!", nChannels);
      handleError( msg, StkError::FUNCTION_ARGUMENT );
    }
    result = setRawFile( fileName );
  }
  else if ( fileType == WVOUT_WAV )
    result = setWavFile( fileName );
  else if ( fileType == WVOUT_SND )
    result = setSndFile( fileName );
  else if ( fileType == WVOUT_AIF )
    result = setAifFile( fileName );
  else if ( fileType == WVOUT_MAT )
    result = setMatFile( fileName );
  else {
    sprintf(msg, "WvOut: Unknown file type specified (%ld).", fileType);
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  }

  if ( result == false )
    handleError(msg, StkError::FILE_ERROR);

  // Allocate new memory if necessary.
  if ( lastChannels < channels ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[BUFFER_SIZE*channels];
  }
  counter = 0;
}

bool WvOut :: setRawFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".raw") == NULL) strcat(name, ".raw");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "WvOut: Could not create RAW file: %s", name);
    return false;
  }

  if ( dataType != STK_SINT16 ) {
    dataType = STK_SINT16;
    sprintf(msg, "WvOut: Using 16-bit signed integer data format for file %s.", name);
    handleError(msg, StkError::WARNING);
  }

  byteswap = false;
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
#endif

  printf("\nCreating RAW file: %s\n", name);
  return true;
}

bool WvOut :: setWavFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".wav") == NULL) strcat(name, ".wav");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "WvOut: Could not create WAV file: %s", name);
    return false;
  }



  struct wavhdr hdr = {"RIF", 44, "WAV", "fmt", 16, 1, 1,
                        (SINT32) Stk::sampleRate(), 0, 2, 16, "dat", 0};
  hdr.riff[3] = 'F';
  hdr.wave[3] = 'E';
  hdr.fmt[3]  = ' ';
  hdr.data[3] = 'a';
  hdr.num_chans = (SINT16) channels;
  if ( dataType == STK_SINT8 )
    hdr.bits_per_samp = 8;
  else if ( dataType == STK_SINT16 )
    hdr.bits_per_samp = 16;
  else if ( dataType == STK_SINT32 )
    hdr.bits_per_samp = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 32;
  }
  else if ( dataType == MY_FLOAT64 ) {
    hdr.format_tag = 3;
    hdr.bits_per_samp = 64;
  }
  hdr.bytes_per_samp = (SINT16) (channels * hdr.bits_per_samp / 8);
  hdr.bytes_per_sec = (SINT32) (hdr.sample_rate * hdr.bytes_per_samp);

  byteswap = false;
#ifndef __LITTLE_ENDIAN__
  byteswap = true;
  swap32((unsigned char *)&hdr.file_size);
  swap32((unsigned char *)&hdr.chunk_size);
  swap16((unsigned char *)&hdr.format_tag);
  swap16((unsigned char *)&hdr.num_chans);
  swap32((unsigned char *)&hdr.sample_rate);
  swap32((unsigned char *)&hdr.bytes_per_sec);
  swap16((unsigned char *)&hdr.bytes_per_samp);
  swap16((unsigned char *)&hdr.bits_per_samp);
#endif

  if ( fwrite(&hdr, 4, 11, fd) != 11 ) {
    sprintf(msg, "WvOut: Could not write WAV header for file %s", name);
    return false;
  }

  printf("\nCreating WAV file: %s\n", name);
  return true;
}

void WvOut :: closeWavFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * channels * bytes_per_sample;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd, 40, SEEK_SET); // jump to data length
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * channels * bytes_per_sample + 44;
#ifndef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);
  fclose( fd );
}

bool WvOut :: setSndFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".snd") == NULL) strcat(name, ".snd");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "WvOut: Could not create SND file: %s", name);
    return false;
  }

  struct sndhdr hdr = {".sn", 40, 0, 3, (SINT32) Stk::sampleRate(), 1, "Created by STK"};
  hdr.pref[3] = 'd';
  hdr.num_channels = channels;
  if ( dataType == STK_SINT8 )
    hdr.format = 2;
  else if ( dataType == STK_SINT16 )
    hdr.format = 3;
  else if ( dataType == STK_SINT32 )
    hdr.format = 5;
  else if ( dataType == MY_FLOAT32 )
    hdr.format = 6;
  else if ( dataType == MY_FLOAT64 )
    hdr.format = 7;

  byteswap = false;
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
  swap32 ((unsigned char *)&hdr.hdr_length);
  swap32 ((unsigned char *)&hdr.format);
  swap32 ((unsigned char *)&hdr.sample_rate);
  swap32 ((unsigned char *)&hdr.num_channels);
#endif

  if ( fwrite(&hdr, 4, 10, fd) != 10 ) {
    sprintf(msg, "WvOut: Could not write SND header for file %s", name);
    return false;
  }

  printf("\nCreating SND file: %s\n", name);
  return true;
}

void WvOut :: closeSndFile( void )
{
  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  SINT32 bytes = totalCount * bytes_per_sample * channels;
#ifdef __LITTLE_ENDIAN__
  swap32 ((unsigned char *)&bytes);
#endif
  fseek(fd, 8, SEEK_SET); // jump to data size
  fwrite(&bytes, 4, 1, fd);
  fclose(fd);
}

bool WvOut :: setAifFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".aif") == NULL) strcat(name, ".aif");
  fd = fopen(name, "wb");
  if ( !fd ) {
    sprintf(msg, "WvOut: Could not create AIF file: %s", name);
    return false;
  }

  // Common parts of AIFF/AIFC header.
  struct aifhdr hdr = {"FOR", 46, "AIF", "COM", 18, 0, 0, 16, "0"};
  struct aifssnd ssnd = {"SSN", 8, 0, 0};
  hdr.form[3] = 'M';
  hdr.aiff[3] = 'F';
  hdr.comm[3] = 'M';
  ssnd.ssnd[3] = 'D';
  hdr.num_chans = channels;
  if ( dataType == STK_SINT8 )
    hdr.sample_size = 8;
  else if ( dataType == STK_SINT16 )
    hdr.sample_size = 16;
  else if ( dataType == STK_SINT32 )
    hdr.sample_size = 32;
  else if ( dataType == MY_FLOAT32 ) {
    hdr.aiff[3] = 'C';
    hdr.sample_size = 32;
    hdr.comm_size = 24;
  }
  else if ( dataType == MY_FLOAT64 ) {
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

  byteswap = false;  
#ifdef __LITTLE_ENDIAN__
  byteswap = true;
  swap32((unsigned char *)&hdr.form_size);
  swap32((unsigned char *)&hdr.comm_size);
  swap16((unsigned char *)&hdr.num_chans);
  swap16((unsigned char *)&hdr.sample_size);
  swap32((unsigned char *)&ssnd.ssnd_size);
  swap32((unsigned char *)&ssnd.offset);
  swap32((unsigned char *)&ssnd.block_size);
#endif

  // The structure boundaries don't allow a single write of 54 bytes.
  if ( fwrite(&hdr, 4, 5, fd) != 5 ) goto error;
  if ( fwrite(&hdr.num_chans, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_frames, 4, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.sample_size, 2, 1, fd) != 1 ) goto error;
  if ( fwrite(&hdr.srate, 10, 1, fd) != 1 ) goto error;

  if ( dataType == MY_FLOAT32 ) {
    char type[4] = {'f','l','3','2'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  else if ( dataType == MY_FLOAT64 ) {
    char type[4] = {'f','l','6','4'};
    char zeroes[2] = { 0, 0 };
    if ( fwrite(&type, 4, 1, fd) != 1 ) goto error;
    if ( fwrite(&zeroes, 2, 1, fd) != 1 ) goto error;
  }
  
  if ( fwrite(&ssnd, 4, 4, fd) != 4 ) goto error;

  printf("\nCreating AIF file: %s\n", name);
  return true;

 error:
  sprintf(msg, "WvOut: Could not write AIF header for file %s", name);
  return false;
}

void WvOut :: closeAifFile( void )
{
  unsigned long frames = (unsigned long) totalCount;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&frames);
#endif
  fseek(fd, 22, SEEK_SET); // jump to "COMM" sample_frames
  fwrite(&frames, 4, 1, fd);

  int bytes_per_sample = 1;
  if ( dataType == STK_SINT16 )
    bytes_per_sample = 2;
  else if ( dataType == STK_SINT32 || dataType == MY_FLOAT32 )
    bytes_per_sample = 4;
  else if ( dataType == MY_FLOAT64 )
    bytes_per_sample = 8;

  unsigned long bytes = totalCount * bytes_per_sample * channels + 46;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  fseek(fd, 4, SEEK_SET); // jump to file size
  fwrite(&bytes, 4, 1, fd);

  bytes = totalCount * bytes_per_sample * channels + 8;
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 ) bytes += 6;
#ifdef __LITTLE_ENDIAN__
  swap32((unsigned char *)&bytes);
#endif
  if ( dataType == MY_FLOAT32 || dataType == MY_FLOAT64 )
    fseek(fd, 48, SEEK_SET); // jump to "SSND" chunk size
  else
    fseek(fd, 42, SEEK_SET); // jump to "SSND" chunk size
  fwrite(&bytes, 4, 1, fd);

  fclose( fd );
}

bool WvOut :: setMatFile( const char *fileName )
{
  char name[128];
  strncpy(name, fileName, 128);
  if ( strstr(name, ".mat") == NULL) strcat(name, ".mat");
  fd = fopen(name, "w+b");
  if ( !fd ) {
    sprintf(msg, "WvOut: Could not create MAT file: %s", name);
    return false;
  }

  if ( dataType != MY_FLOAT64 ) {
    dataType = MY_FLOAT64;
    sprintf(msg, "WvOut: Using 64-bit floating-point data format for file %s", name);
    handleError(msg, StkError::WARNING);
  }

  struct mathdr hdr;
  strcpy(hdr.heading,"MATLAB 5.0 MAT-file, Generated using the Synthesis ToolKit in C++ (STK). By Perry R. Cook and Gary P. Scavone, 1995-2002.");

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
  hdr.adf[8] = (SINT32) channels; // This is the number of rows
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
  if ( fwrite(&hdr, 172, 1, fd) != 1 ) goto error;

  // Write MATLAB array name
  SINT32 tmp;
  if (namelength > 4) {
    if ( fwrite(&namelength, 4, 1, fd) != 1) goto error;
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = (SINT32) ceil((float)namelength / 8);
    if ( fseek(fd, tmp*8-namelength, SEEK_CUR) == -1 ) goto error;
    headsize += tmp * 8;
  }
  else { // Compressed data element format
    if ( fwrite(arrayName, namelength, 1, fd) != 1 ) goto error;
    tmp = 4 - namelength;
    if ( fseek(fd, tmp, SEEK_CUR) == -1 ) goto error;
  }

  // Finish writing known header information
  tmp = 9;        // Matlab IEEE 754 double data type
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  tmp = 0;        // Size of real part subelement in bytes (8 per sample)
  if ( fwrite(&tmp, 4, 1, fd) != 1 ) goto error;
  headsize += 8;  // Total number of bytes in data element so far

  if ( fseek(fd, 132, SEEK_SET) == -1 ) goto error;
  if ( fwrite(&headsize, 4, 1, fd) != 1 ) goto error; // Write header size ... will update at end
  if ( fseek(fd, 0, SEEK_END) == -1 ) goto error;

  byteswap = false;
  printf("\nCreating MAT-file (%s) containing MATLAB array: %s\n", name, arrayName);
  return true;

 error:
  sprintf(msg, "WvOut: Could not write MAT-file header for file %s", name);
  return false;
}

void WvOut :: closeMatFile( void )
{
  fseek(fd, 164, SEEK_SET); // jump to number of columns
  fwrite(&totalCount, 4, 1, fd);

  SINT32 headsize, temp;
  fseek(fd, 132, SEEK_SET);  // jump to header size
  fread(&headsize, 4, 1, fd);
  temp = headsize;
  headsize += (SINT32) (totalCount * 8 * channels);
  fseek(fd, 132, SEEK_SET);
  // Write file size (minus some header info)
  fwrite(&headsize, 4, 1, fd);

  fseek(fd, temp+132, SEEK_SET); // jumpt to data size (in bytes)
  temp = totalCount * 8 * channels;
  fwrite(&temp, 4, 1, fd);

  fclose(fd);
}

unsigned long WvOut :: getFrames( void ) const
{
  return totalCount;
}

MY_FLOAT WvOut :: getTime( void ) const
{
  return (MY_FLOAT) totalCount / Stk::sampleRate();
}

void WvOut :: writeData( unsigned long frames )
{
  if ( dataType == STK_SINT8 ) {
    if ( fileType == WVOUT_WAV ) { // 8-bit WAV data is unsigned!
      unsigned char sample;
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        sample = (unsigned char) (data[k] * 127.0 + 128.0);
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
    else {
      signed char sample;
      for ( unsigned long k=0; k<frames*channels; k++ ) {
        sample = (signed char) (data[k] * 127.0);
        //sample = ((signed char) (( data[k] + 1.0 ) * 127.5 + 0.5)) - 128;
        if ( fwrite(&sample, 1, 1, fd) != 1 ) goto error;
      }
    }
  }
  else if ( dataType == STK_SINT16 ) {
    SINT16 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (SINT16) (data[k] * 32767.0);
      //sample = ((SINT16) (( data[k] + 1.0 ) * 32767.5 + 0.5)) - 32768;
      if ( byteswap ) swap16( (unsigned char *)&sample );
      if ( fwrite(&sample, 2, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (SINT32) (data[k] * 2147483647.0);
      //sample = ((SINT32) (( data[k] + 1.0 ) * 2147483647.5 + 0.5)) - 2147483648;
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT32 ) {
    FLOAT32 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT32) (data[k]);
      if ( byteswap ) swap32( (unsigned char *)&sample );
      if ( fwrite(&sample, 4, 1, fd) != 1 ) goto error;
    }
  }
  else if ( dataType == MY_FLOAT64 ) {
    FLOAT64 sample;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      sample = (FLOAT64) (data[k]);
      if ( byteswap ) swap64( (unsigned char *)&sample );
      if ( fwrite(&sample, 8, 1, fd) != 1 ) goto error;
    }
  }
  return;

 error:
  sprintf(msg, "WvOut: Error writing data to file.");
  handleError(msg, StkError::FILE_ERROR);
}

void WvOut :: tick(const MY_FLOAT sample)
{
  if ( !fd ) return;

  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter == BUFFER_SIZE ) {
    writeData( BUFFER_SIZE );
    counter = 0;
  }
}

void WvOut :: tick(const MY_FLOAT *vector, unsigned int vectorSize)
{
  if ( !fd ) return;

  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void WvOut :: tickFrame(const MY_FLOAT *frameVector, unsigned int frames)
{
  if ( !fd ) return;

  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    for ( j=0; j<channels; j++ ) {
      data[counter*channels+j] = frameVector[i*channels+j];
    }
    counter++;
    totalCount++;

    if ( counter == BUFFER_SIZE ) {
      writeData( BUFFER_SIZE );
      counter = 0;
    }
  }
}
