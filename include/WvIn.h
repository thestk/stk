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

#ifndef STK_WVIN_H
#define STK_WVIN_H

// Files larger than CHUNK_THRESHOLD will be copied into memory
// in CHUNK_SIZE increments, rather than completely loaded into
// a buffer at once.

const unsigned long CHUNK_THRESHOLD = 5000000;  // 5 Mb
const long CHUNK_SIZE = 1024;          // sample frames

#include "Stk.h"

class WvIn : public Stk
{
public:
  //! Default constructor.
  WvIn();

  //! Overloaded constructor for file input.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  WvIn( std::string fileName, bool raw = false, bool doNormalize = true );

  //! Class destructor.
  virtual ~WvIn();

  //! Open the specified file and load its data.
  /*!
    An StkError will be thrown if the file is not found, its format is
    unknown, or a read error occurs.
  */
  void openFile( std::string fileName, bool raw = false, bool doNormalize = true );

  //! If a file is open, close it.
  void closeFile(void);

  //! Clear outputs and reset time (file pointer) to zero.
  void reset(void);

  //! Normalize data to a maximum of +-1.0.
  /*!
    For large, incrementally loaded files with integer data types,
    normalization is computed relative to the data type maximum.
    No normalization is performed for incrementally loaded files
    with floating-point data types.
  */
  void normalize(void);

  //! Normalize data to a maximum of \e +-peak.
  /*!
    For large, incrementally loaded files with integer data types,
    normalization is computed relative to the data type maximum 
    (\e peak/maximum).  For incrementally loaded files with floating-
    point data types, direct scaling by \e peak is performed.
  */
  void normalize(StkFloat peak);

  //! Return the file size in sample frames.
  unsigned long getSize(void) const;

  //! Return the number of audio channels in the file.
  unsigned int getChannels(void) const;

  //! Return the input file sample rate in Hz (not the data read rate).
  /*!
    WAV, SND, and AIF formatted files specify a sample rate in
    their headers.  STK RAW files have a sample rate of 22050 Hz
    by definition.  MAT-files are assumed to have a rate of 44100 Hz.
  */
  StkFloat getFileRate(void) const;

  //! Query whether reading is complete.
  bool isFinished(void) const;

  //! Set the data read rate in samples.  The rate can be negative.
  /*!
    If the rate value is negative, the data is read in reverse order.
  */
  void setRate(StkFloat aRate);

  //! Increment the read pointer by \e aTime samples.
  virtual void addTime(StkFloat aTime);

  //! Turn linear interpolation on/off.
  /*!
    Interpolation is automatically off when the read rate is
    an integer value.  If interpolation is turned off for a
    fractional rate, the time index is truncated to an integer
    value.
  */
  void setInterpolate(bool doInterpolate);

  //! Return the average across the last output sample frame.
  virtual StkFloat lastOut(void) const;

  //! Read out the average across one sample frame of data.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual StkFloat tick(void);

  //! Read out vectorSize averaged sample frames of data in \e vector.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with averaged sample frames.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if a file
    is read incrementally and a read error occurs or the \c channel
    argument is zero or it is greater than the number of channels in
    the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Return a pointer to the last output sample frame.
  virtual const StkFloat *lastFrame(void) const;

  //! Return a pointer to the next sample frame of data.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual const StkFloat *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  /*!
    An StkError will be thrown if a file is read incrementally and a read error occurs.
  */
  virtual StkFloat *tickFrame(StkFloat *frameVector, unsigned int frames);

  //! Fill the StkFrames object with sample frames of data and return the same reference.
  /*!
    An StkError will be thrown if a file is read incrementally and
    a read error occurs or if there is an incompatability between the
    number of channels in the WvIn object and that in the StkFrames
    object.
  */
  virtual StkFrames& tickFrame( StkFrames& frames );

protected:

  // Initialize class variables.
  void init( void );

  // Read file data.
  virtual void readData(unsigned long index);

  // Get STK RAW file information.
  bool getRawInfo( const char *fileName );

  // Get WAV file header information.
  bool getWavInfo( const char *fileName );

  // Get SND (AU) file header information.
  bool getSndInfo( const char *fileName );

  // Get AIFF file header information.
  bool getAifInfo( const char *fileName );

  // Get MAT-file header information.
  bool getMatInfo( const char *fileName );

  FILE *fd_;
  StkFloat *data_;
  StkFloat *lastOutputs_;
  bool chunking_;
  bool finished_;
  bool interpolate_;
  bool byteswap_;
  unsigned long fileSize_;
  unsigned long bufferSize_;
  unsigned long dataOffset_;
  unsigned int channels_;
  long chunkPointer_;
  StkFormat dataType_;
  StkFloat fileRate_;
  StkFloat gain_;
  StkFloat time_;
  StkFloat rate_;
};

#endif
