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

#if !defined(__RTDuplex_h)
#define __RTDuplex_h

#include "Object.h"
#include "RTSoundIO.h"

class RTDuplex : public Object
{
 protected:
	RTSoundIO *soundIO;
  short indata[RT_BUFFER_SIZE];
  short outdata[RT_BUFFER_SIZE];
  long readCounter;
  long writeCounter;
  int channels;
  MY_FLOAT gain;
  MY_FLOAT *insamples;
 public:
  RTDuplex(MY_FLOAT srate, int chans);
  ~RTDuplex();
  MY_FLOAT tick(MY_FLOAT outsample);
  MY_MULTI mtick(MY_MULTI outsamples);
};

#endif // defined(__RTDuplex_h)
