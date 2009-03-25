/***************************************************/
/*! \class TcpWvIn
    \brief STK internet streaming input class.

    This protected Wvin subclass can read streamed
    data over a network via a TCP socket connection.
    The data is assumed in big-endian, or network,
    byte order.

    TcpWvIn supports multi-channel data in
    interleaved format.  It is important to
    distinguish the tick() methods, which return
    samples produced by averaging across sample
    frames, from the tickFrame() methods, which
    return pointers to multi-channel sample frames.
    For single-channel data, these methods return
    equivalent values.

    This class starts a socket server, which waits
    for a single remote connection.  The default
    data type for the incoming stream is signed
    16-bit integers, though any of the defined
    StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "TcpWvIn.h"

const int N_BUFFERS = 10;

// Do OS dependent includes
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))
  #include <sys/types.h>
  #include <unistd.h>
  #include <string.h>
#endif

extern "C" THREAD_RETURN THREAD_TYPE inputThread(void * ptr)
{
  thread_info *info = (thread_info *)ptr;

  while ( !info->finished ) {
    ((TcpWvIn *) info->object)->receive();
  }

  return 0;
}

TcpWvIn :: TcpWvIn( int port )
{
  init( port );
}

TcpWvIn :: ~TcpWvIn()
{
  // Close down the thread.
  connected_ = false;
  threadInfo_.finished = true;
  delete thread_;

  delete soket_;

  if (buffer_)
    delete [] buffer_;
}

void TcpWvIn :: init( int port )
{
  buffer_ = 0;
  bufferBytes_ = 0;
  connected_ = false;

  // Start socket server ... an error can be thrown from the Socket class.
  soket_ = new Socket( port );

  thread_ = new Thread();
  threadInfo_.finished = false;
  threadInfo_.object = (void *) this;

  // Start the input thread.
  if ( !thread_->start( &inputThread, &threadInfo_ ) ) {
    errorString_ << "TcpWvIn::init: unable to start input thread!";
    handleError( StkError::PROCESS_THREAD );
  }
}

void TcpWvIn :: listen(unsigned int nChannels, Stk::StkFormat format)
{
  mutex_.lock();

  if ( connected_ ) {
    soket_->close(fd_);
  }

  if ( nChannels < 1 ) {
    errorString_ << "TcpWvOut::listen: the channel argument (" << nChannels << ") must be greater than zero.";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels_;
  channels_ = nChannels;

  if ( format == STK_SINT16 ) dataSize_ = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataSize_ = 4;
  else if ( format == STK_FLOAT64 ) dataSize_ = 8;
  else if ( format == STK_SINT8 ) dataSize_ = 1;
  else {
    errorString_ << "TcpWvIn::listen: unknown data type specified (" << format << ").";
    handleError( StkError::FUNCTION_ARGUMENT );
  } 
  dataType_ = format;

  int lastBufferBytes = bufferBytes_;
  bufferBytes_ = CHUNK_SIZE * N_BUFFERS * channels_ * dataSize_;

  // Allocate new memory if necessary.
  if ( lastBufferBytes < bufferBytes_ ) {
    if ( buffer_) delete [] buffer_;
    buffer_ = (char *) new char[bufferBytes_];
  }
  if ( lastChannels < channels_ ) {
    if ( data_ ) delete [] data_;
    data_ = (StkFloat *) new StkFloat[CHUNK_SIZE*channels_];
    if ( lastOutputs_ ) delete [] lastOutputs_;
    lastOutputs_ = (StkFloat *) new StkFloat[channels_];
  }

  WvIn::reset();
  counter_ = 0;
  writePoint_ = 0;
  bytesFilled_ = 0;

  // Accept a connection.
  errorString_ << "TcpWvIn: listening for connection on port " << soket_->port() << " ... ";
  handleError( StkError::STATUS );
  fd_ = soket_->accept();
  if ( fd_ < 0) {
    errorString_ << "TcpWvIn: Could not accept connection request!";
    handleError( StkError::PROCESS_SOCKET );
  }
  errorString_ << "TcpWvIn::listen: connection made!";
  handleError( StkError::STATUS );

  // Start input thread.
  connected_ = true;

  mutex_.unlock();
}

void TcpWvIn :: receive( void )
{
  if ( !connected_ ) {
    Stk::sleep(100);
    return;
  }

  fd_set mask;
  FD_ZERO(&mask);
  FD_SET(fd_, &mask);

  // The select function will block until data is available for reading.
  select(fd_+1, &mask, (fd_set *)0, (fd_set *)0, NULL);

  if (FD_ISSET(fd_, &mask)) {
    mutex_.lock();
    long unfilled = bufferBytes_ - bytesFilled_;
    if ( unfilled > 0 ) {
      // There's room in our buffer for more data.
      long endPoint = writePoint_ + unfilled;
      if ( endPoint > bufferBytes_ ) unfilled -= endPoint - bufferBytes_;
      int i = Socket::readBuffer(fd_, (void *)&buffer_[writePoint_], unfilled, 0);
      if ( i <= 0 ) {
        errorString_ << "TcpWvIn::receive: the remote TcpWvIn socket connection has closed.";
        handleError( StkError::STATUS );
        connected_ = false;
        mutex_.unlock();
        return;
      }
      bytesFilled_ += i;
      writePoint_ += i;
      if (writePoint_ == bufferBytes_)
        writePoint_ = 0;
      mutex_.unlock();
    }
    else {
      // Sleep 10 milliseconds AFTER unlocking mutex.
      mutex_.unlock();
      Stk::sleep( 10 );
    }
  }
}

int TcpWvIn :: readData( void )
{
  // We have two potential courses of action should this method
  // be called and the input buffer isn't sufficiently filled.
  // One solution is to fill the data buffer with zeros and return.
  // The other solution is to wait until the necessary data exists.
  // I chose the latter, as it works for both streamed files
  // (non-realtime data transport) and realtime playback (given
  // adequate network bandwidth and speed).

  // Wait until data is ready.
  long bytes = CHUNK_SIZE * channels_ * dataSize_;
  while ( connected_ && bytesFilled_ < bytes )
    Stk::sleep( 10 );

  if ( !connected_ && bytesFilled_ == 0 ) return 0;
  bytes = ( bytesFilled_ < bytes ) ? bytesFilled_ : bytes;

  // Copy samples from buffer to data.
  long samples = bytes / dataSize_;
  mutex_.lock();
  if ( dataType_ == STK_SINT16 ) {
    gain_ = 1.0 / 32767.0;
    SINT16 *buf = (SINT16 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap16((unsigned char *) buf);
#endif
      data_[i] = (StkFloat) *buf++;
      data_[i] *= gain_;
    }
  }
  else if ( dataType_ == STK_SINT32 ) {
    gain_ = 1.0 / 2147483647.0;
    SINT32 *buf = (SINT32 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      data_[i] = (StkFloat) *buf++;
      data_[i] *= gain_;
    }
  }
  else if ( dataType_ == STK_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      data_[i] = (StkFloat) *buf++;
    }
  }
  else if ( dataType_ == STK_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap64((unsigned char *) buf);
#endif
      data_[i] = (StkFloat) *buf++;
    }
  }
  else if ( dataType_ == STK_SINT8 ) {
    gain_ = 1.0 / 127.0;
    signed char *buf = (signed char *) (buffer_+readPoint_);
    for (int i=0; i<samples; i++ ) {
      data_[i] = (StkFloat) *buf++;
      data_[i] *= gain_;
    }
  }

  readPoint_ += bytes;
  if ( readPoint_ == bufferBytes_ )
    readPoint_ = 0;
  bytesFilled_ -= bytes;
  if ( bytesFilled_ < 0 )
    bytesFilled_ = 0;

  mutex_.unlock();
  return samples / channels_;
}

bool TcpWvIn :: isConnected(void)
{
  if ( bytesFilled_ > 0 || counter_ > 0 )
    return true;
  else
    return connected_;
}

const StkFloat *TcpWvIn :: lastFrame(void) const
{
  return lastOutputs_;
}

StkFloat TcpWvIn :: lastOut(void) const
{
  return WvIn::lastOut();
}

StkFloat TcpWvIn :: tick(void)
{
  return WvIn::tick();
}

StkFloat *TcpWvIn :: tick(StkFloat *vector, unsigned int vectorSize)
{
  return WvIn::tick( vector, vectorSize );
}

StkFrames& TcpWvIn :: tick( StkFrames& frames, unsigned int channel )
{
  return WvIn::tick( frames, channel );
}

const StkFloat *TcpWvIn :: tickFrame(void)
{
  // If no connection and we've output all samples in the queue, return.
  if ( !connected_ && bytesFilled_ == 0 && counter_ == 0 ) return lastOutputs_;

  if (counter_ == 0)
    counter_ = readData();

  long temp = (CHUNK_SIZE - counter_) * channels_;
  for (unsigned int i=0; i<channels_; i++)
    lastOutputs_[i] = data_[temp++];

  counter_--;
  if ( counter_ < 0 )
    counter_ = 0;

  return lastOutputs_;
}

StkFloat *TcpWvIn :: tickFrame(StkFloat *frameVector, unsigned int frames)
{
  return WvIn::tickFrame( frameVector, frames );
}

StkFrames& TcpWvIn :: tickFrame( StkFrames& frames )
{
  return WvIn::tickFrame( frames );
}
