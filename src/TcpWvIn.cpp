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
    STK_FORMATs are  permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "TcpWvIn.h"

#define N_BUFFERS 10

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
    Thread::test();
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
  connected = false;
  threadInfo.finished = true;
  thread->wait( -1 );
  delete thread;

  delete soket;

  if (buffer)
    delete [] buffer;
}

void TcpWvIn :: init( int port )
{
  buffer = 0;
  bufferBytes = 0;
  connected = false;

  // Start socket server ... an error can be thrown from the Socket class.
  soket = new Socket( port );

  thread = new Thread();
  threadInfo.finished = false;
  threadInfo.object = (void *) this;

  // Start the input thread.
  if ( !thread->start( &inputThread, &threadInfo ) ) {
    sprintf(msg, "TcpWvIn: Unable to start input thread!");
    handleError( msg, StkError::PROCESS_THREAD );
  }
}

void TcpWvIn :: listen(unsigned int nChannels, Stk::STK_FORMAT format)
{
  mutex.lock();

  if ( connected ) {
    soket->close(fd);
  }

  if (nChannels < 1) {
    sprintf(msg, "TcpWvOut: the channel argument (%d) must be greater than zero.", nChannels);
    handleError( msg, StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels;
  channels = nChannels;

  if ( format == STK_SINT16 ) dataSize = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataSize = 4;
  else if ( format == STK_FLOAT64 ) dataSize = 8;
  else if ( format == STK_SINT8 ) dataSize = 1;
  else {
    sprintf( msg, "TcpWvIn: Unknown data type specified (%ld).", format );
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  } 
  dataType = format;

  int lastBufferBytes = bufferBytes;
  bufferBytes = CHUNK_SIZE * N_BUFFERS * channels * dataSize;

  // Allocate new memory if necessary.
  if ( lastBufferBytes < bufferBytes ) {
    if ( buffer) delete [] buffer;
    buffer = (char *) new char[bufferBytes];
  }
  if ( lastChannels < channels ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[CHUNK_SIZE*channels];
    if ( lastOutput ) delete [] lastOutput;
    lastOutput = (MY_FLOAT *) new MY_FLOAT[channels];
  }

  WvIn::reset();
  counter = 0;
  writePoint = 0;
  bytesFilled = 0;

  // Accept a connection.
  printf("Listening for connection on port %d ... ", soket->port());
  fd = soket->accept();
  if ( fd < 0) {
    sprintf( msg, "TcpWvIn: Couldn't accept connection request!");
    handleError( msg, StkError::PROCESS_SOCKET );
  }
  printf(" connection made!\n\n");

  // Start input thread.
  connected = true;

  mutex.unlock();
}

void TcpWvIn :: receive( void )
{
  if ( !connected ) {
    Stk::sleep(100);
    return;
  }

  fd_set mask;
  FD_ZERO(&mask);
  FD_SET(fd, &mask);

  // The select function will block until data is available for reading.
  select(fd+1, &mask, (fd_set *)0, (fd_set *)0, NULL);

  if (FD_ISSET(fd, &mask)) {
    mutex.lock();
    long unfilled = bufferBytes - bytesFilled;
    if ( unfilled > 0 ) {
      // There's room in our buffer for more data.
      long endPoint = writePoint + unfilled;
      if ( endPoint > bufferBytes ) unfilled -= endPoint - bufferBytes;
      int i = Socket::readBuffer(fd, (void *)&buffer[writePoint], unfilled, 0);
      if ( i <= 0 ) {
        printf("The remote TcpWvIn socket connection has closed.\n\n");
        connected = false;
        mutex.unlock();
        return;
      }
      bytesFilled += i;
      writePoint += i;
      if (writePoint == bufferBytes)
        writePoint = 0;
      mutex.unlock();
    }
    else {
      // Sleep 10 milliseconds AFTER unlocking mutex.
      mutex.unlock();
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
  long bytes = CHUNK_SIZE * channels * dataSize;
  while ( connected && bytesFilled < bytes )
    Stk::sleep( 10 );

  if ( !connected && bytesFilled == 0 ) return 0;
  bytes = ( bytesFilled < bytes ) ? bytesFilled : bytes;

  // Copy samples from buffer to data.
  long samples = bytes / dataSize;
  mutex.lock();
  if ( dataType == STK_SINT16 ) {
    gain = 1.0 / 32767.0;
    SINT16 *buf = (SINT16 *) (buffer+readPoint);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap16((unsigned char *) buf);
#endif
      data[i] = (MY_FLOAT) *buf++;
      data[i] *= gain;
    }
  }
  else if ( dataType == STK_SINT32 ) {
    gain = 1.0 / 2147483647.0;
    SINT32 *buf = (SINT32 *) (buffer+readPoint);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      data[i] = (MY_FLOAT) *buf++;
      data[i] *= gain;
    }
  }
  else if ( dataType == STK_FLOAT32 ) {
    FLOAT32 *buf = (FLOAT32 *) (buffer+readPoint);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap32((unsigned char *) buf);
#endif
      data[i] = (MY_FLOAT) *buf++;
    }
  }
  else if ( dataType == STK_FLOAT64 ) {
    FLOAT64 *buf = (FLOAT64 *) (buffer+readPoint);
    for (int i=0; i<samples; i++ ) {
#ifdef __LITTLE_ENDIAN__
      swap64((unsigned char *) buf);
#endif
      data[i] = (MY_FLOAT) *buf++;
    }
  }
  else if ( dataType == STK_SINT8 ) {
    gain = 1.0 / 127.0;
    signed char *buf = (signed char *) (buffer+readPoint);
    for (int i=0; i<samples; i++ ) {
      data[i] = (MY_FLOAT) *buf++;
      data[i] *= gain;
    }
  }

  readPoint += bytes;
  if ( readPoint == bufferBytes )
    readPoint = 0;
  bytesFilled -= bytes;
  if ( bytesFilled < 0 )
    bytesFilled = 0;

  mutex.unlock();
  return samples / channels;
}

bool TcpWvIn :: isConnected(void)
{
  if ( bytesFilled > 0 || counter > 0 )
    return true;
  else
    return connected;
}

const MY_FLOAT *TcpWvIn :: lastFrame(void) const
{
  return lastOutput;
}

MY_FLOAT TcpWvIn :: lastOut(void) const
{
  return WvIn::lastOut();
}

MY_FLOAT TcpWvIn :: tick(void)
{
  return WvIn::tick();
}

MY_FLOAT *TcpWvIn :: tick(MY_FLOAT *vector, unsigned int vectorSize)
{
  return WvIn::tick( vector, vectorSize );
}

const MY_FLOAT *TcpWvIn :: tickFrame(void)
{
  // If no connection and we've output all samples in the queue, return.
  if ( !connected && bytesFilled == 0 && counter == 0 ) return lastOutput;

  if (counter == 0)
    counter = readData();

  long temp = (CHUNK_SIZE - counter) * channels;
  for (unsigned int i=0; i<channels; i++)
    lastOutput[i] = data[temp++];

  counter--;
  if ( counter < 0 )
    counter = 0;

  return lastOutput;
}

MY_FLOAT *TcpWvIn :: tickFrame(MY_FLOAT *frameVector, unsigned int frames)
{
  return WvIn::tickFrame( frameVector, frames );
}
