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

    WvOut currently supports uncompressed WAV,
    AIFF, AIFC, SND (AU), MAT-file (Matlab), and
    STK RAW file formats.  Signed integer (8-,
    16-, and 32-bit) and floating- point (32- and
    64-bit) data types are supported.  STK RAW
    files use 16-bit integers by definition.
    MAT-files will always be written as 64-bit
    floats.  If a data type specification does not
    match the specified file type, the data type
    will automatically be modified.  Compressed
    data types are not supported.

    Currently, WvOut is non-interpolating and the
    output rate is always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_WVOUT_H
#define STK_WVOUT_H

#include "Stk.h"

const unsigned long BUFFER_SIZE = 1024;  // sample frames

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
  WvOut( const char *fileName, unsigned int nChannels = 1, FILE_TYPE type = WVOUT_WAV, Stk::StkFormat format = STK_SINT16 );

  //! Class destructor.
  virtual ~WvOut();

  //! Create a file of the specified type and name and output samples to it in the given data format.
  /*!
    An StkError is thrown for invalid argument values or if an error occurs when initializing the output file.
  */
  void openFile( const char *fileName, unsigned int nChannels = 1,
                 WvOut::FILE_TYPE type = WVOUT_WAV, Stk::StkFormat format = STK_SINT16 );

  //! If a file is open, write out samples in the queue and then close it.
  void closeFile( void );

  //! Return the number of sample frames output.
  unsigned long getFrames( void ) const;

  //! Return the number of seconds of data output.
  StkFloat getTime( void ) const;

  //! Returns \c true if clipping has been detected during output since instantiation or the last reset.
  bool getClipStatus( void ) { return clipping_; };

  //! Reset the clipping status to \c false.
  void resetClipStatus( void ) { clipping_ = false; };

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick(const StkFloat sample);

  //! Output each sample in \e vector to all channels in \e vectorSize sample frames.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tick( const StkFloat *vector, unsigned int vectorSize );

  //! Output a channel of the StkFrames object to all channels of the WvOut object.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if a file
    write error occurs or the \c channel argument is zero or it is
    greater than the number of channels in the StkFrames object.
  */
  virtual void tick( const StkFrames& frames, unsigned int channel = 1 );

  //! Output the \e frameVector of sample frames of the given length.
  /*!
    An StkError is thrown if a file write error occurs.
  */
  virtual void tickFrame( const StkFloat *frameVector, unsigned int frames = 1);

  //! Output the StkFrames data to the WvOut object.
  /*!
    An StkError will be thrown if a file write error occurs or if
    there is an incompatability between the number of channels in the
    WvOut object and that in the StkFrames object.
  */
  virtual void tickFrame( const StkFrames& frames );

 protected:

  // Initialize class variables.
  void init( void );

  // Write data to output file;
  virtual void writeData( unsigned long frames );

  // Check for sample clipping and clamp.
  void clipTest( StkFloat& sample );

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

  FILE *fd_;
  std::valarray<StkFloat> data_;
  FILE_TYPE fileType_;
  StkFormat dataType_;
  unsigned int channels_;
  unsigned long counter_;
  unsigned long totalCount_;
  bool byteswap_;
  bool clipping_;

};

#endif
