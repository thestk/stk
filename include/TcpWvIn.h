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
    STK_FORMATs are permissible.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__TCPWVIN_H)
#define __TCPWVIN_H

#include "WvIn.h"
#include "Socket.h"
#include "Thread.h"

typedef struct {
  bool finished;
  void *object;
} thread_info;

class TcpWvIn : protected WvIn
{
public:
  //! Default constructor starts a socket server.  If not specified, the server is associated with port 2006.
  /*!
    An StkError will be thrown if an error occurs while initializing the input thread or starting the socket server.
  */
  TcpWvIn( int port = 2006 );

  //! Class destructor.
  ~TcpWvIn();

  //! Listen for a (new) connection with specified data channels and format.
  /*!
    An StkError will be thrown a socket error or an invalid function argument.
  */
  void listen(unsigned int nChannels = 1, Stk::STK_FORMAT format = STK_SINT16);

  //! Returns TRUE is an input connection exists or input data remains in the queue.
  /*!
    This method will not return FALSE after an input connection has been closed until
    all buffered input data has been read out.
  */
  bool isConnected(void);

  //! Return the average across the last output sample frame.
  MY_FLOAT lastOut(void) const;

  //! Read out the average across one sample frame of data.
  MY_FLOAT tick(void);

  //! Read out vectorSize averaged sample frames of data in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

  //! Return a pointer to the last output sample frame.
  const MY_FLOAT *lastFrame(void) const;

  //! Return a pointer to the next sample frame of data.
  const MY_FLOAT *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  MY_FLOAT *tickFrame(MY_FLOAT *frameVector, unsigned int frames);

  // Called by the thread routine to receive data via the socket connection
  // and fill the socket buffer.  This is not intended for general use but
  // had to be made public for access from the thread.
  void receive(void);

protected:

  // Initialize class variables.
  void init( int port );

  // Read buffered socket data into the data buffer ... will block if none available.
  int readData( void );

  Socket *soket;
  Thread *thread;
  Mutex mutex;
  char *buffer;
  long bufferBytes;
  long bytesFilled;
  long writePoint;
  long readPoint;
  long counter;
  int dataSize;
  bool connected;
  int fd;
  thread_info threadInfo;

};

#endif
