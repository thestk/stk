/******************************************/
/*  RTSoundIO.cpp                         */
/*  Realtime Sound I/O Object for STK,    */
/*  by Gary P. Scavone, 1998-1999.        */
/*                                        */
/*  The sound output sections of this     */
/*  object were based in part on code     */
/*  by Doug Scott (SGI), Tim Stilson      */
/*  (Linux), Bill Putnam (Win Wav), and   */
/*  R. Marsanyi (DirectSound).            */
/*                                        */
/*  This object provides a standard API   */
/*  across all platforms for STK realtime */
/*  audio input/output.  The sound output */
/*  code is fairly robust.  Audio input,  */
/*  however, is more dependent on the     */
/*  capabilities of the particular OS and */
/*  the soundcard being used.  For the    */
/*  moment, I'll try to provide 1 or 2    */
/*  channel support.                      */
/*                                        */
/*  16-bit integer audio input/output     */
/*  data is being assumed.                */
/******************************************/

#if !defined(__RTSOUNDIO_h)
#define __RTSOUNDIO_h

#include "Object.h"

#if defined(__OS_IRIX_)
  #include <dmedia/audio.h>
#elif defined(__WINDS_API_)
  #include <windows.h>
  #include <dsound.h>
#elif defined(__WINMM_API_)
  #include <windows.h>
  #include <MMSystem.h>
  #define NUM_OUT_BUFFERS 6
#endif

class RTSoundIO : public Object
{
 protected:
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
	ALport audio_port_in;
	ALport audio_port_out;
#elif (defined(__STK_REALTIME_) && defined(__OSS_API_))
	int audio_fd;
#elif (defined(__STK_REALTIME_) && defined(__WINDS_API_) )
	LPDIRECTSOUND       lpDirectSound;
	LPDIRECTSOUNDBUFFER lpDSBuffer;
	DWORD               dwDSBufSize;
  DWORD               zoneSize;
  LPDIRECTSOUNDCAPTURE lpDSCapture;
  LPDIRECTSOUNDCAPTUREBUFFER lpDSCBuffer;
	DWORD               dwDSCBufSize;
#elif (defined(__STK_REALTIME_) && defined(__WINMM_API_) )
	HWAVEOUT  audioPort;
	WAVEHDR   whOut[NUM_OUT_BUFFERS];
	UINT      outBufNum;
	DWORD	  lastWriteTime;
#endif
 public:
	RTSoundIO(MY_FLOAT srate, int channels, char *mode);
  ~RTSoundIO();
  int playBuffer(short *buf, int bufsize);
  int recordBuffer(short *buf, int bufsize);
};

#endif
