/******************************************/
/*  RTSoundIO.cpp                         */
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

#include "RTSoundIO.h"

#if defined(__SGI_REALTIME_)

#include <stdio.h>
#include <unistd.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels)
{
  ALconfig audio_port_config;
  int lookaheadbuffers = 8;  // number of lookahead buffers
  long pvbuf[2];
  int nbuf, totalBufSize;

  audio_port_config = ALnewconfig();
  if(ALsetchannels(audio_port_config, channels) < 0) {
	 fprintf(stderr,"Cannot initialize audio port\n");
	 exit(0);
  }

  /* Size the output queue */
  nbuf = (channels == 2) ? lookaheadbuffers : lookaheadbuffers/2;
  totalBufSize = RT_BUFFER_SIZE * nbuf;
  if(ALsetqueuesize(audio_port_config, totalBufSize) < 0) {
	 fprintf(stderr,"Cannot initialize audio port\n");
	 exit(0);
  }

  if(audio_port) ALcloseport(audio_port);
  audio_port = ALopenport("sgi.driver", "w", audio_port_config);
  if(!audio_port) {
	 fprintf(stderr,"Cannot initialize audio port\n");
	 exit(0);
  }
  ALfreeconfig(audio_port_config);
  audio_port_config = 0;

  pvbuf[0] = AL_OUTPUT_RATE;
  pvbuf[1] = (long) srate;
  ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2); /* set output SR */
  /* tell port to accept refill at buffers - 1 */
  ALsetfillpoint(audio_port,RT_BUFFER_SIZE * (lookaheadbuffers - 1));  
}

RTSoundIO :: ~RTSoundIO()
{
  if(audio_port) ALcloseport(audio_port);
  audio_port=0;
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
    ALwritesamps(audio_port, buf, bufsize);
    return 0;
}

#elif defined(__USS_REALTIME_)

#define ABS(x) ((x < 0) ? (-x) : (x))

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels)
{
  int lookaheadbuffers = 8;  // number of lookahead buffers
  int nbuf, totalBufSize;

  char DEVICE_NAME[100];
  int format;
  int stereo;     /* 0=mono, 1=stereo */
  int stereoset;
  int speed;
  int BUFFER_SIZE_LOG;
  int fragsize;

  BUFFER_SIZE_LOG = (int)(log10((double)RT_BUFFER_SIZE)/log10(2.0));

  if (channels > 2)
	{
	  fprintf(stderr,"Unsupported # of output channels!\n");
	  exit(0);
	}

  if (channels == 2) stereo = 1;
  else stereo = 0;

  strcpy(DEVICE_NAME,"/dev/dspW");

  if ((audio_fd = open(DEVICE_NAME, O_WRONLY, 0)) == -1)
    { /* Opening device failed */
	  fprintf(stderr,"Cannot open audio device: %s\n",DEVICE_NAME);
	  exit(0);
    }

  /* Size the output queue */
  nbuf = (channels == 2) ? lookaheadbuffers : lookaheadbuffers/2;
  totalBufSize = RT_BUFFER_SIZE * nbuf;

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
	  fprintf(stderr,"Soundcard doesn't support 16-bit signed LE format\n");
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
	  fprintf(stderr,"The sound card did not set correct stereo mode\n");
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

#elif defined(__WINDS_REALTIME_)

#include	<stdio.h>

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels)
{
  HRESULT result;
  HWND hWnd;
  DWORD dwDataLen;
  WAVEFORMATEX  wfFormat;
  DSBUFFERDESC  dsbdDesc, primarydsbDesc;
  LPDIRECTSOUNDBUFFER m_pDSPrimeBuffer;
  BYTE *pDSBuffData;

  /* Number of buffers of size RT_BUFFER_SIZE to make secondary DS buffer */
  int nBufs = 16;

  /* Initialize pointers to NULL */
  m_pDirectSound = NULL;
  m_pDSBuffer = NULL;
  m_pDSPrimeBuffer = NULL;

  /* Create the DS object */
  if ((result = DirectSoundCreate(NULL, &m_pDirectSound, NULL)) != DS_OK)
    {
      fprintf(stderr,"Cannot open default sound device!!\n");
      exit(0);
    }
  hWnd = GetForegroundWindow();
  if ((result = m_pDirectSound->SetCooperativeLevel(hWnd, DSSCL_EXCLUSIVE)) != DS_OK)
    {
      fprintf(stderr,"DS Constructor: couldn't set cooperative level!\n");
      exit(0);
    }

  /* Define the wave format structure */
  wfFormat.wFormatTag = WAVE_FORMAT_PCM;
  wfFormat.nChannels = channels;
  wfFormat.nSamplesPerSec = (unsigned long) srate;
  wfFormat.wBitsPerSample = 8 * sizeof(short);
  wfFormat.nBlockAlign = wfFormat.nChannels * wfFormat.wBitsPerSample / 8;
  wfFormat.nAvgBytesPerSec = wfFormat.nSamplesPerSec * wfFormat.nBlockAlign;
  wfFormat.cbSize = 0;

  /* Setup the primary DS buffer description */
  ZeroMemory(&primarydsbDesc, sizeof(DSBUFFERDESC));
  primarydsbDesc.dwSize = sizeof(DSBUFFERDESC);
  primarydsbDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
  primarydsbDesc.dwBufferBytes = 0;
  primarydsbDesc.lpwfxFormat = NULL;

  /* Create the primary DS buffer */
  if ((result = m_pDirectSound->CreateSoundBuffer(&primarydsbDesc,
				&m_pDSPrimeBuffer, NULL)) != DS_OK)
    {
      fprintf(stderr,"Cannot get the primary DS buffer address!\n");
      exit(0);
    }

  /* Set the primary DS buffer sound format.  We have to do this because
     the default primary buffer is 8-bit, 22kHz! */
  if ((result = m_pDSPrimeBuffer->SetFormat(&wfFormat)) != DS_OK)
    {
      fprintf(stderr,"Cannot set the primary DS buffer to proper sound format!\n");
      exit(0);
    }

  /* Setup the secondary DS buffer description */
  m_cbBufSize = RT_BUFFER_SIZE * sizeof(short) * nBufs;
  ZeroMemory(&dsbdDesc, sizeof(DSBUFFERDESC));
  dsbdDesc.dwSize = sizeof(DSBUFFERDESC);
  dsbdDesc.dwFlags = DSBCAPS_GLOBALFOCUS;
  dsbdDesc.dwBufferBytes = m_cbBufSize;
  dsbdDesc.lpwfxFormat = &wfFormat;

  /* Create the secondary DS buffer */
  if ((result = m_pDirectSound->CreateSoundBuffer(&dsbdDesc,
												  &m_pDSBuffer, NULL)) != DS_OK)
    {
      fprintf(stderr,"DS Constructor: couldn't create sound buffer!\n");
      exit(0);
    }

  /* Lock the DS buffer */
  if ((result = m_pDSBuffer->Lock(0, m_cbBufSize, (LPLPVOID)&pDSBuffData,
								  &dwDataLen, NULL, NULL, 0)) != DS_OK)
    {
      fprintf(stderr,"DS Constructor: couldn't lock sound buffer!\n");
      exit(0);
    }

  /* Zero the DS buffer */
  ZeroMemory(pDSBuffData, dwDataLen);

  /* Unlock the DS buffer */
  if ((result = m_pDSBuffer->Unlock(pDSBuffData, dwDataLen, NULL, 0)) != DS_OK)
    {
      fprintf(stderr,"DS Constructor: couldn't unlock sound buffer!\n");
      exit(0);
    }

  m_cbBufOffset = 0;  // reset last write position to start of buffer

  /* Start the buffer playback */
  if ((result = m_pDSBuffer->Play( 0, 0, DSBPLAY_LOOPING ) != DS_OK))
    {
      fprintf(stderr,"DS Constructor: couldn't play sound buffer!\n");
      exit(0);
    }
}

RTSoundIO :: ~RTSoundIO()
{
  // Cleanup the sound buffer
  if (m_pDSBuffer)
	{
	  m_pDSBuffer->Stop();
	  m_pDSBuffer->Release();
	  m_pDSBuffer = NULL;
	}

  // Cleanup the DS object
  if (m_pDirectSound)
	{
	  m_pDirectSound->Release();
	  m_pDirectSound = NULL;
	}
}

int RTSoundIO :: playBuffer(short *buf, int bufsize)
{
  HRESULT hr;
  DWORD status;
  LPVOID lpbuf1 = NULL;
  LPVOID lpbuf2 = NULL;
  DWORD dwsize1 = 0;
  DWORD dwsize2 = 0;
  DWORD playPos, safePos, endWrite;
  DWORD millis;

  // Should be playing, right?
  hr = m_pDSBuffer->GetStatus( &status );
  if (!(status && DSBSTATUS_PLAYING))
    {
      printf("Buffer not playing!\n");
    }

  // Sleep until we have enough room in buffer.
  hr = m_pDSBuffer->GetCurrentPosition( &playPos, &safePos );
  if( hr != DS_OK ) return -1;
  if( playPos < m_cbBufOffset ) playPos += m_cbBufSize; // unwrap offset

  endWrite = m_cbBufOffset + RT_BUFFER_SIZE * sizeof(short);
  while ( playPos < endWrite ) {
    // Calculate number of milliseconds until we will have room, as
    // time = distance * (milliseconds/second) / ((bytes/sample) * (samples/second)),
    // rounded up.
    millis = (DWORD) (1.0 + ((endWrite - playPos) * 1000.0) / ( sizeof(short) * SRATE));

    // Sleep for that long
    Sleep( millis );

    // Wake up, find out where we are now
    hr = m_pDSBuffer->GetCurrentPosition( &playPos, &safePos );
    if( hr != DS_OK ) return -1;
    if( playPos < m_cbBufOffset ) playPos += m_cbBufSize; // unwrap offset
  }

  // Lock free space in the DS
  hr = m_pDSBuffer->Lock (m_cbBufOffset, RT_BUFFER_SIZE * sizeof(short), &lpbuf1, &dwsize1, &lpbuf2, &dwsize2, 0);
  if (hr == DS_OK)
    {
      // Copy the buffer into the DS
      CopyMemory(lpbuf1, buf, dwsize1);
      if(NULL != lpbuf2) CopyMemory(lpbuf2, buf+dwsize1, dwsize2);

      // Update our buffer offset and unlock sound buffer
      m_cbBufOffset = (m_cbBufOffset + dwsize1 + dwsize2) % m_cbBufSize;
      m_pDSBuffer->Unlock (lpbuf1, dwsize1, lpbuf2, dwsize2);
    }
  return 0;
}

#elif defined(__WINMM_REALTIME_)

#include	<stdio.h>

#define FRAMETIME (long) (1000.0 * RT_BUFFER_SIZE / SRATE)

RTSoundIO :: RTSoundIO(MY_FLOAT srate, int channels)
{
  MMRESULT		result;
  WAVEFORMATEX	wfx;
  int           bufferSize = RT_BUFFER_SIZE;
	
  audioPort = NULL;

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
    fprintf(stderr,"Cannot open audio port!\n");
    exit(0);
  }

  for( outBufNum = 0; outBufNum < (UINT)NUM_OUT_BUFFERS; outBufNum++ ) 
    { 
      /* set up a couple of wave headers */ 
      whOut[outBufNum].lpData = (LPSTR)calloc(channels*bufferSize, sizeof(short)); 
      if (whOut[outBufNum].lpData == NULL){ 
        waveOutClose( audioPort );
        fprintf(stderr,"Error initializing audio buffers!\n");
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

#endif
