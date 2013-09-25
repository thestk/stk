/******************************************/
/*  MIDIIO.cpp                            */
/*  Realtime MIDI I/O Object for STK,     */
/*  by Gary P. Scavone, 1998.             */
/*  Based in part on code by Perry        */
/*  Cook (SGI), Paul Leonard (Linux),     */
/*  the RoseGarden team (Linux), and      */
/*  Bill Putnam (Win95/NT).               */
/*                                        */
/*  At the moment, this object only       */
/*  handles MIDI Input, though MIDI       */
/*  Output code can go here when someone  */
/*  decides they need it (and writes it). */
/*                                        */
/*  This object opens a MIDI Input device */
/*  and parses MIDI messages into a MIDI  */
/*  buffer.  Time stamp info is converted */
/*  to deltaTime. MIDI data is stored as  */
/*  MY_FLOAT to conform with SKINI.       */
/******************************************/

#include "MIDIIO.h"

#define MIDI_BUFFER_SIZE 1024
int writeOffset;
int readOffset;


#if defined(__SGI_REALTIME_)

/*************************************/
/*         __SGI_REALTIME_           */
/*************************************/

#include <dmedia/midi.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

MDport inport;

MDevent *midiBuffer;

pid_t midi_input_pid;

void midiInputThread(void *)
{
  MDevent newMessage;
  int status;

  while (1) {
    mdReceive(inport, &newMessage, 1);
    status = (newMessage.msg[0] & MD_STATUSMASK);

    // Ignore Active Sensing messages
    if (!((status & 0xff) == 0xfe || (status & 0xff) == 0xf8)) {
      midiBuffer[writeOffset] = newMessage;
      writeOffset++;

      if( writeOffset >= MIDI_BUFFER_SIZE )
        writeOffset = 0;
    }
  }
}

MIDIIO :: MIDIIO()
{
  int nports;

  nports = mdInit();
  printf("%d MIDI devices available\n", nports);
  inport = mdOpenInPort(NULL);
  if (inport == NULL) 	{
    fprintf(stderr,"Cannot open MIDI device.\n");
    printf("Exiting MIDIIO Process.\n");
    exit(0);
  }
  mdSetStampMode(inport, MD_NOSTAMP);

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MDevent[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  midi_input_pid = sproc(midiInputThread, PR_SALL);
  if (midi_input_pid == -1)  {
    fprintf(stderr, "unable to create MIDI input thread...aborting.\n");
    exit(0);
  }
}

MIDIIO :: ~MIDIIO()
{
  kill (midi_input_pid, SIGKILL);
  mdClosePort(inport);
  delete [] midiBuffer;
}

int MIDIIO ::  nextMessage()
{
  int status;
  int byte1;
  int byte2;
  MDevent lastEvent;
  static unsigned long long lastTimeStamp = 0;

  if ( readOffset == writeOffset ) return 0;

  lastEvent = midiBuffer[readOffset];

  readOffset++;
  if ( readOffset >= MIDI_BUFFER_SIZE ) readOffset = 0;

  status = (lastEvent.msg[0] & MD_STATUSMASK);
  byte1 = lastEvent.msg[1];
  byte2 = lastEvent.msg[2];
  channel = (lastEvent.msg[0] & MD_CHANNELMASK);

  if ((status == MD_PROGRAMCHANGE) || 
      (status == MD_CHANNELPRESSURE))
    {
      messageType = status;
      byteTwo = (float) byte1;
      deltaTime = (MY_FLOAT) ((lastEvent.stamp - lastTimeStamp) * 0.000000001);
      lastTimeStamp = lastEvent.stamp;
    }
  else if ((status == MD_NOTEON) || (status == MD_NOTEOFF) ||
           (status == MD_CONTROLCHANGE) || (status == MD_POLYKEYPRESSURE))
    {
      messageType = status;
      byteTwo = (float) byte1;
      byteThree = (float) byte2;
      deltaTime = (MY_FLOAT) ((lastEvent.stamp - lastTimeStamp) * 0.000000001);
      lastTimeStamp = lastEvent.stamp;
    }
  else if (status == MD_PITCHBENDCHANGE)
    {
      messageType = status;
      byteTwo = (float) byte1 * NORM_7;
      byteTwo += (float) byte2;
      deltaTime = (MY_FLOAT) ((lastEvent.stamp - lastTimeStamp) * 0.000000001);
      lastTimeStamp = lastEvent.stamp;
    }
  else
    {
      messageType = -1;
    }

  return messageType;
}


#elif defined(__USS_REALTIME_)

/*************************************/
/*         __USS_REALTIME_           */
/*************************************/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/soundcard.h>       
#include <pthread.h>
//#include <pthread/mit/pthread.h>

int _seqfd;

typedef unsigned char byte;

typedef struct { 
    byte data[4]; 
    unsigned long time; 
} MIDIMESSAGE;

MIDIMESSAGE *midiBuffer;

/* System Messages */
#define MIDI_SYSTEM_MSG   ((byte)0xF0)
#define MessageType(MSG)  (byte)((MSG) & ((byte)0xF0))

#define SEQUENCER_PATH "/dev/sequencer"

/* MIDI time code at 100 ticks per second. */
#define OSS_MIDI_CLOCK_RATE 100

pthread_t midi_input_thread;

void *midiInputThread(void *)
{
  byte NumArgs  = 0;
  byte ArgsLeft = 0;
  unsigned long lastTime = 0;
  unsigned long newTime = 0;
  byte InBytes[4];
  static MIDIMESSAGE newMessage;
  int n;

  while (1) {
    if ((n = read(_seqfd, &InBytes, sizeof(InBytes))) == -1) {
      fprintf(stderr,"Error reading " SEQUENCER_PATH "\n");
      exit(0);
    }

    switch ( InBytes[0] )
      {
      case SEQ_WAIT:
        /* MIDI clock ticks ... the first MIDI message deltaTime is calculated
         * with respect to the start of the MIDI clock.
         */
        newTime = ((InBytes[3]<<16)|(InBytes[2]<<8)| InBytes[1]);
        break;

      case SEQ_ECHO:
        /* no echo events yet defined */
#ifdef DEBUG
        fprintf(stderr,"ECHO EVENT\n");
#endif
        break;

      case SEQ_MIDIPUTC:
        /* Determination of a full MIDI message from the input MIDI stream is based
           here on the observation that MIDI status bytes and subsequent data bytes
           are NOT returned in the same read() call.  Rather, they are spread out
           over multiple read() returns, with only a single value per return.  So,
           if we find a status byte, we then determine the number of expected
           operands and process that number of subsequent read()s to determine the
           complete MIDI message. 
           */
        if (InBytes[1] & 0x80) { /* Status Byte */
          if (MessageType(InBytes[1]) == MIDI_SYSTEM_MSG)
            {
              NumArgs = 0; /* no timing info */
#ifdef DEBUG
              fprintf(stderr, "SYSTEM MESSAGE\n");
#endif
            }
          else if (MessageType(InBytes[1]) == MIDI_PGM_CHANGE ||
                   MessageType(InBytes[1]) == MIDI_CHN_PRESSURE) 
            {
              NumArgs = 1;
            }
          else 
            {
              NumArgs = 2;
            }
          newMessage.data[0] = InBytes[1];
          ArgsLeft = NumArgs;
          newMessage.data[1] = 0;
          newMessage.data[2] = 0;
        }

        if (ArgsLeft && !(InBytes[1] & 0x80)) { /* not a status byte */
          if (ArgsLeft == NumArgs)
            newMessage.data[1] = InBytes[1];
          else
            {
              newMessage.data[2] = InBytes[1];
            }

          --ArgsLeft;

          /* If MIDI message complete, then setup for running status mode
             (another event of the same type without status byte).
           */
          if ( !ArgsLeft ) {
            if (MessageType(newMessage.data[0]) == (int) MIDI_PGM_CHANGE ||
                MessageType(newMessage.data[0]) == (int) MIDI_CHN_PRESSURE)
              {
                ArgsLeft = 1;
              }
            else
              {
                ArgsLeft = 2;
              }
            newMessage.time = newTime - lastTime;
            lastTime = newTime;

            // Put newMessage in the circular buffer
            midiBuffer[writeOffset] = newMessage;
            writeOffset++;

            if( writeOffset >= MIDI_BUFFER_SIZE )
              writeOffset = 0;
            break;
          }
        }

      default:
        break;
      }
  }
}

MIDIIO :: MIDIIO()
{
  int err = 0;
  _seqfd = 0;

#ifdef NONBLOCKING_MIDI
  if((_seqfd = open(SEQUENCER_PATH, O_RDONLY+O_NONBLOCK, 0)) == -1)  {
#else
  if((_seqfd = open(SEQUENCER_PATH, O_RDONLY, 0)) == -1)  {
#endif
	fprintf(stderr,"Cannot open " SEQUENCER_PATH ". \n");
	printf("Exiting MIDIIO Process.\n");
	exit(0);
  }

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  err = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (err)
    {
      fprintf(stderr, "Unable to create MIDI input thread.\n");
      printf("Exiting MIDIIO Process.\n");
      exit(0);
    }
}

MIDIIO :: ~MIDIIO()
{
  if (_seqfd != 0) close(_seqfd);
  delete [] midiBuffer;
}

int MIDIIO::nextMessage()
{
  MIDIMESSAGE lastEvent;
  unsigned long micros = 2000;

  if ( readOffset == writeOffset ) return 0;

  lastEvent = midiBuffer[readOffset];

  readOffset++;
  if ( readOffset >= MIDI_BUFFER_SIZE ) readOffset = 0;

  messageType = (int) (lastEvent.data[0] & 0xf0);
  channel = (int) (lastEvent.data[0] & 0x0f);
  byteTwo = (float) lastEvent.data[1];
  if (messageType == (int) MIDI_PITCH_BEND)
    byteTwo = (float) lastEvent.data[2] + (byteTwo * NORM_7);
  else
    byteThree = (float) lastEvent.data[2];
  deltaTime = (float) lastEvent.time / OSS_MIDI_CLOCK_RATE;

  return messageType;
}


#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )

/*************************************/
/*        __WIN_REALTIME_          */
/*************************************/

#include <stdio.h>
#include "MIDIIO.h"

#define MIDI_NOTEON 0x90
#define MIDI_NOTEOFF 0x80
#define MIDI_POLYKEYPRESSURE 0xA0
#define MIDI_CHANNELPRESSURE 0xD0
#define MIDI_PROGRAMCHANGE 0xC0
#define MIDI_CONTROLCHANGE 0xB0
#define MIDI_PITCHBEND 0xE0

typedef struct { 
    DWORD data; 
    DWORD time; 
} MIDIMESSAGE;

MIDIMESSAGE *midiBuffer;

static void CALLBACK midiInputCallback( HMIDIOUT hmin, UINT inputStatus, 
			DWORD instancePtr, DWORD midiMessage, DWORD timestamp)
{
  MIDIMESSAGE newMessage;
			
  switch (inputStatus) {
  case MIM_OPEN:
#ifdef TESTING_MIDI_IN
    printf("MIM_OPEN\n");
#endif
    break;

  case MIM_CLOSE:
#ifdef TESTING_MIDI_IN
    printf("MIM_CLOSE\n");
#endif
    break;

  case MIM_DATA:
#ifdef TESTING_MIDI_IN
    printf("MIM_DATA\n");
#endif
    // Ignore Active Sensing messages
    if ((midiMessage & 0xff) == 0xfe || (midiMessage & 0xff) == 0xf8) {
      break;
    }
    newMessage.data = midiMessage;
    newMessage.time = timestamp;

    // Put newMessage in the circular buffer
    midiBuffer[writeOffset] = newMessage;
    writeOffset++;

    if( writeOffset >= MIDI_BUFFER_SIZE )
      writeOffset = 0;
    break;

  case MIM_ERROR:
    fprintf(stderr,"Invalid MIDI message received!\n");
#ifdef TESTING_MIDI_IN
    printf("MIM_ERROR\n");
#endif
    break;

  case MIM_LONGDATA:
    /* System exclusive buffer is returned */
    break;

  case MIM_LONGERROR:
#ifdef TESTING_MIDI_IN
    printf("MIM_LONGERROR\n");
#endif
    break;

  default:
    break;
  }
}

HMIDIIN hMidiIn ;   // Handle to Midi Output Device

MIDIIO :: MIDIIO()
{
  MMRESULT result;
  UINT uDeviceID;
  MIDIINCAPS deviceCaps;
  UINT i;
  char inputString[128];

  uDeviceID = midiInGetNumDevs();
  printf("%i MIDI Input Devices Available.\n",uDeviceID);
  if (uDeviceID == 0) {
    printf("Exiting MIDIIO Process.\n");
    exit(0);
  }

  for (i=0; i<uDeviceID; i++) {
    result = midiInGetDevCaps(i, &deviceCaps, sizeof(MIDIINCAPS));
    printf("MIDI Device %d is %s.\n", i, deviceCaps.szPname);
  }

  if (uDeviceID > 1) {
    printf("\nType the MIDI Device to open: ");
    fgets(inputString, 128, stdin);
    uDeviceID = (UINT) atoi(inputString);
  }
  else uDeviceID -= 1;

  // Open the port and return any errors	
  result = midiInOpen(&hMidiIn, uDeviceID, (DWORD)&midiInputCallback, (DWORD)NULL, CALLBACK_FUNCTION);
  if (result != MMSYSERR_NOERROR) {
	fprintf(stderr,"Cannot open MIDI Input Device %d!\n", uDeviceID);
	printf("Exiting MIDIIO Process.\n");
	exit(0);
  }

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;
	
  midiInStart( hMidiIn );
}

MIDIIO :: ~MIDIIO()
{
  midiInReset( hMidiIn );
  midiInStop( hMidiIn );
  midiInClose( hMidiIn );
  delete [] midiBuffer;
}

int MIDIIO ::  nextMessage()
{
  int status;
  int byte1;
  int byte2;
  MIDIMESSAGE lastEvent;
  static DWORD lastTime = 0;
  static DWORD newTime = 0;

  if ( readOffset == writeOffset ) return 0;

  lastEvent = midiBuffer[readOffset];
		
  readOffset++;
  if ( readOffset >= MIDI_BUFFER_SIZE ) readOffset = 0;

  status = (int) (lastEvent.data & 0xff);
  byte1 = (int) (lastEvent.data & 0xff00) >> 8;
  byte2 = (int) (lastEvent.data & 0xff0000) >> 16;
  channel = (int) (status & 0x0f);
  newTime = lastEvent.time;
  deltaTime = (float) (newTime - lastTime) * 0.001;
  lastTime = newTime;

  if ((status == MIDI_PROGRAMCHANGE) || 
			(status == MIDI_CHANNELPRESSURE))
		{
			messageType = status;
			byteTwo = (float) byte1;
		}
  else if ((status == MIDI_NOTEON) || (status == MIDI_NOTEOFF) ||
					 (status == MIDI_CONTROLCHANGE) || (status == MIDI_POLYKEYPRESSURE))
		{
			messageType = status;
			byteTwo = (float) byte1;
			byteThree = (float) byte2;
		}
  else if (status == MIDI_PITCHBEND)
		{
			messageType = status;
			byteTwo = (float) (byte1 * NORM_7);
			byteTwo += (float) byte2;
		}
  else
		{
			messageType = -1;
		}

  return messageType;
}

#endif

void MIDIIO :: printMessage()
{
  printf("type = %d, channel = %d, byte2 = %f, byte3 = %f\n",
		 this->getType(), this->getChannel(), this->getByteTwo(),
		 this->getByteThree());
}

int MIDIIO ::  getType()
{
  return messageType;
}

int MIDIIO ::  getChannel()
{
  return channel;
}

MY_FLOAT MIDIIO :: getByteTwo()
{
  return byteTwo;
}

MY_FLOAT MIDIIO :: getByteThree()
{
  return byteThree;
}

MY_FLOAT MIDIIO :: getDeltaTime()
{
  return deltaTime;
}
