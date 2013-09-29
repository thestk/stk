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

#if !defined(__WVOUT_H)
#define __WVOUT_H

#include "Stk.h"
#include <stdio.h>

#define BUFFER_SIZE 1024  // sample frames

class WvOut : public Stk
{
 public:

  typedef unsigned long FILE_TYPE;

  static const FILE_TYPE WVOUT_RAW; /*!< STK RAW file type. */
  static const FILE_TYPE WVOUT_WAV; /*!< WAV file type. */
  static const FILE_TYPE WVOUT_SND; /*!< SND (AU) file type. */
  static const FILE_TYPE WVOUT_AIF; /*!< AIFF file type. */
  static const FILE_TYPE WVOUT_MAT; /*!< Matlab MAT-file type. */

  //! Default constructor.
  WvOut();

  //! Overloaded constructor used to specify a file name, type, and data format with this object.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  WvOut( const char *fileName, unsigned int nChannels = 1, FILE_TYPE type = WVOUT_WAV, Stk::STK_FORMAT format = STK_SINT16 );

  //! Class destructor.
  virtual ~WvOut();

  //! Create a file of the specified type and name and output samples to it in the given data format.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  void openFile( const char *fileName, unsigned int nChannels = 1,
                 WvOut::FILE_TYPE type = WVOUT_WAV, Stk::STK_FORMAT format = STK_SINT16 );

  //! If a file is open, write out samples in the queue and then close it.
  void closeFile( void );

  //! Return the number of sample frames output.
  unsigned long getFrames( void ) const;

  //! Return the number of seconds of data output.
  MY_FLOAT getTime( void ) const;

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick(const MY_FLOAT sample);

  //! Output each sample in \e vector to all channels in \e vectorSize sample frames.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick(const MY_FLOAT *vector, unsigned int vectorSize);

  //! Output the \e frameVector of sample frames of the given length.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tickFrame(const MY_FLOAT *frameVector, unsigned int frames = 1);

 protected:

  // Initialize class variables.
  void init( void );

  // Write data to output file;
  virtual void writeData( unsigned long frames );

  // Write STK RAW file header.
  bool setRawFile( const char *fileName );

  // Write WAV file header.
  bool setWavFile( const char *fileName );

  // Close WAV file, updating the header.
  void closeWavFile( void );

  // Write SND (AU) file header.
  bool setSndFile( const char *fileName );

  // Close SND file, updating the header.
  void closeSndFile( void );

  // Write AIFF file header.
  bool setAifFile( const char *fileName );

  // Close AIFF file, updating the header.
  void closeAifFile( void );

  // Write MAT-file header.
  bool setMatFile( const char *fileName );

  // Close MAT-file, updating the header.
  void closeMatFile( void );

  char msg[256];
  FILE *fd;
  MY_FLOAT *data;
  FILE_TYPE fileType;
  STK_FORMAT dataType;
  bool byteswap;
  unsigned int channels;
  unsigned long counter;
  unsigned long totalCount;

};

#endif // defined(__WVOUT_H)
