/******************************************/
/*
  Controller Class,
  by Gary P. Scavone, 2000

  This object will accept control messages
  from a variety of sources, such as a MIDI
  port, scorefile, socket connection, or
  pipe.  MIDI messages are retrieved with
  the RtMidi class.  All other input sources
  (scorefile, socket, or pipe) are assumed
  to provide SKINI formatted messages.

  For each call of getNextMessage(), the
  active input devices are queried to see
  if a new control message is available.
  Only one message per call is returned, so
  a subsequent call begins querying the
  next available device "after" the previously
  handled one.

  This class is primarily for use in STK
  main() event loops.

  One of my original goals in creating this class
  was to simplify the message acquisition process
  by removing all threads.  If the windoze
  select() function behaved just like the unix one,
  that would have been possible.  Since it does not
  (it can't be used to poll STDIN), I am using a 
  thread to acquire messages from STDIN, which are
  then sent via a socket connection to the message
  socket server.  Perhaps in the future, I will be
  able to simplify things.
*/
/******************************************/

#include "Controller.h"

Controller :: Controller(int inputMask)
{
  source = inputMask;
  default_ticks = RT_BUFFER_SIZE;
  msg_index = 0;
  num_messages = 0;
  score = new SKINI11();

#if defined(__STK_REALTIME_)

  maxfd = 0;
  char msg[256];
  struct sockaddr_in mysocket;
  FD_ZERO(&mask);

  /*
     The fd array is used to hold the file descriptors for
     all connected sockets.  This saves some time incrementing
     through file descriptors when using select().
  */
  for (int i=0; i<16; i++)
    fd[i] = 0;

  if ( source & STK_MIDI )
    midi_input = new RtMidi();

  /*
     If no input source is specified, we assume the input is coming
     from a SKINI scorefile.  If any input source is specified, we
     will always check STDIN, even in STK_PIPE is not specified.
     This provides a means to exit cleanly when reading MIDI or
     in case a socket connection cannot be made after STK_SOCKET has
     been specified.  The current means of polling STDIN is via a 
     thread.
  */
  if (source) {
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
    if (pthread_create(&stdin_thread, NULL, stdinHandler, NULL)) {
#elif defined(__OS_Win_)
    stdin_thread = _beginthread(stdinHandler, 0, NULL);
    if (stdin_thread == 0)  {
#endif
      sprintf(msg, "Controller: unable to create stdin socket thread!\n");
      throw StkError(msg, StkError::PROCESS_THREAD);
    }
    source |= STK_SOCKET;
  }

  /* Let the user know that they can exit the program via the console
     if necessary.
  */
  if ( !(source & STK_PIPE) && inputMask )
    printf("\nType `Exit<cr>' to quit.\n\n");

  if ( source & STK_SOCKET ) {
    // Set up the socket server to accept remote connections
#if defined(__OS_Win_)  // Windoze-only stuff
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1,1);

    WSAStartup(wVersionRequested, &wsaData);
    if (wsaData.wVersion != wVersionRequested)  {   
      sprintf(msg, "Controller: Wrong Windoze socket library version!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }
#endif

    // Create the server-side socket
    local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (local_socket < 0) {
      sprintf(msg, "Controller: Couldn't create socket!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }

    mysocket.sin_family=AF_INET;
    mysocket.sin_addr.s_addr=INADDR_ANY;
    mysocket.sin_port=htons(STK_SOCKET_PORT);

    // Bind socket to the appropriate port and interface (INADDR_ANY)
    if (bind(local_socket, (struct sockaddr *)&mysocket, sizeof(mysocket)) < 0) {
      sprintf(msg, "Controller: Couldn't bind socket!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }

    // Listen for one incoming connection
    if (listen(local_socket, 1) < 0) {
      sprintf(msg, "Controller: Couldn't set up listen on socket!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }
    if (inputMask & STK_SOCKET)
      // Only print the message if STK_SOCKET was initially specified
      printf("Listening for a connection on port %d\n\n", STK_SOCKET_PORT);

    FD_SET(local_socket, &mask);
    if (local_socket > maxfd) maxfd = local_socket;
  }
#endif // __STK_REALTIME
}

Controller :: ~Controller()
{
  delete score;

#if defined(__STK_REALTIME_)

  if ( source & STK_MIDI )
    delete midi_input;

  if ( source & STK_SOCKET ) {
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
    shutdown(local_socket,0);
    pthread_cancel(stdin_thread);
    pthread_join(stdin_thread, NULL);
#elif defined(__OS_Win_)
    TerminateThread((HANDLE)stdin_thread,0);
    closesocket(local_socket);
    WSACleanup();
#endif
  }
#endif // __STK_REALTIME
}

int Controller :: getType()
{
  return type;
}

MY_FLOAT Controller :: getByte2()
{
  return byte2;
}

MY_FLOAT Controller :: getByte3()
{
  return byte3;
}

MY_FLOAT Controller :: getChannel()
{
  return channel;
}

void Controller :: setDefaultTicks(long nSamples)
{
  default_ticks = nSamples;
}

int Controller :: getNextMessage()
{
#if defined(__STK_REALTIME_)
  static fd_set rmask;
  static struct timeval timeout = {0, 0};
  static int device = 0;
  static int nSockets = 0;
  static int fd_thread = 0;
  int checked = 0, i;
  char msg[256];
#endif

  // reset message type
  type = 0;

  if (!source) {
    // no realtime flags ... assuming scorefile input
    memset(message[msg_index], 0, MESSAGE_LENGTH);
    if ( fgets(message[msg_index], MESSAGE_LENGTH, stdin) == 0 )
      return -1;
    goto have_message;
  }

#if defined(__STK_REALTIME_)

  if (num_messages > 0)
    goto have_message;

  while (checked < nSockets+1) {

    if ( source & STK_SOCKET ) {

      rmask = mask;
      if (select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout)) {
        // there is a file descriptor set

        // check whether there's a new socket connection available
        if ( FD_ISSET(local_socket, &rmask) ) {
          // accept and service new connection
          int remote_socket=accept(local_socket, NULL, NULL);
          if (remote_socket < 0) {
            sprintf(msg, "Controller: Couldn't accept connection request!\n");
            throw StkError(msg, StkError::PROCESS_SOCKET);
          }
          if (nSockets != 0)
            // print message for connections subsequent to "piping" socket
            printf("New socket connection made.\n\n");

          // set the socket non-blocking
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
          fcntl(remote_socket, F_SETFL, O_NONBLOCK);
#elif defined(__OS_Win_)
          unsigned long non_block = 1;
          ioctlsocket(remote_socket, FIONBIO, &non_block);
#endif

          fd[nSockets] = remote_socket;
          if (nSockets == 0) fd_thread = remote_socket;
          nSockets++;
          FD_SET(remote_socket, &mask);
          if ( remote_socket > maxfd) maxfd = remote_socket;
          FD_CLR(local_socket, &rmask);
        }



        // check socket connections
        while (device < nSockets) {
          if (FD_ISSET(fd[device], &rmask)) {
            // this socket has data
            i = parseSocketData(fd[device]);
            if (i == 0) {
              // the socket connection closed
              nSockets--;
              if ( nSockets == 0 )
                return -1;
              if ( nSockets == 1 && FD_ISSET(fd_thread, &mask) ) {
                // the "piping" socket is still running
                if (source & STK_MIDI) {
                  printf("MIDI input still running ... type 'Exit<cr>' to quit.\n\n");
                  return 0;
                }
                else if (!(source & STK_PIPE) )
                  return -1;
              }
              if (device < nSockets) {
                // move descriptors down in the list
                for (int j=device; j<nSockets; j++)
                  fd[j] = fd[j+1];
              }
              device++;
              return 0;
            }

            device++;

            if ( !strncmp(message[msg_index], "Exit", 4) || !strncmp(message[msg_index], "exit", 4) ) {
              // we have an "Exit" message ... ignore it
              msg_index++;
              return 0;
            }
            // not an "Exit" message ... parse it
            goto have_message;
          }
          device++;
          if (++checked >= nSockets+1) break;
        }
      }
      else { // no file descriptors were set
        device += nSockets;
        checked += nSockets;
      }
    }

    // check MIDI 
    if (device >= nSockets) {
      //printf("got here, nSockets = %d, checked = %d, device = %d\n", nSockets, checked, device);
      device = 0;
      if (source & STK_MIDI) {
        if ( midi_input->nextMessage() > 0 ) {
          // get MIDI message info
          type = midi_input->getType();
          channel = midi_input->getChannel();
          byte2 = midi_input->getByteTwo();
          byte3 = midi_input->getByteThree();
          return default_ticks;
        }
      }
      if (++checked >= nSockets+1) break;
    }
  }
  // if we get here, we checked all devices but found no messages
  return default_ticks;

#endif // __STK_REALTIME

 have_message:

  //printf("%s", message[msg_index]);
  long ticks;
  score->parseThis(message[msg_index++]);
  num_messages--;
  if (msg_index >= MAX_MESSAGES) msg_index = 0;
  type = score->getType();
  if (type > 0) {
    MY_FLOAT temp = score->getDelta();
    if ( temp >= 0.0 )
      ticks = (long) (temp * SRATE);
    else
      // negative delta times specify realtime messages
      ticks = default_ticks;

    channel = score->getChannel();
    byte2 = score->getByteTwo();
    byte3 = score->getByteThree();
  }
  else {
    // Don't tick for comments or improperly formatted messages
    ticks = 0;
  }

  return ticks;
}

#if defined(__STK_REALTIME_)

int Controller :: parseSocketData(int fd)
{
  /*
    Parsing the socket data buffer is complicated by the fact that
    multiple and sometimes incomplete socket messages can be returned
    from a single recv() call, especially during high socket activity.
    This method will read all data available from a socket connection,
    filling the message buffer.  This is necessary because the select()
    function triggers on socket activity, not on the presence of
    (buffered) data.  So, whenever activity is indicated, we need to
    grab all available data.
  */

  static char socket_buffer[MESSAGE_LENGTH];
  int index = 0, m = 0, bufsize = 0;
  int fill_msg;

  fill_msg = (msg_index + num_messages) % MAX_MESSAGES;
  memset(message[fill_msg], 0, MESSAGE_LENGTH);

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  errno = 0;
  while (bufsize != -1 && errno != EAGAIN) {
#elif defined(__OS_Win_)
  while (bufsize != SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
#endif
    while (index < bufsize) {
      message[fill_msg][m++] = socket_buffer[index];
      if (socket_buffer[index++] == '\n') {
        m = 0;
        num_messages++;
        fill_msg = (msg_index + num_messages) % MAX_MESSAGES;
        memset(message[fill_msg], 0, MESSAGE_LENGTH);
      }
    }
    index = 0;

    // receive a new socket buffer
    memset(socket_buffer, 0, MESSAGE_LENGTH);
    bufsize = recv(fd, socket_buffer, MESSAGE_LENGTH, 0);
    if (bufsize == 0) {
      FD_CLR(fd, &mask);
#if defined(__OS_Win_)
      closesocket(fd);
#else
      close(fd);
#endif
      return 0;
    }
  }

  return 1;
}

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))

void *stdinHandler(void *)

#elif defined(__OS_Win_)

void stdinHandler(void *)

#endif
{
  char message[MESSAGE_LENGTH];
  char msg[256];
  int local_socket;
  struct sockaddr_in server_address;

#if defined(__OS_Win_)  // Windoze-only stuff
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    sprintf(msg, "Controller: Wrong Windoze socket library version!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }
#endif

  // Create the client-side socket
  local_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (local_socket < 0) {
    sprintf(msg, "Controller: Couldn't create socket!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  struct hostent *hostp;
  hostp = gethostbyname("localhost");

  // Fill in the address structure
  server_address.sin_family = AF_INET;
  memcpy((void *)&server_address.sin_addr, hostp->h_addr, hostp->h_length);
  server_address.sin_port = htons(STK_SOCKET_PORT);

  // Connect to the server
  if (connect(local_socket, (struct sockaddr *)&server_address,
              sizeof(server_address) ) < 0) {
#if defined(__OS_Win_)
      closesocket(local_socket);
      WSACleanup();
#else
      close(local_socket);
#endif
    sprintf(msg, "Controller: Couldn't connect stdin socket thread to server!\n");
    throw StkError(msg, StkError::PROCESS_SOCKET);
  }

  for (;;) {
    memset(message, 0, MESSAGE_LENGTH);
    if ( fgets(message, MESSAGE_LENGTH, stdin) == 0 )
      break;
    // check for an "Exit" message
    if ( !strncmp(message, "Exit", 4) || !strncmp(message, "exit", 4) )
      break;

    if (send(local_socket, (const char *)message, strlen(message), 0) < 0) {
#if defined(__OS_Win_)
      closesocket(local_socket);
      WSACleanup();
#else
      close(local_socket);
#endif
      sprintf(msg, "Controller: connection to socket server failed!\n");
      throw StkError(msg, StkError::PROCESS_SOCKET);
    }
  }

#if defined(__OS_Win_)
  closesocket(local_socket);
  _endthread();
#else
  close(local_socket);
  return NULL;
#endif
}

#endif // __STK_REALTIME
