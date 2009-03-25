/***************************************************/
/*! \class TcpWvOut
    \brief STK internet streaming output class.

    This protected WvOut subclass can stream
    data over a network via a TCP socket connection.
    The data is converted to big-endian byte order,
    if necessary, before being transmitted.

    TcpWvOut supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which output
    single samples to all channels in a sample
    frame, from the tickFrame() method, which
    takes a pointer to multi-channel sample
    frame data.

    This class connects to a socket server, the
    port and IP address of which must be specified
    as constructor arguments.  The default data
    type is signed 16-bit integers but any of the
    defined StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "TcpWvOut.h"
#include <string.h>

TcpWvOut :: TcpWvOut()
{
  buffer_ = 0;
  soket_ = 0;
}

TcpWvOut :: TcpWvOut(int port, const char *hostname, unsigned int nChannels, Stk::StkFormat format)
{
  buffer_ = 0;
  soket_ = 0;
  connect( port, hostname, nChannels, format );
}

TcpWvOut :: ~TcpWvOut()
{
  disconnect();
  delete soket_;
  delete [] buffer_;
}

void TcpWvOut :: connect(int port, const char *hostname, unsigned int nChannels, Stk::StkFormat format)
{
  if ( soket_ && soket_->isValid( soket_->id() ) )
    disconnect();

  if (nChannels < 1) {
    errorString_ << "TcpWvOut::connect: the channel argument (" << nChannels << ") must be greater than zero!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels_;
  channels_ = nChannels;

  if ( format == STK_SINT8 ) dataSize_ = 1;
  else if ( format == STK_SINT16 ) dataSize_ = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataSize_ = 4;
  else if ( format == STK_FLOAT64 ) dataSize_ = 8;
  else {
    errorString_ << "TcpWvOut::connect: unknown data type specified (" << format << ").";
    handleError( StkError::FUNCTION_ARGUMENT );
  } 
  dataType_ = format;

  if ( !soket_ )
    soket_ = new Socket( port, hostname );
  else
    soket_->connect( port, hostname );

  // Allocate new memory if necessary.
  if ( lastChannels < channels_ ) {
    data_.resize( BUFFER_SIZE*channels_ );
    if ( buffer_) delete [] buffer_;
    long bytes = dataSize_ * BUFFER_SIZE * channels_;
    buffer_ = (char *) new char[bytes];
  }
  counter_ = 0;
}

void TcpWvOut :: disconnect(void)
{
  if ( soket_ ) {
    writeData( counter_ );
    soket_->close();
  }
}

void TcpWvOut :: writeData( unsigned long frames )
{
  if ( dataType_ == STK_SINT8 ) {
    signed char *ptr = (signed char *) buffer_;
    for ( unsigned long k=0; k<frames*channels_; k++ ) {
      this->clipTest( data_[k] );
      *ptr++ = (signed char) (data_[k] * 127.0);
    }
  }
  else if ( dataType_ == STK_SINT16 ) {
    SINT16 *ptr = (SINT16 *) buffer_;
    for ( unsigned long k=0; k<frames*channels_; k++ ) {
      this->clipTest( data_[k] );
      *ptr = (SINT16) (data_[k] * 32767.0);
#ifdef __LITTLE_ENDIAN__
      swap16 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_SINT32 ) {
    SINT32 *ptr = (SINT32 *) buffer_;
    for ( unsigned long k=0; k<frames*channels_; k++ ) {
      this->clipTest( data_[k] );
      *ptr = (SINT32) (data_[k] * 2147483647.0);
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 *ptr = (FLOAT32 *) buffer_;
    for ( unsigned long k=0; k<frames*channels_; k++ ) {
      this->clipTest( data_[k] );
      *ptr = (FLOAT32) data_[k];
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 *ptr = (FLOAT64 *) buffer_;
    for ( unsigned long k=0; k<frames*channels_; k++ ) {
      this->clipTest( data_[k] );
      *ptr = (FLOAT64) data_[k];
#ifdef __LITTLE_ENDIAN__
      swap64 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }

  long bytes = dataSize_ * frames * channels_;
  if ( soket_->writeBuffer( (const void *)buffer_, bytes, 0 ) < 0 ) {
    errorString_ << "TcpWvOut: connection to socket server failed!";
    handleError( StkError::PROCESS_SOCKET );
  }
}

unsigned long TcpWvOut :: getFrames( void ) const
{
  return totalCount_;
}

StkFloat TcpWvOut :: getTime( void ) const
{
  return (StkFloat) totalCount_ / Stk::sampleRate();
}

void TcpWvOut :: tick( const StkFloat sample )
{
  if ( !soket_ || !soket_->isValid( soket_->id() ) ) return;

  for ( unsigned int j=0; j<channels_; j++ )
    data_[counter_*channels_+j] = sample;

  counter_++;
  totalCount_++;

  if ( counter_ == BUFFER_SIZE ) {
    writeData( BUFFER_SIZE );
    counter_ = 0;
  }
}

void TcpWvOut :: tick( const StkFloat *vector, unsigned int vectorSize )
{
  if ( !soket_ || !soket_->isValid( soket_->id() ) ) return;

  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void TcpWvOut :: tick( const StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "TcpWvOut::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( !soket_ || !soket_->isValid( soket_->id() ) ) return;

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      tick( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      tick( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      tick( frames[iStart + i] );
  }
}

void TcpWvOut :: tickFrame( const StkFloat *frameVector, unsigned int frames )
{
  if ( !soket_ || !soket_->isValid( soket_->id() ) ) return;

  unsigned int j;
  for ( unsigned int i=0; i<frames; i++ ) {
    for ( j=0; j<channels_; j++ ) {
      data_[counter_*channels_+j] = frameVector[i*channels_+j];
    }
    counter_++;
    totalCount_++;

    if ( counter_ == BUFFER_SIZE ) {
      writeData( BUFFER_SIZE );
      counter_ = 0;
    }
  }
}

void TcpWvOut :: tickFrame( const StkFrames& frames )
{
  if ( channels_ != frames.channels() ) {
    errorString_ << "TcpWvOut::tickFrame(): incompatible channel value in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( !soket_ || !soket_->isValid( soket_->id() ) ) return;

  unsigned int j;
  if ( channels_ == 1 || frames.interleaved() ) {
    unsigned long iFrames = 0, iData = counter_;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      for ( j=0; j<channels_; j++ ) {
        data_[iData++] = frames[iFrames++];
      }
      counter_++;
      totalCount_++;

      if ( counter_ == BUFFER_SIZE ) {
        writeData( BUFFER_SIZE );
        counter_ = 0;
      }
    }
  }
  else {
    unsigned int hop = frames.frames();
    unsigned long iData = counter_;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      for ( j=0; j<channels_; j++ ) {
        data_[iData++] = frames[i + j*hop];
      }
      counter_++;
      totalCount_++;

      if ( counter_ == BUFFER_SIZE ) {
        writeData( BUFFER_SIZE );
        counter_ = 0;
      }
    }
  }
}
