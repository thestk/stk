/*******************************************/
/*  Real-Time Wave File Output Class,      */
/*  by Perry R. Cook, 1996                 */
/*  revised by Gary P. Scavone, 1999       */
/*                                         */
/*  This object opens a realtime soundout  */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/

#include "Object.h"
#include "WvOut.h"
#include "RTSoundIO.h"

#if !defined(__RTWvOut_h)
#define __RTWvOut_h

class RTWvOut : public WvOut
{
 protected:
	RTSoundIO *soundIO;
  short data[RT_BUFFER_SIZE];
  long counter;
  int channels;
 public:
  RTWvOut(MY_FLOAT srate, int chans);
  ~RTWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);
};

#endif // defined(__RTWvOut_h)
