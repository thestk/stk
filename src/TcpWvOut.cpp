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
    defined STK_FORMATs are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "TcpWvOut.h"
#include <string.h>

TcpWvOut :: TcpWvOut()
{
  buffer = 0;
  soket = 0;
}

TcpWvOut :: TcpWvOut(int port, const char *hostname, unsigned int nChannels, Stk::STK_FORMAT format)
{
  buffer = 0;
  soket = 0;
  connect( port, hostname, nChannels, format );
}

TcpWvOut :: ~TcpWvOut()
{
  disconnect();
  delete soket;
  delete [] buffer;
}

void TcpWvOut :: connect(int port, const char *hostname, unsigned int nChannels, Stk::STK_FORMAT format)
{
  if ( soket && soket->isValid( soket->socket() ) )
    disconnect();

  if (nChannels < 1) {
    sprintf(msg, "TcpWvOut: the channel argument (%d) must be greater than zero.", nChannels);
    handleError( msg, StkError::FUNCTION_ARGUMENT );
  }

  unsigned int lastChannels = channels;
  channels = nChannels;

  if ( format == STK_SINT8 ) dataSize = 1;
  else if ( format == STK_SINT16 ) dataSize = 2;
  else if ( format == STK_SINT32 || format == STK_FLOAT32 ) dataSize = 4;
  else if ( format == STK_FLOAT64 ) dataSize = 8;
  else {
    sprintf( msg, "TcpWvOut: Unknown data type specified (%ld).", format );
    handleError(msg, StkError::FUNCTION_ARGUMENT);
  } 
  dataType = format;

  if ( !soket )
    soket = new Socket( port, hostname );
  else
    soket->connect( port, hostname );

  // Allocate new memory if necessary.
  if ( lastChannels < channels ) {
    if ( data ) delete [] data;
    data = (MY_FLOAT *) new MY_FLOAT[BUFFER_SIZE*channels];
    if ( buffer) delete [] buffer;
    long bytes = dataSize * BUFFER_SIZE * channels;
    buffer = (char *) new char[bytes];
  }
  counter = 0;
}

void TcpWvOut :: disconnect(void)
{
  if ( soket ) {
    writeData( counter );
    soket->close();
  }
}

void TcpWvOut :: writeData( long frames )
{
  if ( dataType == STK_SINT8 ) {
    signed char *ptr = (signed char *) buffer;
    for ( unsigned long k=0; k<frames*channels; k++ )
      *ptr++ = (signed char) (data[k] * 127.0);
  }
  else if ( dataType == STK_SINT16 ) {
    SINT16 *ptr = (SINT16 *) buffer;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      *ptr = (SINT16) (data[k] * 32767.0);
#ifdef __LITTLE_ENDIAN__
      swap16 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType == STK_SINT32 ) {
    SINT32 *ptr = (SINT32 *) buffer;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      *ptr = (SINT32) (data[k] * 2147483647.0);
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType == STK_FLOAT32 ) {
    FLOAT32 *ptr = (FLOAT32 *) buffer;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      *ptr = (FLOAT32) data[k];
#ifdef __LITTLE_ENDIAN__
      swap32 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }
  else if ( dataType == STK_FLOAT64 ) {
    FLOAT64 *ptr = (FLOAT64 *) buffer;
    for ( unsigned long k=0; k<frames*channels; k++ ) {
      *ptr = (FLOAT64) data[k];
#ifdef __LITTLE_ENDIAN__
      swap64 ((unsigned char *)ptr);
#endif
      ptr++;
    }
  }

  long bytes = dataSize * frames * channels;
  if ( soket->writeBuffer( (const void *)buffer, bytes, 0 ) < 0 ) {
    sprintf(msg, "TcpWvOut: connection to socket server failed!");
    handleError( msg, StkError::PROCESS_SOCKET );
  }
}

unsigned long TcpWvOut :: getFrames( void ) const
{
  return totalCount;
}

MY_FLOAT TcpWvOut :: getTime( void ) const
{
  return (MY_FLOAT) totalCount / Stk::sampleRate();
}

void TcpWvOut :: tick(MY_FLOAT sample)
{
  if ( !soket || !soket->isValid( soket->socket() ) ) return;

  for ( unsigned int j=0; j<channels; j++ )
    data[counter*channels+j] = sample;

  counter++;
  totalCount++;

  if ( counter == BUFFER_SIZE ) {
    writeData( BUFFER_SIZE );
    counter = 0;
  }
}

void TcpWvOut :: tick(const MY_FLOAT *vector, unsigned int vectorSize)
{
  if ( !soket || !soket->isValid( soket->socket() ) ) return;

  for (unsigned int i=0; i<vectorSize; i++)
    tick( vector[i] );
}

void TcpWvOut :: tickFrame(const MY_FLOAT *frameVector, unsigned int frames)
{
  if ( !soket || !soket->isValid( soket->socket() ) ) return;

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
