/******************************************/
/*
  StrmWvIn Audio Input Class,
  by Gary P. Scavone, 2000

  This object inherits from WvIn and is used
  to accept 16-bit data (signed integer) via
  a socket connection (streamed audio).
  Streamed data must be in big-endian format,
  which conforms to network byte ordering.

  This class starts a socket server, which
  waits for a remote connection.  Actual data
  reading and buffering takes place in a thread.
*/
/******************************************/

#if !defined(__StrmWvIn_h)
#define __StrmWvIn_h

#include "Object.h"
#include "WvIn.h"

class StrmWvIn : public WvIn
{
protected:
  INT16 *strmdata;
  INT16 *lastSamples;
  void getData(long index);
public:
  StrmWvIn(int chans = 1, MY_FLOAT srate = SRATE);
  ~StrmWvIn();
  void setRate(MY_FLOAT aRate);
  void addTime(MY_FLOAT aTime);
  void setLooping(int aLoopStatus);
  long getSize();
  int informTick();
};

#endif
