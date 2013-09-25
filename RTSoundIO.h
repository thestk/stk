/******************************************/
/*  RTSoundIO.h                           */
/*  Realtime Sound I/O Object for STK,    */
/*  by Gary P. Scavone, 1998.             */
/*  Based in part on code by Doug         */
/*  Scott (SGI), Tim Stilson (Linux),     */
/*  Bill Putnam (Win Wav), and            */
/*  R. Marsanyi (DirectSound).            */
/*                                        */
/*  At the moment, this object only       */
/*  handles realtime sound output, though */
/*  input code can go here when someone   */
/*  decides they need it (and writes it). */
/*                                        */
/*  This object provides a standard API   */
/*  across all platforms for STK realtime */
/*  sound output.  At the moment, this    */
/*  object is only used by RTWvOut.       */
/******************************************/

#if !defined(__RTSOUNDIO_h)
#define __RTSOUNDIO_h

#include "Object.h"

#if defined(__SGI_REALTIME_)
  #include <audio.h>
#elif defined(__WINDS_REALTIME_)
  #include	<windows.h>
  #include "include/dsound.h"
#elif defined(__WINMM_REALTIME_)
  #include	<windows.h>
  #include	<MMSystem.h>
  #define NUM_OUT_BUFFERS 6
#endif

class RTSoundIO : public Object
{
 protected:
#if defined(__SGI_REALTIME_)
	ALport audio_port;
#elif defined(__USS_REALTIME_)
	int audio_fd;
#elif defined(__WINDS_REALTIME_)
	LPDIRECTSOUND       m_pDirectSound;
	LPDIRECTSOUNDBUFFER m_pDSBuffer;
	UINT                m_cbBufOffset;
	UINT                m_cbBufSize;
#elif defined(__WINMM_REALTIME_)
	HWAVEOUT  audioPort;
	WAVEHDR   whOut[NUM_OUT_BUFFERS];
	UINT      outBufNum;
	DWORD	  lastWriteTime;
#endif
 public:
	RTSoundIO(MY_FLOAT srate, int channels);
    ~RTSoundIO();
    int playBuffer(short *buf, int bufsize);
};

#endif
