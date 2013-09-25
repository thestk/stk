/*******************************************/
/*  Real-Time Duplex Input/Output Class,   */
/*  by Gary P. Scavone, 1999               */
/*                                         */
/*  This object opens the sound i/o        */
/*  device, reads buffers in from it, and  */
/*  pokes buffers of samples out to it.    */
/*                                         */
/*  At the moment, duplex mode is possible */
/*  only on Linux (OSS), IRIX, and         */
/*  Windows95/98 platforms.                */
/*******************************************/

#if !defined(__RtDuplex_h)
#define __RtDuplex_h

#include "Object.h"
#include "RtAudio.h"

class RtDuplex : public Object
{
 protected:
	RtAudio *sound_dev;
  INT16 *indata;
  INT16 *outdata;
  long data_length;
  long readCounter;
  long writeCounter;
  int channels;
  MY_FLOAT gain;
  MY_FLOAT *insamples;
 public:
  RtDuplex(int chans = 1, MY_FLOAT srate = SRATE, int device = -1);
  ~RtDuplex();
  MY_FLOAT tick(MY_FLOAT outsample);
  MY_MULTI mtick(MY_MULTI outsamples);
};

#endif // defined(__RtDuplex_h)
