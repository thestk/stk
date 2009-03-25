/***************************************************/
/*! \class InetWvIn
    \brief STK internet streaming input class.

    This Wvin subclass reads streamed audio data over a network via a
    TCP or UDP socket connection.  The data is assumed in big-endian,
    or network, byte order.  Only a single socket connection is
    supported.

    InetWvIn supports multi-channel data.  It is important to
    distinguish the tick() methods, which return samples produced by
    averaging across sample frames, from the tickFrame() methods,
    which return references or pointers to multi-channel sample
    frames.

    This class implements a socket server.  When using the TCP
    protocol, the server "listens" for a single remote connection
    within the InetWvIn::start() function.  For the UDP protocol, no
    attempt is made to verify packet delivery or order.  The default
    data type for the incoming stream is signed 16-bit integers,
    though any of the defined StkFormats are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_INETWVIN_H
#define STK_INETWVIN_H

#include "WvIn.h"
#include "TcpServer.h"
#include "UdpSocket.h"
#include "Thread.h"
#include "Mutex.h"

typedef struct {
  bool finished;
  void *object;
} ThreadInfo;

class InetWvIn : public WvIn
{
public:
  //! Default constructor.
  /*!
    An StkError will be thrown if an error occurs while initializing the input thread.
  */
  InetWvIn( unsigned long bufferFrames = 1024, unsigned int nBuffers = 8 );

  //! Class destructor.
  ~InetWvIn();

  //! Wait for a (new) socket connection with specified protocol, port, data channels and format.
  /*!
    For the UDP protocol, this function will create a socket
    instance and return.  For the TCP protocol, this function will
    block until a connection is established.  An StkError will be
    thrown if a socket error occurs or an invalid function argument is
    provided.
  */
  void listen( int port = 2006, unsigned int nChannels = 1,
               Stk::StkFormat format = STK_SINT16,
               Socket::ProtocolType protocol = Socket::PROTO_TCP );

  //! Returns true is an input connection exists or input data remains in the queue.
  /*!
    This method will not return false after an input connection has been closed until
    all buffered input data has been read out.
  */
  bool isConnected( void );

  // Called by the thread routine to receive data via the socket connection
  // and fill the socket buffer.  This is not intended for general use but
  // had to be made public for access from the thread.
  void receive( void );

protected:

  // Read buffered socket data into the data buffer ... will block if none available.
  int readData( void );

  void computeFrame( void );

  Socket *soket_;
  Thread thread_;
  Mutex mutex_;
  char *buffer_;
  unsigned long bufferFrames_;
  unsigned long bufferBytes_;
  unsigned long bytesFilled_;
  unsigned int nBuffers_;
  unsigned long writePoint_;
  unsigned long readPoint_;
  long bufferCounter_;
  int dataBytes_;
  bool connected_;
  int fd_;
  ThreadInfo threadInfo_;
  Stk::StkFormat dataType_;

};

#endif
