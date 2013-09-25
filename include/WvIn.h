/********************************************/
/*
  Audio Data Input Base Class
  by Gary P. Scavone, 1999-2000

  This class can handle multi-channel
  input.  Multi-channel input is
  interleaved in the vector "data".
  Actual data input occurs in the
  subclasses of WvIn.

  Currently, STK is only supporting a few data
  types (16-bit integer .snd, .wav, .raw, and
  .aif files and 64-bit double MAT-files).  In
  order to support more formats AND to make the
  writing of subclasses easier, a format ENUM
  could be defined and a generalized getData()
  function written within this WvIn class.  Then,
  most subclasses of WvIn would only have to
  setup the appropriate parameters and all
  other processing would happen here.
*/
/********************************************/

#if !defined(__WvIn_h)
#define __WvIn_h

/* "Oneshot" files larger than MAX_FILE_LOAD_SIZE will be
   copied into memory in RT_BUFFER_SIZE chunks, rather than
   completely loaded into a buffer at instantiation.
*/
#define MAX_FILE_LOAD_SIZE 5000000

// Buffer size, in sample frames, when incrementally loading from disk
#define LOAD_BUFFER_SIZE 1024

#include "Object.h"
#include "StkError.h"

class WvIn : public Object
{
protected:  
  long fileSize;
  long bufferSize;
  long readPointer;
  long dataOffset;
  int channels;
  int looping;
  int finished;
  int chunking;
  int interpolate;
  MY_FLOAT *data;
  MY_FLOAT time;
  MY_FLOAT rate;
  MY_FLOAT phaseOffset;
  MY_FLOAT *lastOutput;
  FILE *fd;
  virtual void getData(long index);
public:
  WvIn();
  virtual ~WvIn();
  void reset();
  void normalize();
  void normalize(MY_FLOAT newPeak);
  virtual void setRate(MY_FLOAT aRate);
  void setFreq(MY_FLOAT aFreq);
  virtual void addTime(MY_FLOAT aTime);
  void addPhase(MY_FLOAT anAngle);
  void addPhaseOffset(MY_FLOAT anAngle);
  void setInterpolate(int anInterpStatus);
  virtual void setLooping(int aLoopStatus);
  long getSize();
  int isFinished();
  virtual int informTick();
  MY_FLOAT tick();
  MY_MULTI mtick();
  MY_FLOAT lastOut();
  MY_MULTI mlastOut();
};

#endif // defined(__WvIn_h)
