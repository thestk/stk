/******************************************/
/*
  RtAudio.cpp
  Realtime Sound I/O Object for STK
  by Gary P. Scavone, 1998-2000.

  The sound output sections of this object
  were originally based in part on code by
  Doug Scott (SGI), Tim Stilson (Linux),
  Bill Putnam (Win Wav), and R. Marsanyi
  (DirectSound).  The latest DirectSound
  code was re-written by Dave Chisholm at
  CCRMA.

  This object provides a standard API
  across all platforms for STK realtime
  audio input/output.  Multi-channel
  support is supported when provided by
  the soundcard.

  Only 16-bit integer input/output
  routines are written for the moment
  though it would be simple to overload
  the methods for other data types.
*/
/******************************************/

#if !defined(__RtAudio_h)
#define __RtAudio_h

#include "Object.h"
#include "StkError.h"

#if defined(__OS_IRIX_)
  #include <dmedia/audio.h>
  #include <unistd.h>
  #include <errno.h>

#elif defined(__ALSA_API_)
  #include <sys/ioctl.h>
  #include <sys/asoundlib.h>

#elif defined(__OSS_API_)
  #include <sys/ioctl.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/soundcard.h>
  #include <errno.h>

#elif defined(__OS_Win_)
  #include <windows.h>
  #include <dsound.h>
  #include <winsock.h>
	#include "mmsystem.h"

  // this is how often we check for new audio input (milliseconds)
	#define TIMER_PERIOD 20
  // the resolution which we tell windows we are willing to tolerate (milliseconds)
	#define TIMER_RESOLUTION 5
  // in seconds, doesn't have a real effect on latency
	#define DS_CAPTURE_BUFFER_SIZE 2
  // this controls the inherent latency of the output ... more fragments produce
  // a more stable, though slower, response
	#define NUM_FRAGMENTS 10
	#define MAX_DEVICES 10

	typedef struct DeviceInfo {
		LPGUID guid;
		char* description;
		char* moduleName;
	} DeviceInfo;

#endif

class RtAudio : public Object
{
protected:
#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))
  int stk_chans;
	ALport audio_port_in;
	ALport audio_port_out;

#elif (defined(__STK_REALTIME_) && defined(__OSS_API_))
	int audio_fd;

#elif (defined(__STK_REALTIME_) && defined(__ALSA_API_))
	snd_pcm_t *ohandle;
	snd_pcm_t *ihandle;
  int stk_chans;   // the number of channels we want to use
  int dev_ichans;  // the number of input channels the device needs
  int dev_ochans;  // the number of output channels the device needs
  int ifragsize;
  int ofragsize;
  int bytes_per_sample;
  unsigned int direction;
  unsigned char *outbuf;
  unsigned char *inbuf;

#elif (defined(__STK_REALTIME_) && defined(__OS_Win_) )

  DeviceInfo devices[MAX_DEVICES];
  int numDevices;
  char errormsg[256];
  long inputBufferSize;
  BYTE *inputBuffer;
  UINT nextRecordRead, nextRecordWrite;
  MY_FLOAT sampleRate;
  
  //these are the variable relating to direct sound output
  LPDIRECTSOUND	directSoundObject;
  LPDIRECTSOUNDBUFFER directSoundBuffer;
  DWORD directSoundBufferSize;
  UINT nextWritePos;

  //direct sound input
  LPDIRECTSOUNDCAPTURE directSoundCaptureObject;
  LPDIRECTSOUNDCAPTUREBUFFER directSoundCaptureBuffer;
  DWORD directSoundCaptureBufferSize;

  // our periodic function will set this flag if something goes wrong
  bool internalError;
  bool playing, recording;
  UINT timerID;

  static void CALLBACK PeriodicCallbackFn(UINT uID, UINT uMsg, DWORD dwUser,
                                          DWORD dw1, DWORD dw2);
  static bool CALLBACK SoundDeviceEnumCallback(LPGUID lpguid,
                                               LPCSTR lpcstrDescription,
                                               LPCSTR lpcstrModule,
                                               LPVOID lpContext);

  void addDevice(LPGUID guid, char* description, char* moduleName);
  void getInputSamples();
  static char* getErrorMessage(int code);

#endif

public:
	RtAudio(int channels, MY_FLOAT srate, const char *mode, int device = -1);
  ~RtAudio();
  int playBuffer(INT16 *buf, int bufsize);
  int recordBuffer(INT16 *buf, int bufsize);

#if (defined(__STK_REALTIME_) && defined(__OS_Win_) )
  // Sets the pointer to its own internal buffer, returning
  // amount of data available ... slightly more efficient.
  int recordBuffer(INT16**);
  void stopPlay();
  void startPlay();
  void stopRecord();
  void startRecord();
#endif
};

#endif
