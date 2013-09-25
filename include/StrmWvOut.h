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
  as constructor arguments.  Actual data writing
  and buffering takes place in a thread.
*/
/******************************************/

#if !defined(__StrmWvOut_h)
#define __StrmWvOut_h

#include "Object.h"
#include "WvOut.h"

class StrmWvOut : public WvOut
{
 protected:
  int local_socket;
 public:
  StrmWvOut(int port, const char *hostname = "localhost", int chans = 1);
  ~StrmWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__StrmWvOut_h)
