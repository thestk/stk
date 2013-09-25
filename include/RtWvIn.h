/*******************************************/
/*  RtWvIn Input Class,                    */
/*  by Gary P. Scavone, 1999-2000          */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to read in realtime 16-bit data   */
/*  from a computer's audio port.          */
/*                                         */
/*  NOTE: This object is NOT intended for  */
/*  use in achieving simultaneous realtime */
/*  audio input/output (together with      */
/*  RtWvOut). Under certain circumstances  */
/*  such a scheme is possible, though you  */
/*  should definitely know what you are    */
/*  doing before trying.  For safer "full- */
/*  duplex" operation, use the RtDuplex    */
/*  class.                                 */
/*******************************************/

#if !defined(__RtWvIn_h)
#define __RtWvIn_h

#include "Object.h"
#include "RtAudio.h"
#include "WvIn.h"

class RtWvIn : public WvIn
{
protected:
	RtAudio *sound_dev;
  INT16 *rtdata;
  INT16 *lastSamples;
  MY_FLOAT gain;
  void getData(long index);
public:
  RtWvIn(int chans = 1, MY_FLOAT srate = SRATE, int device = -1);
  ~RtWvIn();
  void setRate(MY_FLOAT aRate);
  void addTime(MY_FLOAT aTime);
  void setLooping(int aLoopStatus);
  long getSize();
  int informTick();
};

#endif
