/******************************************/
/*
  StrmWvOut Audio Output Class,
  by Gary P. Scavone, 2000

  This object inherits from WvOut and is used
  to send 16-bit data (signed integer) via
  a socket connection (streamed audio).
  Streamed data must be in big-endian format,
  which conforms to network byte ordering.

  This class connects to a socket server, the
  port and IP address of which must be specified
  as constructor arguments.  No data buffering
  is currently implemented.  The class simply
  fires off a buffer of data over the socket
  connection as soon as it is filled.
*/
/******************************************/

#include "StrmWvOut.h"

#if defined(__STK_REALTIME_)

#ifdef __LITTLE_ENDIAN__
  #include "ByteSwap.h"
#endif

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#elif defined(__OS_Win_)
#include <winsock.h>

#endif

StrmWvOut :: StrmWvOut(int port, const char *hostname, int chans)
{
  char msg[256];
  struct sockaddr_in server_address;

  if (chans < 1) {
    sprintf(msg, "StrmWvOut: number of channels (%d) must be one or greater.\n", chans);
    throw StkError(msg, StkError::FUNCTION_SYNTAX);
  }

  channels = chans;

#if defined(__OS_Win_)  // Windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    sprintf(msg, "StrmWvOut: Wrong Windoze socket library version!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }
#endif

  // Create the client-side socket
  local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (local_socket < 0) {
    sprintf(msg, "StrmWvOut: Couldn't create socket!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  struct hostent *hostp;

  if ((hostp = gethostbyname(hostname)) == 0) {
    sprintf(msg, "StrmWvOut: unknown host (%s)!\n", hostname);
    throw StkError(msg, StkError::PROCESS_SOCKET_IPADDR);
  }

  // Fill in the address structure
  server_address.sin_family = AF_INET;
  memcpy((void *)&server_address.sin_addr, hostp->h_addr, hostp->h_length);
  server_address.sin_port = htons(port);

  // Connect to the server
  if (connect(local_socket, (struct sockaddr *)&server_address,
              sizeof(server_address) ) < 0) {
#if defined(__OS_Win_)
    closesocket(local_socket);
    WSACleanup();
#else
    close(local_socket);
#endif
    sprintf(msg, "StrmWvOut: Couldn't connect to socket server!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  data_length = RT_BUFFER_SIZE*channels;  // samples
  // Add a few extra samples for good measure
  data = (INT16 *) new INT16[data_length+10];
}

StrmWvOut :: ~StrmWvOut()
{
  while (counter<data_length)	{
    data[counter++] = 0;
  }
  send(local_socket, (const char *)data, data_length*2, 0);

#if defined(__OS_Win_)
  closesocket(local_socket);
  WSACleanup();
#else
  close(local_socket);
#endif
}

void StrmWvOut :: tick(MY_FLOAT sample)
{
  static INT16 isample;

  isample = (INT16) (sample * 32000.0);
#ifdef __LITTLE_ENDIAN__
  swap16 ((unsigned char *)&isample);
#endif
  for (int i=0;i<channels;i++)
    data[counter++] = isample;

  if (counter >= data_length) {
    if (send(local_socket, (const char *)data, data_length*2, 0) < 0) {
      char msg[256];
#if defined(__OS_Win_)
      closesocket(local_socket);
      WSACleanup();
#else
      close(local_socket);
#endif
      sprintf(msg, "StrmWvOut: connection to socket server failed!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }
    counter = 0;
  }
}

void StrmWvOut :: mtick(MY_MULTI samples)
{
  for (int i=0;i<channels;i++) {
    data[counter] = (INT16) (*samples++ * 32000.0);
#ifdef __LITTLE_ENDIAN__
    swap16((unsigned char *)&data[counter]);
#endif
    counter++;
  }

  if (counter >= data_length) {
    if (send(local_socket, (const char *)data, data_length*2, 0) < 0) {
      char msg[256];
#if defined(__OS_Win_)
      closesocket(local_socket);
      WSACleanup();
#else
      close(local_socket);
#endif
      sprintf(msg, "StrmWvOut: connection to socket server failed!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }
    counter = 0;
  }
}

#endif
