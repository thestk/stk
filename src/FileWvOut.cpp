/***************************************************/
/*! \class FileWvOut
    \brief STK audio file output class.

    This class inherits from WvOut.  It provides a "tick-level"
    interface to the FileWrite class.

    FileWvOut writes samples to an audio file and supports
    multi-channel data.  It is important to distinguish the tick()
    methods, which output single samples to all channels in a sample
    frame, from the tickFrame() methods, which take a pointer or
    reference to multi-channel sample frame data.

    See the FileWrite class for a description of the supported audio
    file formats.

    Currently, FileWvOut is non-interpolating and the output rate is
    always Stk::sampleRate().

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#include "FileWvOut.h"

FileWvOut :: FileWvOut( unsigned int bufferFrames )
  :bufferFrames_( bufferFrames )
{
}

FileWvOut::FileWvOut( std::string fileName, unsigned int nChannels, FileWrite::FILE_TYPE type, Stk::StkFormat format, unsigned int bufferFrames )
  :bufferFrames_( bufferFrames )
{
  this->openFile( fileName, nChannels, type, format );
}

FileWvOut :: ~FileWvOut()
{
  this->closeFile();
}

void FileWvOut :: closeFile( void )
{
  if ( file_.isOpen() ) {

    // Output any remaining samples in the buffer before closing.
    if ( bufferIndex_ > 0 ) {
      data_.resize( bufferIndex_, data_.channels() );
      file_.write( data_ );
    }

    file_.close();
    frameCounter_ = 0;
  }
}

void FileWvOut :: openFile( std::string fileName,
                            unsigned int nChannels,
                            FileWrite::FILE_TYPE type,
                            Stk::StkFormat format )
{
  closeFile();

  if ( nChannels < 1 ) {
    errorString_ << "FileWvOut::openFile: the channels argument must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  // An StkError can be thrown by the FileWrite class here.
  file_.open( fileName, nChannels, type, format );

  // Allocate new memory if necessary.
  data_.resize( bufferFrames_, nChannels );

  bufferIndex_ = 0;
  iData_ = 0;
}

void FileWvOut :: incrementFrame( void )
{
  frameCounter_++;
  bufferIndex_++;

  if ( bufferIndex_ == bufferFrames_ ) {
    file_.write( data_ );
    bufferIndex_ = 0;
    iData_ = 0;
  }
}

void FileWvOut :: computeSample( const StkFloat sample )
{
  if ( !file_.isOpen() ) {
    errorString_ << "FileWvOut::computeSample(): no file open!";
    handleError( StkError::WARNING );
    return;
  }

  unsigned int nChannels = data_.channels();
  StkFloat input = sample;
  clipTest( input );
  for ( unsigned int j=0; j<nChannels; j++ )
    data_[iData_++] = input;

  this->incrementFrame();
}

void FileWvOut :: computeFrames( const StkFrames& frames )
{
  if ( !file_.isOpen() ) {
    errorString_ << "FileWvOut::computeFrames(): no file open!";
    handleError( StkError::WARNING );
    return;
  }

  if ( data_.channels() != frames.channels() ) {
    errorString_ << "FileWvOut::computeFrames(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned int j, nChannels = data_.channels();
  if ( nChannels == 1 || frames.interleaved() ) {

    unsigned int iFrames = 0;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      for ( j=0; j<nChannels; j++ ) {
        data_[iData_] = frames[iFrames++];
        clipTest( data_[iData_++] );
      }

      this->incrementFrame();
    }
  }
  else { // non-interleaved frames

    unsigned long hop = frames.frames();
    unsigned int index;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {

      index = i;
      for ( j=0; j<nChannels; j++ ) {
        data_[iData_] = frames[index];
        clipTest( data_[iData_++] );
        index += hop;
      }

      this->incrementFrame();
    }
  }
}

