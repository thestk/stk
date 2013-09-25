/*******************************************/
/*  RTWvIn Input Class,                    */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object inherits from WvIn and is  */
/*  used to read in realtime 16-bit data   */
/*  from a computer's audio port.          */
/*                                         */
/*  NOTE: This object is NOT intended for  */
/*  use in achieving simultaneous realtime */
/*  audio input/output (together with      */
/*  RTWvOut). Under certain circumstances  */
/*  such a scheme is possible, though you  */
/*  should definitely know what you are    */
/*  doing before trying.  For safer "full- */
/*  duplex" operation, use the RTDuplex    */
/*  class.                                 */
/*******************************************/

#if !defined(__RTWvIn_h)
#define __RTWvIn_h

#include "Object.h"
#include "RTSoundIO.h"
#include "WvIn.h"

class RTWvIn : public WvIn
{
 protected:
	RTSoundIO *soundIO;
  short *rtdata;
  MY_FLOAT gain;
 public:
  RTWvIn(MY_FLOAT srate, short chans);
  ~RTWvIn();
  void normalize();
  void normalize(MY_FLOAT newPeak);
  void addPhaseOffset(MY_FLOAT anAngle);
  void setLooping(int aLoopStatus);
  void getMoreData();
  int informTick();
};

#endif
