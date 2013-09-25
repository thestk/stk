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
/*  moment, I'll try to provide 1 and 2   */
/*  channel support.                      */
/*                                        */
/*  16-bit integer audio input/output     */
/*  data is being assumed.                */
/******************************************/

#include "RTSoundIO.h"

#if (defined(__STK_REALTIME_) && defined(__OS_IRIX_))

#include <stdio.h>
#include <unistd.h>
#include <errno.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels, char *mode)
{
  ALconfig audio_port_config;
  int lookaheadbuffers = 8;  // number of lookahead buffers
  long pvbuf[4];
  int nbuf, totalBufSize;

  /* Check the number of channels */
  if (channels > 2) {
    fprintf(stderr,"RTSoundIO: Unsupported # of audio i/o channels: %d\n", channels);
    exit(0);
  }

  /* Create ALconfig structure */
  audio_port_config = ALnewconfig();
  if (!audio_port_config) {
    fprintf(stderr,"Couldn't create ALconfig:%s\n", alGetErrorString(oserror()));
    exit(0);
  }

  /* Configure channels */
  if(ALsetchannels(audio_port_config, channels) < 0) {
	 fprintf(stderr,"Cannot configure channels: %s\n", alGetErrorString(oserror()));
	 exit(0);
  }

  /* Size the output queue */
  nbuf = (channels == 2) ? lookaheadbuffers : lookaheadbuffers/2;
  totalBufSize = RT_BUFFER_SIZE * nbuf;
  if(ALsetqueuesize(audio_port_config, totalBufSize) < 0) {
	 fprintf(stderr,"Cannot configure output queue size: %s\n", alGetErrorString(oserror()));
	 exit(0);
  }

  if (!strcmp(mode,"play")) { // playback only

    /* Open the output audio port */
    audio_port_out = ALopenport("STK output port", "w", audio_port_config);
    if(!audio_port_out) {
      fprintf(stderr,"Cannot initialize output audio port: %s\n", alGetErrorString(oserror()));
      exit(0);
    }

    /* Set sample rate parameters */
    pvbuf[0] = AL_OUTPUT_RATE;
    pvbuf[1] = (long) srate;
    ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2); /* set output SR */

    /* Tell port to accept refill at buffers - 1 */
    ALsetfillpoint(audio_port_out,RT_BUFFER_SIZE * (lookaheadbuffers - 1));
    audio_port_in = 0;
  }
  else if (!strcmp(mode,"record")) { // record only

    /* Open the input audio port */
    audio_port_in = ALopenport("STK input port", "r", audio_port_config);
    if(!audio_port_in) {
      fprintf(stderr,"Cannot initialize input audio port: %s\n", alGetErrorString(oserror()));
      exit(0);
    }

    /* Set sample rate parameters */
    pvbuf[0] = AL_OUTPUT_RATE;
    pvbuf[1] = (long) srate;
    ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2); /* set input SR */

    /* tell port to accept refill at buffers - 1 */
    ALsetfillpoint(audio_port_in,RT_BUFFER_SIZE * (lookaheadbuffers - 1));  
    audio_port_out = 0;
  }
  else if (!strcmp(mode,"duplex")) { // duplex mode

    /* Open the output audio port */
    audio_port_out = ALopenport("STK output port", "w", audio_port_config);
    if(!audio_port_out) {
      fprintf(stderr,"Cannot initialize output audio port: %s\n", alGetErrorString(oserror()));
      exit(0);
    }

    /* Open the input audio port */
    audio_port_in = ALopenport("STK input port", "r", audio_port_config);
    if(!audio_port_in) {
      fprintf(stderr,"Cannot initialize input audio port: %s\n", alGetErrorString(oserror()));
      exit(0);
    }

    /* Set sample rate parameters */
    pvbuf[0] = AL_OUTPUT_RATE;
    pvbuf[1] = (long) srate;
    pvbuf[2] = AL_INPUT_RATE;
    pvbuf[3] = (long) srate;
    ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 4); /* set output SR */
    /* tell port to accept refill at buffers - 1 */
    ALsetfillpoint(audio_port_out,RT_BUFFER_SIZE * (lookaheadbuffers - 1));  
    ALsetfillpoint(audio_port_in,RT_BUFFER_SIZE * (lookaheadbuffers - 1));  
  }
  else {
    fprintf(stderr,"Unsupported RTSoundIO mode: %s\n",mode);
    exit(0);
  }

  ALfreeconfig(audio_port_config);
  audio_port_config = 0;
}

RTSoundIO :: ~RTSoundIO()
{
  if(audio_port_out) ALcloseport(audio_port_out);
  audio_port_out=0;

  if(audio_port_in) ALcloseport(audio_port_in);
  audio_port_in=0;
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
    ALwritesamps(audio_port_out, buf, bufsize);
    return 0;
}

int RTSoundIO :: recordBuffer(short *buf, int bufsize)
{
    ALreadsamps(audio_port_in, buf, bufsize);
    return 0;
}


/* Linux OSS Sound API here */

#elif (defined(__STK_REALTIME_) && defined(__OSS_API_))

#define ABS(x) ((x < 0) ? (-x) : (x))

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels, char *mode)
{
  int lookaheadbuffers = 8;  // number of lookahead buffers
  int nbuf;

  char DEVICE_NAME[100];
  int format;
  int stereo;     /* 0=mono, 1=stereo */
  int stereoset;
  int speed;
  int BUFFER_SIZE_LOG;
  int fragsize;

  BUFFER_SIZE_LOG = (int)(log10((double)RT_BUFFER_SIZE)/log10(2.0));

  /* Check the number of channels */
  if (channels > 2) {
    fprintf(stderr,"RTSoundIO: Unsupported # of audio i/o channels: %d\n", channels);
    exit(0);
  }

  if (channels == 2) stereo = 1;
  else stereo = 0;

  strcpy(DEVICE_NAME,"/dev/dsp");

  if (!strcmp(mode,"play")) { // playback only
    if ((audio_fd = open(DEVICE_NAME, O_WRONLY, 0)) == -1)
      { /* Opening device failed */
        fprintf(stderr,"Cannot open audio device: %s\n",DEVICE_NAME);
        exit(0);
      }
  }
  else if (!strcmp(mode,"record")) { // record only
    if ((audio_fd = open(DEVICE_NAME, O_RDONLY, 0)) == -1)
      { /* Opening device failed */
        fprintf(stderr,"Cannot open audio device: %s\n",DEVICE_NAME);
        exit(0);
      }
  }
  else if (!strcmp(mode,"duplex")) { // duplex mode
    if ((audio_fd = open(DEVICE_NAME, O_RDWR, 0)) == -1)
      { /* Opening device failed */
        fprintf(stderr,"Cannot open audio device: %s\n",DEVICE_NAME);
        exit(0);
      }
    int caps;
    if (ioctl(audio_fd, SNDCTL_DSP_GETCAPS, &caps))
      {
        close(audio_fd);
        fprintf(stderr,"Error getting device capabilities: %s\n",DEVICE_NAME);
        exit(0);
      }
    if (!(caps & DSP_CAP_DUPLEX))
      {
        close(audio_fd);
        fprintf(stderr,"Audio device does not support duplex mode: %s\n",DEVICE_NAME);
        exit(0);
      }
    if (ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, 0))
      {
        close(audio_fd);
        fprintf(stderr,"Error setting duplex mode: %s\n",DEVICE_NAME);
        exit(0);
      }
    int cursrc, srcbit = SOUND_MASK_MIC;
    ioctl(audio_fd, MIXER_READ(SOUND_MIXER_RECSRC),&cursrc);
    srcbit = (srcbit & cursrc);
    ioctl(audio_fd,MIXER_WRITE(SOUND_MIXER_RECSRC),&srcbit);

    // The following opens a direct analog line from the mic to the output
    //srcbit = 99;
    //ioctl(audio_fd,MIXER_WRITE(SOUND_MIXER_IMIX),&srcbit);
  }
  else {
    fprintf(stderr,"Unsupported RTSoundIO mode: %s\n",mode);
    exit(0);
  }

  /* Size the output queue */
  nbuf = (channels == 2) ? lookaheadbuffers : lookaheadbuffers/2;

  fragsize = (nbuf << 16) + BUFFER_SIZE_LOG;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &fragsize))
    {
      close(audio_fd);
      fprintf(stderr,"Error setting audio buffer size!\n");
      exit(0);
    }

  format = AFMT_S16_LE;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format)==-1)
    { /* Fatal error */
      close(audio_fd);
      fprintf(stderr,"SNDCTL_DSP_SETFMT error\n");
      exit(0);
    }

  if (format != AFMT_S16_LE)
    {
      close(audio_fd);
      fprintf(stderr,"Audio device doesn't support 16-bit signed LE format\n");
      exit(0);
    }

  stereoset = stereo;
  if (ioctl(audio_fd, SNDCTL_DSP_STEREO, &stereoset)==-1)
    { /* Fatal error */
      close(audio_fd);
      fprintf(stderr,"SNDCTL_DSP_STEREO\n");
      exit(0);
    }

  if (stereoset != stereo)
    {
      close(audio_fd);
      fprintf(stderr,"The audio device did not set correct stereo mode: %s\n",DEVICE_NAME);
      exit(0);
    }   

  speed = (int)srate;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &speed)==-1)
    { /* Fatal error */
      close(audio_fd);
      fprintf(stderr,"SNDCTL_DSP_SPEED\n");
      exit(0);
    }

  if (ABS(speed - srate)>100)
    {
      close(audio_fd);
      fprintf(stderr,"The device doesn't support the requested speed.\n");
      exit(0);
    }
}

RTSoundIO :: ~RTSoundIO()
{
  if(audio_fd) close(audio_fd);
  audio_fd=0;
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
  /* The OSS write() routine takes the buffer size in bytes, thus the
     multiplication by two.
  */
  int len;

  if ((len = write(audio_fd, buf, 2*bufsize)) == -1)
    {
      fprintf(stderr,"Audio write error!\n");
      return -1;
    }
  return 0;
}

int RTSoundIO :: recordBuffer(short *buf, int bufsize)
{
  /* The OSS read() routine takes the buffer size in bytes, thus the
     multiplication by two.
  */
  int len;

  if ((len = read(audio_fd, buf, 2*bufsize)) == -1)
    {
      fprintf(stderr,"Audio read error!\n");
      return -1;
    }
  return 0;
}


#elif (defined(__STK_REALTIME_) && defined(__WINDS_API_) )
/*
 * AUDIO OUTPUT:
 *
 * There are two ways (or more) to go about handling sound output
 * under DirectSound.  For minimum latency, one should write new
 * buffers in front of the read pointer (Method 1).  The other
 * method is to always write new buffers of data behind the read
 * pointer (Method 2).  Method 2 is very safe but inherently
 * produces a delay equivalent to the entire sound buffer (plus
 * any other delays that Microsloth provides).  Even using
 * Method 1, however, Microsloth requires that you leave about
 * 15 ms of buffer between the read and write pointers. I've tried
 * both methods and neither results in performance that can compare
 * to either Linux or SGI sound output.  In order to minimize
 * latency, however, I'll go with Method 1 and leave Method 2
 * commented out below.
 *
 * If the primary sound buffer exists in hardware, I will write
 * directly to it.  If the primary sound buffer is emulated in
 * software, this is not possible and we must use a secondary buffer.
 *
 * AUDIO INPUT:
 *
 * I didn't spend a lot of time doing the audio input code.  I
 * basically got it working, heard that it had noticeable delay,
 * but didn't try screwing around to minimize that delay.  I provide
 * a single variable (at the beginning of the Capture initialization)
 * which controls the size of the DirectSound Capture buffer, which
 * in turn controls the latency.
 *
 * The DirectSoundCapture API is only available with DirectX versions
 * 5.0 and higher.  If you don't have such capabilities (ex. WinNT),
 * then use the WinMM API code (uncomment the __WINMM_API_ define
 * statement in Object.h and comment out the __WINDS_API_ flag).  This
 * will allow you to still compile STK, but without the audio input
 * capabilities.
 */

#include	<stdio.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels, char *mode)
{
  HRESULT result;
  WAVEFORMATEX  wfFormat;
  BYTE *pAudioPtr;
  DWORD dwDataLen;

  // Initialize the DirectSound object and buffer pointers to NULL
  lpDirectSound = NULL;
  lpDSBuffer = NULL;
  lpDSCapture = NULL;
  lpDSCBuffer = NULL;

  // Define the wave format structure (16-bit PCM, srate, channels)
  ZeroMemory(&wfFormat, sizeof(WAVEFORMATEX));
  wfFormat.wFormatTag = WAVE_FORMAT_PCM;
  wfFormat.nChannels = channels;
  wfFormat.nSamplesPerSec = (unsigned long) srate;
  wfFormat.wBitsPerSample = 8 * sizeof(short);
  wfFormat.nBlockAlign = wfFormat.nChannels * wfFormat.wBitsPerSample / 8;
  wfFormat.nAvgBytesPerSec = wfFormat.nSamplesPerSec * wfFormat.nBlockAlign;

  if ( (!strcmp(mode,"play")) || (!strcmp(mode,"duplex")) ) {
    DSBUFFERDESC dsbdesc;
    HWND hWnd;

    // If using Method 1:
    // Define a maximum distance that the write pointer is
    // allowed to lead safePos.  The size of this zone is
    // fairly critical to the behavior of this scheme.  The
    // value below is set for a 10 millisecond region.
    zoneSize = (DWORD) (0.01 * srate * sizeof(short));  // bytes

    // Create the DS object
    if ((result = DirectSoundCreate(NULL, &lpDirectSound, NULL)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Cannot open default sound output device!!\n");
      exit(0);
    }

    // Get DS device capabilites
    DSCAPS dscaps;
    ZeroMemory(&dscaps, sizeof(DSCAPS));
    dscaps.dwSize = sizeof(DSCAPS);
    if ((result = lpDirectSound->GetCaps(&dscaps)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Cannot get DS device capabilities!!\n");
      exit(0);
    }

    // Determine whether primary buffer exists in hardware or software
    if (dscaps.dwFlags & DSCAPS_EMULDRIVER) { // Write to secondary buffer

      // Number of buffers of size RT_BUFFER_SIZE to make secondary
      // DS buffer.  A larger secondary buffer does NOT produce more
      // output delay when the write pointer is kept in front of the
      // read pointer (Method 1).  However, if you use Method 2, this
      // variable will be more critical and you'll probably want to
      // make it smaller.
      int nBufs = 12;

      // Set coooperative level
      hWnd = GetForegroundWindow();
      if ((result = lpDirectSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Couldn't set EXCLUSIVE cooperative level!\n");
        exit(0);
      }

      // Even though we will write to the secondary buffer, we need
      // to access the primary buffer to set the correct output format.
      // The default is 8-bit, 22 kHz!
      LPDIRECTSOUNDBUFFER lpdsbPrimary;

      // Setup the DS primary buffer description.
      ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
      dsbdesc.dwSize = sizeof(DSBUFFERDESC);
      dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;

      // Obtain the primary buffer
      if ((result = lpDirectSound->CreateSoundBuffer(&dsbdesc,
                                &lpdsbPrimary, NULL)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Cannot get the primary DS buffer address!\n");
        exit(0);
      }

      // Set the primary DS buffer sound format.
      if ((result = lpdsbPrimary->SetFormat(&wfFormat)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Cannot set the primary DS buffer to proper sound format!\n");
        exit(0);
      }

      // Setup the secondary DS buffer description.
      dwDSBufSize = RT_BUFFER_SIZE * sizeof(short) * nBufs;
      ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
      dsbdesc.dwSize = sizeof(DSBUFFERDESC);
      dsbdesc.dwFlags = DSBCAPS_STICKYFOCUS | DSBCAPS_GETCURRENTPOSITION2;
      dsbdesc.dwBufferBytes = dwDSBufSize;
      dsbdesc.lpwfxFormat = &wfFormat;

      // Try to create the secondary DS buffer.
      if ((result = lpDirectSound->CreateSoundBuffer(&dsbdesc, &lpDSBuffer, NULL)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Couldn't create the DS sound buffer!\n");
        exit(0);
      }
    }
    else { // Write to primary buffer

      // Setup the DS primary buffer description.
      ZeroMemory(&dsbdesc, sizeof(DSBUFFERDESC));
      dsbdesc.dwSize = sizeof(DSBUFFERDESC);
      dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_STICKYFOCUS;
      // Buffer size is determined by sound hardware
      dsbdesc.dwBufferBytes = 0;
      dsbdesc.lpwfxFormat = NULL;  // Must be NULL for primary buffer.

      // Obtain write-primary coooperative level.
      hWnd = GetForegroundWindow();
      if ((result = lpDirectSound->SetCooperativeLevel(hWnd, DSSCL_WRITEPRIMARY)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Couldn't set WRITEPRIMARY cooperative level!\n");
        exit(0);
      }

      // Try to create the primary DS buffer.
      if ((result = lpDirectSound->CreateSoundBuffer(&dsbdesc, &lpDSBuffer, NULL)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Couldn't create the DS sound buffer!\n");
        exit(0);
      }

      // Set primary DS buffer to desired format.
      if ((result = lpDSBuffer->SetFormat(&wfFormat)) != DS_OK) {
        fprintf(stderr,"RTSoundIO: Couldn't set correct format!\n");
        exit(0);
      }
    }
    // Get the buffer size
    DSBCAPS dsbcaps;
    dsbcaps.dwSize = sizeof(DSBCAPS);
    lpDSBuffer->GetCaps(&dsbcaps);
    dwDSBufSize = dsbcaps.dwBufferBytes;

    // Lock the DS buffer
    if ((result = lpDSBuffer->Lock(0, dwDSBufSize, (LPLPVOID) &pAudioPtr, &dwDataLen, NULL, NULL, 0)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't lock DS sound buffer!\n");
      exit(0);
    }

    // Zero the DS buffer
    ZeroMemory(pAudioPtr, dwDataLen);

    // Unlock the DS buffer
    if ((result = lpDSBuffer->Unlock(pAudioPtr, dwDataLen, NULL, 0)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't unlock DS sound buffer!\n");
      exit(0);
    }
  } // end of play/duplex initialization

  if ( (!strcmp(mode,"record")) || (!strcmp(mode,"duplex")) ) {
    // DirectSound Capture capabilities require DirectX 5.0 or higher

    // The following variable controls the size of the DS Capture
    // buffer, which in turns controls the latency in the capture
    // process.  When dscbufscale = 1, the buffer is equivalent to
    // one second of audio.  A dscbufscale = 0.5 halves this value.
    // Likewise, a dscbufscale = 2 doubles this value.  It seems to
    // work OK with dscbufscale = 0.5, but there is a periodic
    // "clicking" which sometimes occurs.  You can go lower, but
    // the "clicking" gets worse.  Good luck!  Yet another reason
    // why not to use Windoze.
    float dscbufscale = 0.5;

    // Create the DS Capture object
    if ((result = DirectSoundCaptureCreate(NULL, &lpDSCapture, NULL)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't open DirectSoundCapture Object!\n");
      fprintf(stderr,"RTSoundIO: Requires DirectX 5 or later.\n");
      exit(0);
    }

    // Setup the DS Capture buffer description
    DSCBUFFERDESC dscbdesc;
    ZeroMemory(&dscbdesc, sizeof(DSCBUFFERDESC));
    dscbdesc.dwSize = sizeof(DSCBUFFERDESC);
    dscbdesc.dwFlags = 0;
    // Control size of DS Capture buffer here
    dwDSCBufSize = (DWORD)(wfFormat.nAvgBytesPerSec*dscbufscale);
    dscbdesc.dwBufferBytes = dwDSCBufSize;
    dscbdesc.dwReserved = 0;
    dscbdesc.lpwfxFormat = &wfFormat;

    // Create the DS Capture buffer
    if ((result = lpDSCapture->CreateCaptureBuffer(&dscbdesc, &lpDSCBuffer, NULL)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't create DirectSoundCapture buffer!\n");
      if (result == DSERR_BADFORMAT) {
        fprintf(stderr,"RTSoundIO: The input device could not support the desired\n");
        fprintf(stderr,"sample rate (%f), bits per sample (16),\n", srate);
        fprintf(stderr,"and/or number of channels (%d).\n", channels);
      }
      exit(0);
    }

    // Lock the DS Capture buffer
    if ((result = lpDSCBuffer->Lock(0, dwDSCBufSize, (LPLPVOID) &pAudioPtr, &dwDataLen, NULL, NULL, 0)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't lock DS Capture sound buffer!\n");
      exit(0);
    }

    // Zero the DS Capture buffer
    ZeroMemory(pAudioPtr, dwDataLen);

    // Unlock the DS Capture buffer
    if ((result = lpDSCBuffer->Unlock(pAudioPtr, dwDataLen, NULL, 0)) != DS_OK) {
      fprintf(stderr,"RTSoundIO: Couldn't unlock DS Capture sound buffer!\n");
      exit(0);
    }

    // Start the DS Capture buffer input
    if ((result = lpDSCBuffer->Start(DSCBSTART_LOOPING) != DS_OK)) {
      fprintf(stderr,"RTSoundIO: Couldn't start DS Capture sound input!\n");
      exit(0);
    }
  } // end of record/duplex initialization

  if ( (!strcmp(mode,"play")) || (!strcmp(mode,"duplex")) ) {
    // Start the DS buffer playback
    if ((result = lpDSBuffer->Play(0, 0, DSBPLAY_LOOPING ) != DS_OK)) {
      fprintf(stderr,"RTSoundIO: Couldn't play DS sound buffer!\n");
      exit(0);
    }
  }
}

RTSoundIO :: ~RTSoundIO()
{
  // Cleanup the DS buffers
  if (lpDSBuffer)	{
	  lpDSBuffer->Stop();
	  lpDSBuffer->Release();
	  lpDSBuffer = NULL;
	}
  if (lpDSCBuffer) {
	  lpDSCBuffer->Stop();
	  lpDSCBuffer->Release();
	  lpDSCBuffer = NULL;
	}

  // Cleanup the DS objects
  if (lpDirectSound) {
	  lpDirectSound->Release();
	  lpDirectSound = NULL;
	}
  if (lpDSCapture) {
	  lpDSCapture->Release();
	  lpDSCapture = NULL;
	}
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
  HRESULT hr;
  LPVOID lpbuf1 = NULL;
  LPVOID lpbuf2 = NULL;
  DWORD dwsize1 = 0;
  DWORD dwsize2 = 0;
  DWORD playPos, safePos;
  static UINT nextWritePos = 0;

  // Find out where the read and "safe write" pointers are.
  hr = lpDSBuffer->GetCurrentPosition(&playPos, &safePos);
  if (hr != DS_OK) return -1;

  // METHOD 1: Keep write pointer in front of read pointer.
  //
  // Microsloth says that the safePos is about 15 ms ahead of
  // playPos.  I think this figure is somewhat hardware related, 
  // especially if you are writing to the primary buffer.  With
  // my shit-blaster 16, I found the safePos to be about 10 ms
  // ahead of playPos. If you really need to reduce delay, you
  // can try moving your "safePos" closer to the play pointer.
  // You'll be treading on dangerous ground, but then again,
  // you're obviously using Windoze so you're already familiar
  // with such uncertainty!  I've been able to lop off 2-5 ms
  // in some circumstances.
  //static DWORD backup = (DWORD) (0.005 * SRATE * sizeof(short));
  //safePos = (safePos + dwDSBufSize - backup) % dwDSBufSize;

  // Assume that the next write position is always in front
  // of safePos.  If not, the write pointer must have wrapped.
  // NOTE: If safePos somehow gets ahead of the write pointer,
  // then an underrun has occurred and there's not much we can
  // do anyway.
  DWORD deltaPos;
  if( safePos > nextWritePos )
    deltaPos = nextWritePos + dwDSBufSize - safePos;
  else
    deltaPos = nextWritePos - safePos;

  // Check whether the write pointer is in the allowed region.
  while ( deltaPos > zoneSize ) {
    // If we are here, then we must wait until the write pointer
    // is in the allowed region.  For this, we can either
    // continuously check the pointer positions until they are
    // OK or we can use the Sleep() function to pause operations
    // for a certain amount of time.  Use of the Sleep() function
    // would seem to be the better choice, however, there are
    // reports that Sleep() often "sleeps" for much longer than
    // requested.  I'll let you choose which method to use.
    static int sleep = 1;    // 1 = sleep, 0 = don't sleep

    if (sleep) {
      // Sleep until safePos catches up. Calculate number of
      // milliseconds to wait as:
      // time = distance * (milliseconds/second) * fudgefactor / 
      //        ((bytes/sample) * (samples/second))
      // A "fudgefactor" less than 1 is used because it was found
      // that sleeping too long was MUCH worse than sleeping for
      // several shorter periods.
      DWORD millis = (DWORD) ((deltaPos * 200.0) / ( sizeof(short) * SRATE));

      // Sleep for that long
      Sleep( millis );
    }

    // Wake up, find out where we are now
    hr = lpDSBuffer->GetCurrentPosition( &playPos, &safePos );
    if( hr != DS_OK ) return -1;

    // Backup safePos?  (See above)
    //safePos = (safePos + dwDSBufSize - backup) % dwDSBufSize;

    if( safePos > nextWritePos )
      deltaPos = nextWritePos + dwDSBufSize - safePos;
    else
      deltaPos = nextWritePos - safePos;
  }
  // End of Method 1

  /*
  // METHOD 2: Keep write region behind of play pointer.
  if( playPos < nextWritePos ) playPos += dwDSBufSize; // unwrap offset
  DWORD endWrite = nextWritePos + bufsize * sizeof(short);

  // Check whether the write region is behind the play pointer.
  while ( playPos < endWrite ) {
    // If we are here, then we must wait until the play pointer
    // gets beyond the write region.  For this, we can either
    // continuously check the pointer positions until they are
    // OK or we can use the Sleep() function to pause operations
    // for a certain amount of time.  Use of the Sleep() function
    // would seem to be the better choice, however, there are
    // reports that Sleep() often "sleeps" for much longer than
    // requested.  I'll let you choose which method to use.
    static int sleep = 1;    // 1 = sleep, 0 = don't sleep

    if (sleep) {
      // Sleep until safePos catches up. Calculate number of
      // milliseconds to wait as:
      // time = distance * (milliseconds/second) * fudgefactor / 
      //        ((bytes/sample) * (samples/second))
      // A "fudgefactor" less than 1 is used because it was found
      // that sleeping too long was MUCH worse than sleeping for
      // several shorter periods.
      DWORD millis = (DWORD) (((endWrite - playPos) * 200.0) / ( sizeof(short) * SRATE));

      // Sleep for that long
      Sleep( millis );
    }

    // Wake up, find out where we are now
    hr = lpDSBuffer->GetCurrentPosition( &playPos, &safePos );
    if( hr != DS_OK ) return -1;
    if( playPos < nextWritePos ) playPos += dwDSBufSize; // unwrap offset
  }
  // End of Method 2.
  */

  // Lock free space in the DS
  hr = lpDSBuffer->Lock (nextWritePos, bufsize * sizeof(short), &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);
  if (hr == DS_OK) {
    // Copy the buffer into the DS
    CopyMemory(lpbuf1, buf, dwsize1);
    if(NULL != lpbuf2) CopyMemory(lpbuf2, buf+dwsize1, dwsize2);

    // Update our buffer offset and unlock sound buffer
    nextWritePos = (nextWritePos + dwsize1 + dwsize2) % dwDSBufSize;
    lpDSBuffer->Unlock (lpbuf1, dwsize1, lpbuf2, dwsize2);
    return 0;
  }
  else return -1;
}

int RTSoundIO :: recordBuffer(short *buf, int bufsize)
{
  HRESULT hr;
  LPVOID lpbuf1 = NULL;
  LPVOID lpbuf2 = NULL;
  DWORD dwsize1 = 0;
  DWORD dwsize2 = 0;
  DWORD capPos, safePos;
  static UINT nextReadPos = 0;

  // Find out where the write and "safe read" pointers are.
  hr = lpDSCBuffer->GetCurrentPosition(&capPos, &safePos);
  if (hr != DS_OK) return -1;
  //printf("capPos = %d, safePos = %d\n", capPos, safePos);

  if( safePos < nextReadPos ) safePos += dwDSCBufSize; // unwrap offset
  DWORD endRead = nextReadPos + bufsize * sizeof(short);
  //printf("endRead = %d\n", endRead);

  // Check whether the read region is behind the capture pointer.
  while ( safePos < endRead ) {
    // If we are here, then we must wait until the read pointer
    // gets beyond the write region.  For this, we can either
    // continuously check the pointer positions until they are
    // OK or we can use the Sleep() function to pause operations
    // for a certain amount of time.  Use of the Sleep() function
    // would seem to be the better choice, however, there are
    // reports that Sleep() often "sleeps" for much longer than
    // requested.  I'll let you choose which method to use.
    static int sleep = 1;    // 1 = sleep, 0 = don't sleep

    if (sleep) {
      // Sleep until safePos catches up. Calculate number of
      // milliseconds to wait as:
      // time = distance * (milliseconds/second) * fudgefactor / 
      //        ((bytes/sample) * (samples/second))
      // A "fudgefactor" less than 1 is used because it was found
      // that sleeping too long was MUCH worse than sleeping for
      // several shorter periods.
      DWORD millis = (DWORD) (((endRead - safePos) * 200.0) / ( sizeof(short) * SRATE));

      // Sleep for that long
      Sleep( millis );
    }

    // Wake up, find out where we are now
    hr = lpDSCBuffer->GetCurrentPosition( &capPos, &safePos );
    if( hr != DS_OK ) return -1;
    //printf("capPos = %d, safePos = %d\n", capPos, safePos);
    if( safePos < nextReadPos ) safePos += dwDSCBufSize; // unwrap offset
  }
  //printf("how about here?\n");
  // Lock free space in the DS Capture buffer
  hr = lpDSCBuffer->Lock(nextReadPos, bufsize * sizeof(short), &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);
  if (hr == DS_OK) {
    // Copy the DS Capture data to the buffer
    CopyMemory(buf, lpbuf1, dwsize1);
    if(NULL != lpbuf2) CopyMemory(buf+dwsize1, lpbuf2, dwsize2);

    // Update our buffer offset and unlock sound buffer
    nextReadPos = (nextReadPos + dwsize1 + dwsize2) % dwDSCBufSize;
    lpDSCBuffer->Unlock (lpbuf1, dwsize1, lpbuf2, dwsize2);
    return 0;
  }
  else return -1;
}

#elif (defined(__STK_REALTIME_) && defined(__WINMM_API_) )

#include	<stdio.h>

#define FRAMETIME (long) (1000.0 * RT_BUFFER_SIZE / SRATE)

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels, char *mode)
{
  MMRESULT		result;
  WAVEFORMATEX	wfx;
  int           bufferSize = RT_BUFFER_SIZE;
	
  audioPort = NULL;

  if ( (!strcmp(mode,"record")) || (!strcmp(mode,"duplex")) ) {
    fprintf(stderr,"Sorry ... no audio input support under WinMM API!\n");
    exit(0);
  }

  wfx.wFormatTag = WAVE_FORMAT_PCM;
  wfx.nChannels = channels;
  wfx.nSamplesPerSec = (unsigned long) srate;
  wfx.nBlockAlign = sizeof(short) * channels;
  wfx.nAvgBytesPerSec =  (unsigned long) srate * wfx.nBlockAlign;
  wfx.wBitsPerSample = 8 * sizeof(short);
  wfx.cbSize = 0;

  /* Open a Wave Out device using the wave mapper to guide us */ 
  result = waveOutOpen(&audioPort,WAVE_MAPPER,&wfx,(DWORD)NULL,(DWORD)NULL,CALLBACK_NULL);
  if (result != MMSYSERR_NOERROR) {
    fprintf(stderr,"RTSoundIO: Cannot open audio port (WinMM API)!\n");
    exit(0);
  }

  for( outBufNum = 0; outBufNum < (UINT)NUM_OUT_BUFFERS; outBufNum++ ) 
    { 
      /* set up a couple of wave headers */ 
      whOut[outBufNum].lpData = (LPSTR)calloc(channels*bufferSize, sizeof(short)); 
      if (whOut[outBufNum].lpData == NULL){ 
        waveOutClose( audioPort );
        fprintf(stderr,"RTSoundIO: Error initializing audio buffers (WinMM API)!\n");
        exit(0);
      } 
      whOut[outBufNum].dwBufferLength = channels*bufferSize*sizeof(short); 
      whOut[outBufNum].dwBytesRecorded = 0; 
      whOut[outBufNum].dwUser = 1; 
      //whOut[outBufNum].dwFlags = 0; 
      whOut[outBufNum].dwFlags = WHDR_DONE; 
      whOut[outBufNum].dwLoops = 0; 
      whOut[outBufNum].lpNext = NULL; 
      whOut[outBufNum].reserved = 0;
    } 

  /* Write the first buffer out to get things going */
  outBufNum = 0; 
  result = waveOutPrepareHeader(audioPort, &whOut[outBufNum],sizeof(WAVEHDR)); 
  result = waveOutWrite(audioPort, &whOut[outBufNum], sizeof(WAVEHDR)); 

  /* Keep track of time so that we know how long we can sleep */ 
  lastWriteTime =  timeGetTime(); 
}

RTSoundIO :: ~RTSoundIO()
{
  MMRESULT result;
  long timeToGo;

  /* Close Audio Port */
  if (audioPort != NULL) {
    result = waveOutReset(audioPort); 
    for( outBufNum = 0; outBufNum < (UINT)NUM_OUT_BUFFERS; outBufNum++ ) 
      { 
        /* Loop until the next waveheader indicates that we are done */ 
        while( !(whOut[outBufNum].dwFlags & WHDR_DONE) ) 
          { 
            //printf("."); 
            timeToGo = (long) (FRAMETIME - (timeGetTime()-lastWriteTime)); 
            if( timeToGo > 0 ) Sleep( (long) timeToGo ); 
          } 
        /* Unprepare the header */ 
        result = waveOutUnprepareHeader(audioPort, &whOut[outBufNum],sizeof(WAVEHDR));	 
        if (whOut[outBufNum].lpData != NULL) { 
          free(whOut[outBufNum].lpData); 
          whOut[outBufNum].lpData = NULL; 
        } 
      } 
    result = waveOutClose(audioPort); 
  } 
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
  MMRESULT	result; 
  long timeToGo; 

  outBufNum++; 
  if( outBufNum >= (UINT)NUM_OUT_BUFFERS ) outBufNum = 0; 

  /* Loop until the next waveheader indicates that we are done */ 
  while( !(whOut[outBufNum].dwFlags & WHDR_DONE) ) 
    { 
      //printf(".");
      timeToGo = (long) (FRAMETIME - (timeGetTime()-lastWriteTime)); 
      //timeToGo = (long) (FRAMETIME * 0.5); 
      if( timeToGo > 0 ) Sleep( (long) timeToGo ); 
    } 
  result = waveOutUnprepareHeader(audioPort, &whOut[outBufNum], sizeof(WAVEHDR)); 
 
  memcpy( whOut[outBufNum].lpData, buf, bufsize*sizeof(short));  
  result = waveOutPrepareHeader(audioPort, &whOut[outBufNum], sizeof(WAVEHDR)); 
  result = waveOutWrite(audioPort, &whOut[outBufNum], sizeof(WAVEHDR)); 
  lastWriteTime = timeGetTime(); 

  return 0;
}

int RTSoundIO :: recordBuffer(short *buf, int bufsize)
{
  // There is no current support for audio input under the WinMM API ... sorry!
  return -1;
}

#endif
