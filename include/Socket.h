/***************************************************/
/*! \class Socket
    \brief STK TCP socket client/server class.

    This class provides a uniform cross-platform
    TCP socket client or socket server interface.
    Methods are provided for reading or writing
    data buffers to/from connections.  This class
    also provides a number of static functions for
    use with external socket descriptors.

    The user is responsible for checking the values
    returned by the read/write methods.  Values
    less than or equal to zero indicate a closed
    or lost connection or the occurence of an error.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_SOCKET_H
#define STK_SOCKET_H

#include "Stk.h"

class Socket : public Stk
{
 public:
  //! Default constructor which creates a local socket server on port 2006 (or the specified port number).
  /*!
    An StkError will be thrown if a socket error occurs during instantiation.
  */
  Socket( int port = 2006 );

  //! Class constructor which creates a socket client connection to the specified host and port.
  /*!
    An StkError will be thrown if a socket error occurs during instantiation.
  */
  Socket( int port, const char *hostname );

  //! The class destructor closes the socket instance, breaking any existing connections.
  ~Socket();

  //! Connect a socket client to the specified host and port and returns the resulting socket descriptor.
  /*!
    This method is valid for socket clients only.  If it is called for
    a socket server, -1 is returned.  If the socket client is already
    connected, that connection is terminated and a new connection is
    attempted.  Server connections are made using the accept() method.
    An StkError will be thrown if a socket error occurs during
    instantiation. \sa accept
  */
  int connect( int port, const char *hostname = "localhost" );

  //! Close this socket.
  void close( void );

  //! Return the server/client socket descriptor.
  int id( void ) const;

  //! Return the server/client port number.
  int port( void ) const;

  //! If this is a socket server, extract the first pending connection request from the queue and create a new connection, returning the descriptor for the accepted socket.
  /*!
    If no connection requests are pending and the socket has not
    been set non-blocking, this function will block until a connection
    is present.  If an error occurs or this is a socket client, -1 is
    returned.
  */
  int accept( void );

  //! If enable = false, the socket is set to non-blocking mode.  When first created, sockets are by default in blocking mode.
  static void setBlocking( int socket, bool enable );

  //! Close the socket with the given descriptor.
  static void close( int socket );

  //! Returns true if the socket descriptor is valid.
  static bool isValid( int socket );

  //! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
  int writeBuffer(const void *buffer, long bufferSize, int flags = 0);

  //! Write a buffer via the specified socket.  Returns the number of bytes written or -1 if an error occurs.
  static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );

  //! Read a buffer from the socket connection, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
  int readBuffer(void *buffer, long bufferSize, int flags = 0);

  //! Read a buffer via the specified socket.  Returns the number of bytes read or -1 if an error occurs.
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );

 protected:

  int soket_;
  int port_;
  bool server_;

};

#endif // defined(STK_SOCKET_H)
