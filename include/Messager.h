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

#if !defined(__MESSAGER_H)
#define __MESSSAGER_H

#include "Stk.h"
#include "SKINI.h"

#define MESSAGE_LENGTH  128
#define MAX_MESSAGES 25

#if defined(__STK_REALTIME__)

#include "Thread.h"
#include "Socket.h"
#include "RtMidi.h"

#define STK_MIDI        0x0001
#define STK_PIPE        0x0002
#define STK_SOCKET      0x0004

extern "C" THREAD_RETURN THREAD_TYPE stdinHandler(void * ptr);

#if (defined(__OS_IRIX__) || defined(__OS_LINUX__))
  #include <sys/types.h>
  #include <sys/time.h>
#endif

#endif // __STK_REALTIME__

class Messager : public Stk
{
 public:
  //! Constructor performs initialization based on an input mask.
  /*!
    The default constructor is set to read input from a SKINI
    scorefile.  The flags STK_MIDI, STK_PIPE, and STK_SOCKET can be
    OR'ed together in any combination for multiple "realtime" input
    source parsing.  For realtime input types, an StkError can be
    thrown during instantiation.
  */
  Messager(int inputMask = 0);

  //! Class destructor.
  ~Messager();

  //! Check for a new input message and return the message type.
  /*!
     Return type values greater than zero represent valid messages.
     If an input scorefile has been completely read or all realtime
     input sources have closed, a negative value is returned.  If the
     return type is zero, no valid messages are present.
  */
  long nextMessage(void);

  //! Set the delta time (in samples) returned between valid realtime messages.  This setting has no affect for scorefile messages.
  void setRtDelta(long nSamples);

  //! Return the current message "delta time" in samples.
  long getDelta(void) const;

  //! Return the current message type.
  long getType() const;

  //! Return the byte two value for the current message.
  MY_FLOAT getByteTwo() const;

  //! Return the byte three value for the current message.
  MY_FLOAT getByteThree() const;

  //! Return the channel number for the current message.
  long getChannel() const;

 protected:

  SKINI *skini;
  long type;
  long channel;
  MY_FLOAT byte2;
  MY_FLOAT byte3;
  int sources;
  long delta;
  long rtDelta;
  char message[MAX_MESSAGES][MESSAGE_LENGTH];
  unsigned int messageIndex;
  int nMessages;

#if defined(__STK_REALTIME__)

  // Check MIDI source for new messages.
  bool midiMessage(void);

  // Check socket sources for new messages.
  bool socketMessage(void);

  // Receive and parse socket data.
  bool readSocket(int fd);

  RtMidi *midi;
  Thread *thread;
  Socket *soket;

  unsigned int nSockets;  
  fd_set mask;
  int maxfd;
  int pipefd;
  int fd[16];
  char error[256];

#endif // __STK_REALTIME__

};

#endif // defined(__MESSAGER_H)
