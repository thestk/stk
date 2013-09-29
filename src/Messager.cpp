/***************************************************/
/*! \class Messager
    \brief STK input control message parser.

    This class reads and parses control messages
    from a variety of sources, such as a MIDI
    port, scorefile, socket connection, or pipe.
    MIDI messages are retrieved using the RtMidi
    class.  All other input sources (scorefile,
    socket, or pipe) are assumed to provide SKINI
    formatted messages.

    For each call to nextMessage(), the active
    input sources are queried to see if a new
    control message is available.

    This class is primarily for use in STK main()
    event loops.

    One of the original goals in creating this
    class was to simplify the message acquisition
    process by removing all threads.  If the
    windoze select() function behaved just like
    the unix one, that would have been possible.
    Since it does not (it can't be used to poll
    STDIN), I am using a thread to acquire
    messages from STDIN, which sends these
    messages via a socket connection to the
    message socket server.  Perhaps in the future,
    it will be possible to simplify things.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Messager.h"
#include <string.h>
#include <iostream.h>

int socket_port = 2001;

Messager :: Messager(int inputMask, int port)
{
  sources = inputMask;
  rtDelta = RT_BUFFER_SIZE;
  messageIndex = 0;
  nMessages = 0;
  skini = new SKINI();

#if defined(__STK_REALTIME__)
  // If no input source is specified, we assume the input is coming
  // from a SKINI scorefile.  If any input source is specified, we
  // will always check STDIN, even if STK_PIPE is not specified.  This
  // provides a means to exit cleanly when reading MIDI or in case a
  // socket connection cannot be made after STK_SOCKET has been
  // specified.  The current means of polling STDIN is via a thread,
  // which sends its data via a socket connection to the socket
  // server.
  if ( sources ) {

    if ( sources & STK_MIDI ) {
      // Attempt to open a MIDI device, but don't throw an exception
      // if other input sources are specified.
      try {
        midi = new RtMidi();
      }
      catch (StkError &exception) {
        if ( sources == STK_MIDI ) {
          throw exception;
        }
        // Disable the MIDI input and keep going.
        sources &= ~STK_MIDI;
      }
    }
    
    // If STK_PIPE is not specified, let the users know they can exit
    // the program via the console if necessary.
    if ( !(sources & STK_PIPE) && sources )
      cout << "\nType `Exit<cr>' to quit.\n" << endl;

    sources |= STK_SOCKET;
    socket_port = port;
    soket = new Socket(port);
    if (inputMask & STK_SOCKET)
      printf("\nSocket server listening for connection(s) on port %d ...\n\n", port);

    nSockets = 0;
    maxfd = 0;
    FD_ZERO(&mask);    
    int d = soket->socket();
    FD_SET(d, &mask);
    if (d > maxfd) maxfd = d;

    // The fd array is used to hold the file descriptors for all
    // connected sockets.  This saves some time incrementing through
    // file descriptors when using select().
    for (int i=0; i<16; i++)
      fd[i] = 0;

    // Start the stdin input thread.
    thread = new Thread();
    if ( !thread->start( (THREAD_FUNCTION)&stdinHandler, NULL ) ) {
      sprintf(error, "Messager: Unable to start stdin input thread!");
      handleError( error, StkError::PROCESS_THREAD );
    }
  }
#endif // __STK_REALTIME__
}

Messager :: ~Messager()
{
  delete skini;

#if defined(__STK_REALTIME__)

  if ( sources & STK_MIDI )
    delete midi;

  if ( sources & STK_SOCKET ) {
    delete soket;
    delete thread;
  }
#endif // __STK_REALTIME__
}

long Messager :: getType() const
{
  return type;
}

MY_FLOAT Messager :: getByteTwo() const
{
  return byte2;
}

MY_FLOAT Messager :: getByteThree() const
{
  return byte3;
}

long Messager :: getChannel() const
{
  return channel;
}

void Messager :: setRtDelta(long nSamples)
{
  if ( nSamples > 0 )
    rtDelta = nSamples;
  else
    cerr << "Messager: setRtDelta(" << nSamples << ") less than or equal to zero!" << endl;
}

long Messager :: getDelta() const
{
  return delta;
}

long Messager :: nextMessage()
{
  if (nMessages > 0 ) nMessages--;
  type = 0;

  if ( !sources ) {
    // No realtime flags ... assuming scorefile input.
    memset(message[messageIndex], 0, MESSAGE_LENGTH);
    if ( fgets(message[messageIndex], MESSAGE_LENGTH, stdin) == 0 ) {
      delta = 0;
      return -1; // end of file
    }
    nMessages++;
  }
#if defined(__STK_REALTIME__)
  else if (nMessages == 0) {
    if ( midiMessage() ) return type;
    if ( !socketMessage() ) return type;
  }
#endif

  skini->parseThis(message[messageIndex++]);
  if (messageIndex >= MAX_MESSAGES) messageIndex = 0;
  type = skini->getType();
  if (type <= 0) {
    // Don't tick for comments or improperly formatted messages.
    nMessages--;
    delta = 0;
    type = 0;
    return type;
  }

  channel = skini->getChannel();
  byte2 = skini->getByteTwo();
  byte3 = skini->getByteThree();

  MY_FLOAT temp = skini->getDelta();
  if ( temp >= 0.0 )
    delta = (long) (temp * Stk::sampleRate());
  else
    // Ignore negative delta times (absolute time).
    delta = rtDelta;

  return type;
}

#if defined(__STK_REALTIME__)
bool Messager :: midiMessage( void )
{
  if (sources & STK_MIDI) {
    if ( midi->nextMessage() > 0 ) {
      // get MIDI message info
      type = midi->getType();
      channel = midi->getChannel();
      byte2 = midi->getByteTwo();
      byte3 = midi->getByteThree();
      nMessages++;
      delta = rtDelta;
      return true;
    }
  }
  return false;
}

bool Messager :: socketMessage()
{
  register fd_set rmask;
  static struct timeval timeout = {0, 0};

  rmask = mask;
  if ( select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout) ) {
    // A file descriptor is set.

    // Check if there's a new socket connection available.
    if ( FD_ISSET(soket->socket(), &rmask) ) {
      // Accept and service new connection.
      int newfd = soket->accept();
      if ( newfd < 0 ) {
        sprintf(error, "Messager: Couldn't accept connection request!");
        handleError(error, StkError::WARNING);
      }

      // We assume the first connection will occur for the stdin
      // thread socket.  Since this connection is "hidden" from
      // the user, only print connected messages for subsequent
      // connections.
      if (nSockets == 0)
        pipefd = newfd;
      else
        cout << "New socket connection made.\n" << endl;

      // Set the socket to non-blocking mode.
      Socket::setBlocking( newfd, false );

      // Save the descriptor and update the masks.
      fd[nSockets++] = newfd;
      FD_SET(newfd, &mask);
      if ( newfd > maxfd) maxfd = newfd;
      FD_CLR(soket->socket(), &rmask);
    }

    // Check client socket connections.
    unsigned int client = 0;
    while ( client < nSockets ) {
      if ( !FD_ISSET(fd[client], &rmask) )
        client++;
      else {
        // This connection has data.
        if ( !readSocket( fd[client] ) ) {
          // The socket connection closed.
          nSockets--;
          if ( nSockets == 0 ) {
            type = -1;
            return false;
          }
          if ( nSockets == 1 && FD_ISSET(pipefd, &mask) ) {
            // The "piping" socket is still running.
            if (sources & STK_MIDI) {
              cout << "MIDI input still running ... type 'Exit<cr>' to quit.\n" << endl;
            }
            else if (!(sources & STK_PIPE) ) {
              // The user didn't specify this connection, so quit now.
              type = -1;
              return false;
            }
          }
          if (client < nSockets) {
            // Move descriptors down in the list.
            for (unsigned int j=client; j<nSockets; j++)
              fd[j] = fd[j+1];
          }
          delta = 0;
          return false;
        }

        if ( !strncmp(message[messageIndex], "Exit", 4) || !strncmp(message[messageIndex], "exit", 4) ) {
          // We have an "Exit" message ... don't try to parse it.
          messageIndex++;
          nMessages--;
          delta = 0;
          return false;
        }
        // Not an "Exit" message ... parse it.
        return true;
      }
    }
  }

  // If we get here, we checked all devices but found no messages.
  delta = rtDelta;
  return false;
}

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__))

  #include <errno.h> 

#endif

bool Messager :: readSocket(int fd)
{
  // This method will read all data available from a socket
  // connection, filling the message buffer.  This is necessary
  // because the select() function triggers on socket activity, not on
  // the presence of (buffered) data.  So, whenever activity is
  // indicated, we need to grab all available data.
  char buffer[MESSAGE_LENGTH];
  int index = 0, m = 0, bufferSize = 0;
  int nextMessage;

  nextMessage = (messageIndex + nMessages) % MAX_MESSAGES;
  memset(message[nextMessage], 0, MESSAGE_LENGTH);

#if ( defined(__OS_IRIX__) || defined(__OS_LINUX__) || defined(__OS_MACOSX__) )
  errno = 0;
  while (bufferSize != -1 && errno != EAGAIN) {
#elif defined(__OS_WINDOWS__)
  while (bufferSize != SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK) {
#endif
    while (index < bufferSize) {
      message[nextMessage][m++] = buffer[index];
      if (buffer[index++] == '\n') {
        m = 0;
        nMessages++;
        nextMessage = (messageIndex + nMessages) % MAX_MESSAGES;
        memset(message[nextMessage], 0, MESSAGE_LENGTH);
      }
    }
    index = 0;

    // Receive a new socket buffer.
    memset(buffer, 0, MESSAGE_LENGTH);
    bufferSize = Socket::readBuffer(fd, buffer, MESSAGE_LENGTH, 0);
    if (bufferSize == 0) {
      FD_CLR(fd, &mask);
      Socket::close( fd );
      return false;
    }
  }

  return true;
}

THREAD_RETURN THREAD_TYPE stdinHandler(void *)
{
  char message[MESSAGE_LENGTH];

  Socket *s;
  try {
    s = new Socket( socket_port, "localhost" );
  }
  catch (StkError &) {
    fprintf(stderr, "Messager: Couldn't create stdin input thread!\n");
    return NULL;
  }

  for (;;) {
    memset(message, 0, MESSAGE_LENGTH);
    if ( fgets(message, MESSAGE_LENGTH, stdin) == 0 )
      break;

    // Check for an "Exit" message.
    if ( !strncmp(message, "Exit", 4) || !strncmp(message, "exit", 4) )
      break;

    if ( s->writeBuffer( (void *)message, strlen(message), 0) < 0 ) {
      fprintf(stderr, "Messager: stdin thread connection to socket server failed!\n");
      break;
    }
  }

  delete s;
  return NULL;
}

#endif // __STK_REALTIME__
