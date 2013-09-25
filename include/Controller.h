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

#if !defined(__Controller_h)
#define __Controller_h

#include "Object.h"
#include "SKINI11.h"

#if defined(__STK_REALTIME_)
#include "RtMidi.h"
#include "StkError.h"

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

void *stdinHandler(void *);

#elif defined(__OS_Win_)
#include <process.h>
#include <winsock.h>

void stdinHandler(void *);

#endif

#endif // __STK_REALTIME

#define STK_MIDI        0x0001
#define STK_PIPE        0x0002
#define STK_SOCKET      0x0004
#define STK_SCOREFILE   0x0008

#define MESSAGE_LENGTH  128
#define MAX_MESSAGES 25
#define STK_SOCKET_PORT 2001

class Controller : public Object
{
 protected:
  int source;
  long default_ticks;
  int type;
  MY_FLOAT channel;
  MY_FLOAT byte2;
  MY_FLOAT byte3;
  char message[MAX_MESSAGES][MESSAGE_LENGTH];
  int msg_index;
  int num_messages;
  SKINI11 *score;

#if defined(__STK_REALTIME_)
  fd_set mask;
  int maxfd;
  int fd[16];
  int local_socket;
  RtMidi *midi_input;
  int parseSocketData(int fd);
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  pthread_t stdin_thread;
#elif defined(__OS_Win_)
  unsigned long stdin_thread;
#endif
#endif // __STK_REALTIME

 public:
  Controller(int inputMask);
  ~Controller();
  void setDefaultTicks(long nSamples);
  int getNextMessage();
  int getType();
  MY_FLOAT getByte2();
  MY_FLOAT getByte3();
  MY_FLOAT getChannel();
};

#endif // defined(__Controller_h)
