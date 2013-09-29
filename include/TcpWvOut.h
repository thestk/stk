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

#if !defined(__TCPWVOUT_H)
#define __TCPWVOUT_H

#include "WvOut.h"
#include "Socket.h"

class TcpWvOut : protected WvOut
{
 public:
  //! Default constructor ... the socket is not instantiated.
  TcpWvOut();

  //! Overloaded constructor which opens a network connection during instantiation.
  /*!
    An StkError is thrown if a socket error occurs or an invalid argument is specified.
  */
  TcpWvOut(int port, const char *hostname = "localhost", unsigned int nChannels = 1, Stk::STK_FORMAT format = STK_SINT16);

  //! Class destructor.
  ~TcpWvOut();

  //! Connect to the specified host and port and prepare to stream \e nChannels of data in the given data format.
  /*!
    An StkError is thrown if a socket error occurs or an invalid argument is specified.
  */
  void connect(int port, const char *hostname = "localhost", unsigned int nChannels = 1, Stk::STK_FORMAT format = STK_SINT16);

  //! If a connection is open, write out remaining samples in the queue and then disconnect.
  void disconnect(void);

  //! Return the number of sample frames output.
  unsigned long getFrames( void ) const;

  //! Return the number of seconds of data output.
  MY_FLOAT getTime( void ) const;

  //! Output a single sample to all channels in a sample frame.
  /*!
    An StkError is thrown if a socket write error occurs.
  */
  void tick(MY_FLOAT sample);

  //! Output each sample in \e vector to all channels in \e vectorSize sample frames.
  /*!
    An StkError is thrown if a socket write error occurs.
  */
  void tick(const MY_FLOAT *vector, unsigned int vectorSize);

  //! Output the \e frameVector of sample frames of the given length.
  /*!
    An StkError is thrown if a socket write error occurs.
  */
  void tickFrame(const MY_FLOAT *frameVector, unsigned int frames = 1);

 protected:

  // Write a buffer of length \e frames via the socket connection.
  void writeData( long frames );

  char msg[256];
  char *buffer;
  Socket *soket;
  int dataSize;
};

#endif // defined(__TCPWVOUT_H)
