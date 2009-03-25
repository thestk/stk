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

#ifndef STK_TCPWVIN_H
#define STK_TCPWVIN_H

#include "WvIn.h"
#include "Socket.h"
#include "Thread.h"
#include "Mutex.h"

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
  void listen(unsigned int nChannels = 1, Stk::StkFormat format = STK_SINT16);

  //! Returns true is an input connection exists or input data remains in the queue.
  /*!
    This method will not return false after an input connection has been closed until
    all buffered input data has been read out.
  */
  bool isConnected(void);

  //! Return the average across the last output sample frame.
  StkFloat lastOut(void) const;

  //! Read out the average across one sample frame of data.
  StkFloat tick(void);

  //! Read out vectorSize averaged sample frames of data in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Fill a channel of the StkFrames object with averaged sample frames.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

  //! Return a pointer to the last output sample frame.
  const StkFloat *lastFrame(void) const;

  //! Return a pointer to the next sample frame of data.
  const StkFloat *tickFrame(void);

  //! Read out sample \e frames of data to \e frameVector.
  StkFloat *tickFrame(StkFloat *frameVector, unsigned int frames);

  //! Fill the StkFrames object with sample frames of data and return the same reference.
  /*!
    An StkError will be thrown if there is an incompatability
    between the number of channels in the TcpWvIn object and that in
    the StkFrames object.
  */
  StkFrames& tickFrame( StkFrames& frames );

  // Called by the thread routine to receive data via the socket connection
  // and fill the socket buffer.  This is not intended for general use but
  // had to be made public for access from the thread.
  void receive(void);

protected:

  // Initialize class variables.
  void init( int port );

  // Read buffered socket data into the data buffer ... will block if none available.
  int readData( void );

  Socket *soket_;
  Thread *thread_;
  Mutex mutex_;
  char *buffer_;
  long bufferBytes_;
  long bytesFilled_;
  long writePoint_;
  long readPoint_;
  long counter_;
  int dataSize_;
  bool connected_;
  int fd_;
  thread_info threadInfo_;

};

#endif
