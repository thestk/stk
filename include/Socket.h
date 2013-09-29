/***************************************************/
/*! \class Socket
    \brief STK internet socket abstract base class.

    This class provides common functionality for TCP and UDP internet
    socket server and client subclasses.  This class also provides a
    number of static functions for use with external socket
    descriptors.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
*/
/***************************************************/

#ifndef STK_SOCKET_H
#define STK_SOCKET_H

#include "Stk.h"

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  #include <sys/socket.h>
  #include <sys/types.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>

#elif defined(__OS_WINDOWS__)

  #include <winsock.h>

#endif

class Socket : public Stk
{
 public:

  enum ProtocolType {
    PROTO_TCP,
    PROTO_UDP
  };

  //! Class constructor
  Socket();

  //! Class destructor.
  virtual ~Socket();

  //! Close the socket.
  static void close( int socket );

  //! Return the socket descriptor.
  int id( void ) const;

  //! Return the socket port number.
  int port( void ) const;

  //! Returns true if the socket descriptor is valid.
  static bool isValid( int socket );

  //! If enable = false, the socket is set to non-blocking mode.  When first created, sockets are by default in blocking mode.
  static void setBlocking( int socket, bool enable );

  //! Write a buffer over the socket connection.  Returns the number of bytes written or -1 if an error occurs.
  virtual int writeBuffer(const void *buffer, long bufferSize, int flags = 0) = 0;

  //! Read an input buffer, up to length \e bufferSize.  Returns the number of bytes read or -1 if an error occurs.
  virtual int readBuffer(void *buffer, long bufferSize, int flags = 0) = 0;

  //! Write a buffer via the specified socket.  Returns the number of bytes written or -1 if an error occurs.
  static int writeBuffer(int socket, const void *buffer, long bufferSize, int flags );

  //! Read a buffer via the specified socket.  Returns the number of bytes read or -1 if an error occurs.
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );

 protected:

  int soket_;
  int port_;

};

#endif // defined(STK_SOCKET_H)
