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

#include "Socket.h"
#include <stdio.h>
#include <string.h>

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

Socket :: Socket( int port )
{
  soket_ = -1;
  server_ = true;
  port_ = port;

  // Create a socket server.
#if defined(__OS_WINDOWS__)  // windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    errorString_ << "Socket: Incompatible Windows socket library version!";
    handleError( StkError::PROCESS_SOCKET );
  }
#endif

  // Create the server-side socket
  soket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (soket_ < 0) {
    errorString_ << "Socket: Couldn't create socket server!";
    handleError( StkError::PROCESS_SOCKET );
  }

  int flag = 1;
  int result = setsockopt( soket_, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int) );
  if (result < 0) {
    errorString_ << "Socket: Error setting socket options!";
    handleError( StkError::PROCESS_SOCKET );
  }

  struct sockaddr_in mysocket;
  mysocket.sin_family=AF_INET;
  mysocket.sin_addr.s_addr=INADDR_ANY;
  mysocket.sin_port=htons( port );

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind(soket_, (struct sockaddr *)&mysocket, sizeof(mysocket)) < 0 ) {
    errorString_ << "Socket: Couldn't bind socket!";
    handleError( StkError::PROCESS_SOCKET );
  }

  // Listen for incoming connection(s)
  if ( listen(soket_, 1) < 0 ) {
    errorString_ << "Socket: Couldn't start server listening!";
    handleError( StkError::PROCESS_SOCKET );
  }
}

Socket :: Socket(int port, const char *hostname )
{
  soket_ = -1;
  server_ = false;
  port_ = port;

#if defined(__OS_WINDOWS__)  // windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    errorString_ << "Socket: Incompatible Windows socket library version!";
    handleError( StkError::PROCESS_SOCKET );
  }
#endif

  // Create a socket client connection.
  connect( port, hostname );
}

Socket :: ~Socket()
{
#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  ::close( soket_ );

#elif defined(__OS_WINDOWS__)

  ::closesocket( soket_ );
  WSACleanup();

#endif
}

int Socket :: connect( int port, const char *hostname )
{
  // This method is for client connections only!
  if ( server_ == true ) return -1;

  // Close an existing connection if it exists.
  if ( isValid( soket_ ) ) this->close();

  // Create the client-side socket
  soket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (soket_ < 0) {
    errorString_ << "Socket: Couldn't create socket client!";
    handleError( StkError::PROCESS_SOCKET );
  }

  int flag = 1;
  int result = setsockopt( soket_, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int) );
  if (result < 0) {
    errorString_ << "Socket: Error setting socket options!";
    handleError( StkError::PROCESS_SOCKET );
  }

  struct hostent *hostp;
  if ( (hostp = gethostbyname(hostname)) == 0 ) {
    errorString_ << "Socket: unknown host (" << hostname << ")!";
    handleError( StkError::PROCESS_SOCKET_IPADDR );
  }

  // Fill in the address structure
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  memcpy((void *)&server_address.sin_addr, hostp->h_addr, hostp->h_length);
  server_address.sin_port = htons(port);

  // Connect to the server
  if ( ::connect(soket_, (struct sockaddr *)&server_address,
                 sizeof(server_address) ) < 0) {
    errorString_ << "Socket: Couldn't connect to socket server!";
    handleError( StkError::PROCESS_SOCKET );
  }

  return soket_;
}

int Socket :: id( void ) const
{
  return soket_;
}

int Socket :: port( void ) const
{
  return port_;
}

int Socket :: accept( void )
{
  if ( server_ )
    return ::accept( soket_, NULL, NULL );
  else
    return -1;
}

bool Socket :: isValid( int socket )
{
  return socket != -1;
}

void Socket :: setBlocking( int socket, bool enable )
{
  if ( !isValid( socket ) ) return;

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  int tmp = ::fcntl(socket, F_GETFL, 0);
  if ( tmp >= 0 )
    tmp = ::fcntl( socket, F_SETFL, enable ? (tmp &~ O_NONBLOCK) : (tmp | O_NONBLOCK) );

#elif defined(__OS_WINDOWS__)

  unsigned long non_block = !enable;
  ioctlsocket( socket, FIONBIO, &non_block );

#endif
}

void Socket :: close( void )
{
  if ( !isValid( soket_ ) ) return;
  this->close( soket_ );
  soket_ = -1;
}

void Socket :: close( int socket )
{
  if ( !isValid( socket ) ) return;

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  ::close( socket );

#elif defined(__OS_WINDOWS__)

  ::closesocket( socket );

#endif
}

int Socket :: writeBuffer(const void *buffer, long bufferSize, int flags )
{
  if ( !isValid( soket_ ) ) return -1;
  return send( soket_, (const char *)buffer, bufferSize, flags );
}

int Socket :: writeBuffer(int socket, const void *buffer, long bufferSize, int flags )
{
  if ( !isValid( socket ) ) return -1;
  return send( socket, (const char *)buffer, bufferSize, flags );
}

int Socket :: readBuffer(void *buffer, long bufferSize, int flags )
{
  if ( !isValid( soket_ ) ) return -1;
  return recv( soket_, (char *)buffer, bufferSize, flags );
}

int Socket :: readBuffer(int socket, void *buffer, long bufferSize, int flags )
{
  if ( !isValid( socket ) ) return -1;
  return recv( socket, (char *)buffer, bufferSize, flags );
}
