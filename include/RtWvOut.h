/*******************************************/
/*  Real-Time Audio Output Class,          */
/*  by Perry R. Cook, 1996                 */
/*  Revised by Gary P. Scavone, 2000       */
/*                                         */
/*  This object opens a realtime soundout  */
/*  device, and pokes buffers of samples   */
/*  into it.                               */
/*******************************************/

#if !defined(__RtWvOut_h)
#define __RtWvOut_h

#include "Object.h"
#include "WvOut.h"
#include "RtAudio.h"

class RtWvOut : public WvOut
{
 protected:
	RtAudio *sound_dev;
 public:
  RtWvOut(int chans = 1, int device = -1);
  ~RtWvOut();
  void tick(MY_FLOAT sample);
  void mtick(MY_MULTI samples);

#if (defined(__OS_Win_) )
  void stopPlay();
  void startPlay();
  void stopRecord();
  void startRecord();
#endif
};

#endif // defined(__RtWvOut_h)
