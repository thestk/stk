/***************************************************/
/*! \class RtMidi
    \brief STK realtime MIDI class.

    At the moment, this object only handles MIDI
    input, though MIDI output code can go here
    when someone decides they need it (and writes
    it).

    This object opens a MIDI input device and
    parses MIDI messages into a MIDI buffer.  Time
    stamp info is converted to a delta-time
    value. MIDI data is stored as MY_FLOAT to
    conform with SKINI.  System exclusive messages
    are currently ignored.

    An optional argument to the constructor can be
    used to specify a device or card.  When no
    argument is given, a default device is opened.
    If a device argument fails, a list of available
    devices is printed to allow selection by the user.

    This code is based in part on work of Perry
    Cook (SGI), Paul Leonard (Linux), the
    RoseGarden team (Linux), and Bill Putnam
    (Windows).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "RtMidi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MIDI_BUFFER_SIZE 256
int writeIndex;

#if defined(__OS_IRIX__)

#include <pthread.h>
#include <dmedia/midi.h>
#include <sys/types.h>
#include <signal.h>

MDport inport;
MDevent *midiBuffer;
pthread_t midi_input_thread;

void *midiInputThread(void *)
{
  MDevent newMessage;
  int status;

  while (1) {
    mdReceive(inport, &newMessage, 1);
    status = (newMessage.msg[0] & MD_STATUSMASK);

    // Ignore all system messages
    if (status != 0xf0) {
      midiBuffer[writeIndex] = newMessage;
      writeIndex++;

      if( writeIndex >= MIDI_BUFFER_SIZE )
        writeIndex = 0;
    }
  }
}

RtMidi :: RtMidi(int device)
{
  int nports, card;
  char msg[256];
  char *device_name = 0;

  nports = mdInit();
  if (nports < 1) {
    sprintf(msg, "RtMidi: No Irix MIDI device available.");
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  if (device == 0) {
    // Open default MIDI interface.
    inport = mdOpenInPort(NULL);
    if (inport == NULL) {
      sprintf(msg, "RtMidi: Cannot open default Irix MIDI device.");
      handleError(msg, StkError::MIDI_SYSTEM);
    }
  }
  else {
    card = device;
    if ( (card < 0) || (card >= nports) ) {
      printf("\n");
      for (card=0; card<nports; card++) {
        device_name = mdGetName(card);
        printf("MIDI interface %d: %s\n", card, device_name);
      }
      char choice[16];
      card = -1;
      while ( card < 0 || card >= nports ) {
        printf("\nType a MIDI interface number from above: ");
        fgets(choice, 16, stdin);
        card = atoi(choice);
      }
      printf("\n");
    }
    inport = mdOpenInPort(mdGetName(card));
    if (inport == NULL) {
      sprintf(msg, "RtMidi: Cannot open Irix MIDI interface %d!", card);
      handleError(msg, StkError::MIDI_SYSTEM);
    }
  }

  mdSetStampMode(inport, MD_NOSTAMP);

  // Set up the circular buffer for the Midi input messages.
  midiBuffer = new MDevent[MIDI_BUFFER_SIZE];
  readIndex = 0;
  writeIndex = 0;

  if ( pthread_create(&midi_input_thread, NULL, midiInputThread, NULL) ) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.");
    handleError(msg, StkError::PROCESS_THREAD);
  }
}

RtMidi :: ~RtMidi()
{
  pthread_cancel(midi_input_thread);
  pthread_join(midi_input_thread, NULL);
  mdClosePort(inport);
  delete [] midiBuffer;
}

int RtMidi ::  nextMessage()
{
  int status;
  int byte1;
  int byte2;
  MDevent lastEvent;
  static unsigned long long lastTimeStamp = 0;

  if ( readIndex == writeIndex ) return 0;

  lastEvent = midiBuffer[readIndex];

  readIndex++;
  if ( readIndex >= MIDI_BUFFER_SIZE ) readIndex = 0;

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
      byteTwo = (float) byte1 * ONE_OVER_128;
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


#elif ( defined(__LINUX_OSS__) || defined(__LINUX_ALSA__) )

#include <pthread.h>
#include <sys/time.h>

#if defined(__MIDIATOR__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
int midi_in;

#elif defined(__LINUX_OSS__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/soundcard.h>
#include <errno.h>
int midi_in;

#else // __LINUX_ALSA__

#include <alsa/asoundlib.h>
snd_rawmidi_t *midi_in = 0;

#endif

typedef unsigned char byte;

/* MIDI System Messages */
#define MD_SYSTEM_MSG   ((byte)0xF0)
#define MD_PGM_CHANGE   ((byte)0xC0)
#define MD_CHN_PRESSURE ((byte)0xD0)
#define MD_PITCH_BEND		((byte)0xE0)
#define MessageType(MSG)  (byte)((MSG) & ((byte)0xF0))

typedef struct { 
    byte data[3]; 
    float delta_time; 
} MIDIMESSAGE;

MIDIMESSAGE *midiBuffer;

pthread_t midi_input_thread;

void *midiInputThread(void *)
{
  int numArgs = 2, argsLeft = 0;
  double lastTime = 0.0, newTime = 0.0;
  byte newByte;
  MIDIMESSAGE newMessage;
  int n;
  struct timeval tv;

  (void)gettimeofday(&tv, (struct timezone *)NULL);
  lastTime = (double) (tv.tv_sec + (tv.tv_usec * 0.000001));

  for (;;) {

#if defined(__LINUX_OSS__) || defined(__MIDIATOR__)

    // Normally, you should check the return value of this read() call.
    // A return of -1 usually indicates an error.  However, for OSS
    // compatability in ALSA, we need to ignore such values.

    n = read(midi_in, &newByte, 1);

#else // ALSA_API

    if ((n = snd_rawmidi_read(midi_in, &newByte, 1)) == -1)
      fprintf(stderr, "RtMidi: Error reading ALSA raw MIDI data from device!\n");

#endif

    while (n > 0) {
      if (newByte & 0x80) { // status byte
        if (MessageType(newByte) == MD_SYSTEM_MSG) {
          n--;
          continue;
        }
        else if (MessageType(newByte) == MD_PGM_CHANGE ||
                 MessageType(newByte) == MD_CHN_PRESSURE) {
          numArgs = 1;
        }
        else {
          numArgs = 2;
        }
        newMessage.data[0] = newByte;
        newMessage.data[1] = 0;
        newMessage.data[2] = 0;
        argsLeft = numArgs;
      }
      else { // data byte
        if ( argsLeft == numArgs )
          newMessage.data[1] = newByte;
        else {
          newMessage.data[2] = newByte;
        }
        argsLeft--;
      
        if ( !argsLeft ) { // MIDI message complete      
          // setup for running status mode (another event of the
          // same type without status byte)
          if (MessageType(newMessage.data[0]) == (int) MD_PGM_CHANGE ||
              MessageType(newMessage.data[0]) == (int) MD_CHN_PRESSURE) {
            argsLeft = 1;
          }
          else {
            argsLeft = 2;
          }

          // determine the delta time since the last event
          (void)gettimeofday(&tv, (struct timezone *)NULL);
          newTime = (double) ((double)tv.tv_sec + (((double)tv.tv_usec) * 0.000001));
          newMessage.delta_time = (float) (newTime - lastTime);
          lastTime = newTime;

          // Put newMessage in the circular buffer
          midiBuffer[writeIndex] = newMessage;
          writeIndex++;

          if( writeIndex >= MIDI_BUFFER_SIZE )
            writeIndex = 0;
        }
      }
      n--;
    }
  }
  return 0;
}



#if defined(__MIDIATOR__)

/*
  Hopefully, this special support for the MIDIator serial
  port MIDI device is temporary and it will eventually be
  incorporated into the OSS and ALSA APIs.

  This code is based almost entirely on David Topper's 
  driver code, which is available from:

    ftp://presto.music.virginia.edu/pub/midiator

  See the README-Linux STK document for details on how to
  get the MIDIator setup correctly for use under linux.
*/

#include <termio.h>
#include <linux/serial_reg.h>

#define BAUD_RATE B19200
#define MAX_DEVICES 3
#define MIDI_NAME "/dev/ttyS"

void initializeMidiator();

RtMidi :: RtMidi(int device)
{
  char msg[256];
  char name[16];
  char deviceNames[MAX_DEVICES][16];
  midi_in = 0;

  int i, nDevices = 0;
  for ( i=0; i<MAX_DEVICES; i++ ) {
    sprintf(name, "%s%d", MIDI_NAME, i);
    midi_in = open(name, O_RDONLY | O_NONBLOCK, 0);
    if ( midi_in != -1 ) {
      strncpy( deviceNames[nDevices++], name, 16 );
      close( midi_in );
    }
    else if ( errno == EBUSY )
      fprintf(stderr,"RtMidi: Serial port (%s) is busy and cannot be opened.\n", name);
  }

  if (nDevices == 0) {
    sprintf(msg, "RtMidi: no serial ports available.");
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Check device argument and print list if necessary.
  int deveyes;
  if ( device >= 0 && device < nDevices )
    deveyes = device;
  else if ( nDevices == 1 )
    deveyes = 0;
  else {
    // Invalid device argument ... print list.
    printf("\n");
    for ( i=0; i<nDevices; i++ )
      printf("Serial Port %d: %s\n", i, deviceNames[i]);

    char choice[16];
    deveyes = -1;
    while ( deveyes < 0 || deveyes >= nDevices ) {
      printf("\nType a MIDI device number from above: ");
      fgets(choice, 16, stdin);
      deveyes = atoi(choice);
    }
    printf("\n");
  }

  midi_in = open(deviceNames[deveyes], O_RDONLY, 0);
  if ( midi_in == -1)  {
    sprintf(msg, "RtMidi: Unable to open serial port (%s) for MIDI input!",
            deviceNames[deveyes]);
    handleError(msg, StkError::MIDI_SYSTEM);
  }

	printf("\nInitializing MIDIator MS-124w ... ");
  initializeMidiator();
  printf("ready on serial port %s.\n", deviceNames[deveyes]);

  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readIndex = 0;
  writeIndex = 0;

  int result = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (result) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.");
    handleError(msg, StkError::PROCESS_THREAD);
  }
}

#elif defined(__LINUX_OSS__) // normal OSS setup

#define MAX_DEVICES 8
#define MIDI_NAME "/dev/midi"

RtMidi :: RtMidi(int device)
{
  char msg[256];
  char name[16];
  char deviceNames[MAX_DEVICES][16];
  midi_in = 0;

  // /dev/midi should be a link to the default midi device under OSS
  strcpy(name, MIDI_NAME);

  // The OSS API doesn't really give us a means for probing the
  // capabilities of devices.  Thus, we'll just pursue a brute
  // force method of opening devices until we either find something
  // that doesn't complain or we have to give up.  We'll start with
  // the default device, then try /dev/midi00, /dev/midi01, etc...
  int i, nDevices = 0;
  for ( i=0; i<MAX_DEVICES; i++ ) {
    if (i > 0) sprintf(name, "%s%d%d", MIDI_NAME, 0, i-1);
    midi_in = open(name, O_RDONLY | O_NONBLOCK, 0);
    if ( midi_in != -1 ) {
      strncpy( deviceNames[nDevices++], name, 16 );
      close( midi_in );
    }
    else if ( errno == EBUSY )
      fprintf(stderr,"RtMidi: MIDI device (%s) is busy and cannot be opened.\n", name);
  }

  if (nDevices == 0) {
    sprintf(msg, "RtMidi: no OSS MIDI cards reported available.");
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Check device argument and print list if necessary.
  int deveyes;
  if ( device >= 0 && device < nDevices )
    deveyes = device;
  else if ( nDevices == 1 )
    deveyes = 0;
  else {
    // Invalid device argument ... print list.
    printf("\n");
    for ( i=0; i<nDevices; i++ )
      printf("MIDI Device %d: %s\n", i, deviceNames[i]);

    char choice[16];
    deveyes = -1;
    while ( deveyes < 0 || deveyes >= nDevices ) {
      printf("\nType a MIDI device number from above: ");
      fgets(choice, 16, stdin);
      deveyes = atoi(choice);
    }
    printf("\n");
  }

  midi_in = open(deviceNames[deveyes], O_RDONLY, 0);
  if ( midi_in == -1)  {
    sprintf(msg, "RtMidi: Unable to open OSS device (%s) for MIDI input!",
            deviceNames[deveyes]);
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readIndex = 0;
  writeIndex = 0;

  int result = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (result) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.");
    handleError(msg, StkError::PROCESS_THREAD);
  }
}

#else // ALSA_API

#define MAX_DEVICES 8

RtMidi :: RtMidi(int device)
{
  midi_in = 0;
  char msg[256];
  int result, card, deveyes, nDevices;
  char name[32];
  char deviceNames[MAX_DEVICES][32];
  snd_ctl_t *handle;
  snd_ctl_card_info_t *info;
  snd_ctl_card_info_alloca(&info);

  // Count cards and devices
  card = -1;
  nDevices = 0;
  snd_card_next(&card);
  while ( card >= 0 ) {
    sprintf(name, "hw:%d", card);
    result = snd_ctl_open(&handle, name, 0);
    if (result < 0) {
      sprintf(msg, "RtMidi: ALSA control open (%i): %s.", card, snd_strerror(result));
      handleError(msg, StkError::WARNING);
      goto next_card;
		}
    result = snd_ctl_card_info(handle, info);
		if (result < 0) {
      sprintf(msg, "RtMidi: ALSA control hardware info (%i): %s.", card, snd_strerror(result));
      handleError(msg, StkError::WARNING);
      goto next_card;
		}
		deveyes = -1;
		while (1) {
      result = snd_ctl_rawmidi_next_device(handle, &deveyes);
			if (result < 0) {
        sprintf(msg, "RtMidi: ALSA control next rawmidi device (%i): %s.", card, snd_strerror(result));
        handleError(msg, StkError::WARNING);
        break;
      }
			if (deveyes < 0)
        break;
      sprintf( deviceNames[nDevices++], "hw:%d,%d", card, deveyes );
      if ( nDevices > MAX_DEVICES ) break;
    }
    if ( nDevices > MAX_DEVICES ) break;
  next_card:
    snd_ctl_close(handle);
    snd_card_next(&card);
  }

  if (nDevices == 0) {
    sprintf(msg, "RtMidi: no ALSA MIDI cards reported available.");
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Check device argument and print list if necessary.
  if ( device >= 0 && device < nDevices )
    deveyes = device;
  else if ( nDevices == 1 )
    deveyes = 0;
  else {
    // Invalid device argument ... print list.
    printf("\n");
    for ( int i=0; i<nDevices; i++ )
      printf("MIDI Device %d: %s\n", i, deviceNames[i]);

    char choice[16];
    deveyes = -1;
    while ( deveyes < 0 || deveyes >= nDevices ) {
      printf("\nType a MIDI device number from above: ");
      fgets(choice, 16, stdin);
      deveyes = atoi(choice);
    }
    printf("\n");
  }

  result = snd_rawmidi_open(&midi_in, NULL, deviceNames[deveyes], 0);
  if (result) {
    sprintf(msg, "RtMidi: Error opening ALSA raw MIDI device: %s.", deviceNames[deveyes]);
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readIndex = 0;
  writeIndex = 0;

  result = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (result) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.");
    handleError(msg, StkError::PROCESS_THREAD);
  }
}

#endif

RtMidi :: ~RtMidi()
{
  pthread_cancel(midi_input_thread);
  delete [] midiBuffer;

#if defined(__MIDIATOR__)
  tcdrain(midi_in);
  if (midi_in != 0) close(midi_in);
#elif defined(__LINUX_OSS__)
  if (midi_in != 0) close(midi_in);
#else // ALSA_API
  if (midi_in != 0)
    snd_rawmidi_close(midi_in);
#endif
}

int RtMidi::nextMessage()
{
  MIDIMESSAGE lastEvent;

  if ( readIndex == writeIndex ) return 0;

  lastEvent = midiBuffer[readIndex];

  readIndex++;
  if ( readIndex >= MIDI_BUFFER_SIZE ) readIndex = 0;

  messageType = (int) (lastEvent.data[0] & 0xf0);
  channel = (int) (lastEvent.data[0] & 0x0f);
  byteTwo = (float) lastEvent.data[1];
  if (messageType == (int) MD_PITCH_BEND)
    byteTwo = (float) lastEvent.data[2] + (byteTwo / 128.0);
  else
    byteThree = (float) lastEvent.data[2];
  deltaTime = (float) lastEvent.delta_time;

  return messageType;
}

#if defined(__MIDIATOR__)

void initializeMidiator()
{
  struct termios info;	// serial port configuration info
  int status;         // serial port status
  struct timeval tv;   // to do a little time delay

  // Get the current serial port attributes, so we can change
  // the ones we care about.
  if (tcgetattr(midi_in, &info) < 0) {
    fprintf(stderr, "RtMidi: ioctl to get tty info failed (MIDIator support)!");
    return;
  }

  bzero(&info, sizeof(info));
  info.c_cflag = BAUD_RATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  info.c_iflag &= ~IGNCR;
  info.c_oflag &= ~IGNCR;
	
  // set input mode (non-canonical, no echo,...)
  info.c_lflag = 0;
	
  info.c_cc[VTIME]    = 1;   // inter-character timer unused
  info.c_cc[VMIN]     = 1;   // blocking read until 5 chars received
	
  tcflush(midi_in, TCIFLUSH);

  // Set the attributes
  tcsetattr(midi_in, TCSANOW, &info);

  // Startup sequence, as per ron@MIDI_DEV's instructions
  // Many thanks to Ron for supporting Linux

  // Step 1
  // Power down and deassert DTR and RTS
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TIOCM_DTR;
  status &= ~TIOCM_RTS;
  ioctl(midi_in, TIOCMSET, status);
  // Wait 600 ms to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 600000;
  select(0, NULL, NULL, NULL, &tv);

  // Step 2
  // Power up and assert break
  ioctl(midi_in, TIOCMGET, &status);
  status |= TCSBRK;
  ioctl(midi_in, TIOCMSET, status);
  // Wait 300 ms to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 300000;
  select(0, NULL, NULL, NULL, &tv);

  // Step 3
  // Set input mode
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TIOCM_DTR;
  status |= TIOCM_RTS;
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

  // Step 4
  // Set input mode
  ioctl(midi_in, TIOCMGET, &status);
  status |= TIOCM_DTR;
  status &= TIOCM_RTS;
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

  // Step 5
  // Set output mode
  // Bitval = RTS, clock = DTR

  // 1
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TIOCM_DTR; /* 0 */
  status |= TIOCM_RTS; /* 1 */
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);
  //
  ioctl(midi_in, TIOCMGET, &status);
  status |= TIOCM_DTR; // 1 rising edge clock
  status |= TIOCM_RTS; // 1
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

  // 1
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TIOCM_DTR; // 0
  status |= TIOCM_RTS;  // 1
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);
  //
  ioctl(midi_in, TIOCMGET, &status);
  status |= TIOCM_DTR; // 1 rising edge clock
  status |= TIOCM_RTS; // 1
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

	// 0
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TIOCM_DTR; // 0
  status &= ~TIOCM_RTS; // 0
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);
  //
  ioctl(midi_in, TIOCMGET, &status);
  status |= TIOCM_DTR;  // 1 rising edge clock
  status &= ~TIOCM_RTS; // 0
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

  // Step 6 ... necessary ?
  // Set RTS=0,DTR=1 ... but they already are from previous ^
  //
  ioctl(midi_in, TIOCMGET, &status);
  status |= TIOCM_DTR;  // 1 rising edge clock
  status &= ~TIOCM_RTS; // 0
  ioctl(midi_in, TIOCMSET, status);
  // Wait 40 us to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 40;
  select(0, NULL, NULL, NULL, &tv);

  // Step 7
  // Deassert break
  ioctl(midi_in, TIOCMGET, &status);
  status &= ~TCSBRK;
  ioctl(midi_in, TIOCMSET, status);
  // Wait 100 ms to make sure everything is stable
  tv.tv_sec = 0;
  tv.tv_usec = 100000;
  select(0, NULL, NULL, NULL, &tv);
  //  End Midiator startup sequence -- midi_dev_type = MIDIATOR
}
#endif // MIDIator

#elif defined(__OS_WINDOWS__)

#include <windows.h>
#include <mmsystem.h>

static void CALLBACK midiInputCallback( HMIDIOUT hmin, UINT inputStatus, 
          DWORD instancePtr, DWORD midiMessage, DWORD timestamp);

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

  case MIM_DATA:

    // Ignore Active Sensing messages
    if ((midiMessage & 0xff) == 0xfe || (midiMessage & 0xff) == 0xf8) {
      break;
    }
    newMessage.data = midiMessage;
    newMessage.time = timestamp;

    // Put newMessage in the circular buffer
    midiBuffer[writeIndex] = newMessage;
    writeIndex++;

    if( writeIndex >= MIDI_BUFFER_SIZE )
      writeIndex = 0;
    break;

  default:
    break;
  }
}

HMIDIIN hMidiIn ;   // Handle to Midi Input Device

RtMidi :: RtMidi(int device)
{
  MMRESULT result;
  UINT uDeviceID;
  int deveyes;
  MIDIINCAPS deviceCaps;
  UINT i;
  char msg[256];

  uDeviceID = midiInGetNumDevs();
  if (uDeviceID < 1) {
    sprintf(msg, "RtMidi: No windoze MIDI device available.");
    handleError(msg, StkError::MIDI_SYSTEM);
  }

  // Our normal scheme is to use the default device if no argument
  // is supplied to RtMidi() or if the argument = 0.  However,
  // there is no way to specify a default MIDI device under windoze.
  // So, I'm going to print the list if device is not a valid identifier.
  if ( device >= 0 && device < (int)uDeviceID ) {
		// try to open device specified as argument
		result = midiInOpen(&hMidiIn, device,
												(DWORD)&midiInputCallback,
												(DWORD)NULL,
												CALLBACK_FUNCTION);
		if (result == MMSYSERR_NOERROR)
      goto have_good_device;
  }

  printf("\nMIDI input interfaces available: %i\n", uDeviceID);
  for (i=0; i<uDeviceID; i++) {
    result = midiInGetDevCaps(i, &deviceCaps, sizeof(MIDIINCAPS));
    printf("   MIDI interface %d is %s\n", i, deviceCaps.szPname);
  }

  deveyes = -1;
  if (uDeviceID > 1) {
    char choice[16];
    while ( deveyes < 0 || deveyes >= (int)uDeviceID ) {
      printf("\nType a MIDI device number from above: ");
      fgets(choice, 16, stdin);
      deveyes = atoi(choice);
    }
  }
  else deveyes = 0;

  // Open the port and return any errors	
  result = midiInOpen(&hMidiIn, (UINT)deveyes,
											(DWORD)&midiInputCallback,
											(DWORD)NULL,
											CALLBACK_FUNCTION);
  if (result != MMSYSERR_NOERROR) {
    sprintf(msg, "RtMidi: Cannot open Windoze MIDI interface %d.", deveyes);
    handleError(msg, StkError::MIDI_SYSTEM);
  }

 have_good_device: // the current device is what we will use

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readIndex = 0;
  writeIndex = 0;
	
  midiInStart( hMidiIn );
}

RtMidi :: ~RtMidi()
{
  midiInReset( hMidiIn );
  midiInStop( hMidiIn );
  midiInClose( hMidiIn );
  delete [] midiBuffer;
}

int RtMidi ::  nextMessage()
{
  int status;
  int byte1;
  int byte2;
  MIDIMESSAGE lastEvent;
  static DWORD lastTime = 0;
  static DWORD newTime = 0;

  if ( readIndex == writeIndex ) return 0;

  lastEvent = midiBuffer[readIndex];
		
  readIndex++;
  if ( readIndex >= MIDI_BUFFER_SIZE ) readIndex = 0;

  status = (int) (lastEvent.data & 0xff);
  byte1 = (int) (lastEvent.data & 0xff00) >> 8;
  byte2 = (int) (lastEvent.data & 0xff0000) >> 16;
  channel = (int) (status & 0x0f);
  status &= 0xf0;     // Clear lower byte of status
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
			byteTwo = (float) (byte1 * ONE_OVER_128);
			byteTwo += (float) byte2;
		}
  else
		{
			messageType = -1;
		}

  return messageType;
}

#endif

void RtMidi :: printMessage() const
{
  printf("type = %d, channel = %d, byte2 = %f, byte3 = %f\n",
		 this->getType(), this->getChannel(), this->getByteTwo(),
		 this->getByteThree());
}

int RtMidi ::  getType() const
{
  return messageType;
}

int RtMidi ::  getChannel() const
{
  return channel;
}

MY_FLOAT RtMidi :: getByteTwo() const
{
  return byteTwo;
}

MY_FLOAT RtMidi :: getByteThree() const
{
  return byteThree;
}

MY_FLOAT RtMidi :: getDeltaTime() const
{
  return deltaTime;
}

