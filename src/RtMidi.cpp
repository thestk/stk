/******************************************/
/*
  RtMidi.cpp
  Realtime MIDI I/O Object for STK,
  by Gary P. Scavone, 1998-2000.
  Based in part on code by Perry
  Cook (SGI), Paul Leonard (Linux),
  the RoseGarden team (Linux), and
  Bill Putnam (Win95/NT).

  At the moment, this object only
  handles MIDI input, though MIDI
  output code can go here when someone
  decides they need it (and writes it).

  This object opens a MIDI input device
  and parses MIDI messages into a MIDI
  buffer.  Time stamp info is converted
  to deltaTime. MIDI data is stored as
  MY_FLOAT to conform with SKINI.

  An optional argument to the constructor
  can be used to specify a device or card.
  When no argument is given, a default
  device is opened or a list of available
  devices is printed to allow selection
  by the user.
*/
/******************************************/

#include "RtMidi.h"

#if defined(__STK_REALTIME_)

#define MIDI_BUFFER_SIZE 1024
int writeOffset;
int readOffset;

#if defined(__OS_IRIX_)

/*************************************/
/*         SGI MIDI INPUT            */
/*************************************/

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
      midiBuffer[writeOffset] = newMessage;
      writeOffset++;

      if( writeOffset >= MIDI_BUFFER_SIZE )
        writeOffset = 0;
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
    sprintf(msg, "RtMidi: No SGI MIDI device available.\n");
    throw StkError(msg, StkError::MIDICARD_NOT_FOUND);
  }

  if (device == -1) {
    // open default MIDI interface
    inport = mdOpenInPort(NULL);
    if (inport == NULL) {
      sprintf(msg, "RtMidi: Cannot open default SGI MIDI device.\n");
      throw StkError(msg, StkError::MIDICARD_CONTROL);
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
      printf("\nType a MIDI interface number from above: ");
      fgets(choice, 16, stdin);
      card = atoi(choice);
      printf("\n");
    }
    inport = mdOpenInPort(mdGetName(card));
    if (inport == NULL) {
      sprintf(msg, "RtMidi: Cannot open SGI MIDI interface %d.\n",
              card);
      throw StkError(msg, StkError::MIDICARD_CONTROL);
    }
  }

  mdSetStampMode(inport, MD_NOSTAMP);

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MDevent[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  if (pthread_create(&midi_input_thread, NULL, midiInputThread, NULL)) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
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


#elif ( defined(__OSS_API_) || defined(__ALSA_API_) )

/*************************************/
/*      OSS & ALSA MIDI INPUT        */
/*************************************/

#include <pthread.h>
#include <sys/time.h>

#if defined(__OSS_API_)

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/soundcard.h>
#include <errno.h>
int midi_in;

#else // ALSA_API

#include <sys/asoundlib.h>
snd_rawmidi_t *midi_in = 0;

#endif

typedef unsigned char byte;

/* MIDI System Messages */
#define MD_SYSTEM_MSG   ((byte)0xF0)
#define MD_PGM_CHANGE   ((byte)0xC0)
#define MD_CHN_PRESSURE   ((byte)0xD0)
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

#if defined(__OSS_API_)

    // Normally, you should check the return value of this read() call.
    // A return of -1 usually indicates an error.  However, for OSS
    // compatability in ALSA, we need to ignore such values.

    n = read(midi_in, &newByte, 1);

#else // ALSA_API

    if ((n = snd_rawmidi_read(midi_in, &newByte, 1)) == -1) {
      char msg[256];
      sprintf(msg, "RtMidi: Error reading ALSA raw MIDI device.\n");
      throw StkError(msg, StkError::MIDICARD_CAPS);
    }

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
          midiBuffer[writeOffset] = newMessage;
          writeOffset++;

          if( writeOffset >= MIDI_BUFFER_SIZE )
            writeOffset = 0;
        }
      }
      n--;
    }
  }
}

#if defined(__OSS_API_)

#if defined(__MIDIATOR_)

/* Hopefully, this special support for the MIDIator serial
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
#define MAX_MIDI_DEVS 3
#define MIDI_NAME "/dev/ttyS"

void initializeMidiator();

RtMidi :: RtMidi(int device)
{
  int card = 0, err = 0, nChoices = 0;
  char msg[256];
  char device_name[16];
  bool print_list = FALSE;
  midi_in = 0;

  // /dev/midi should be a link to the default midi device under OSS
  strcpy(device_name, MIDI_NAME);

  // The OSS API doesn't really give us a means for probing the
  // capabilities of devices.  Thus, we'll just pursue a brute
  // force method of opening devices until we either find something
  // that doesn't complain or we have to give up.  We'll start with
  // the default device, then try /dev/midi00, /dev/midi01, etc...

  if (device != -1) {
    // check device specified as argument
    sprintf(device_name, "%s%d", MIDI_NAME, device);
    // try to open this device
    if((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      // Open device failed ... either busy or doesn't exist
      print_list = TRUE;
      printf("\n");
      card = 0;
    }
    else {
      goto have_good_device;
    }
  }

  while (card < MAX_MIDI_DEVS) {
    // if the default device doesn't work, try some others
    sprintf(device_name, "%s%d", MIDI_NAME, card);

    if ((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      // Open device failed ... either busy or doesn't exist
      if (errno == EBUSY && print_list == FALSE)
        fprintf(stderr,"RtMidi: Serial device (%s) is busy and cannot be opened.\n",
                device_name);
      card++;
      continue;
    }

    if (print_list) {
      close(midi_in);
      printf("Serial Port %d: %s\n", card, device_name);
      nChoices++;
      card++;
      continue;
    }

    // This device appears to be OK
    goto have_good_device;
  }

  if (print_list && nChoices) {
    char choice[16];
    printf("\nType a serial port number from above: ");
    fgets(choice, 16, stdin);
    card = atoi(choice);
    printf("\n");
    sprintf(device_name, "%s%d", MIDI_NAME, card);
    if ((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      sprintf(msg, "RtMidi: Unable to open serial port (%s) for MIDI input!\n",
              device_name);
      throw StkError(msg, StkError::MIDICARD_CONTROL);
    }
    goto have_good_device;
  }

  // If we got here, no device was found to meet the requested functionality
  sprintf(msg, "RtMidi: could not open any serial ports for MIDI input!\n");
  throw StkError(msg, StkError::MIDICARD_CAPS);

 have_good_device: // the current device is what we will use

	printf("\nInitializing MIDIator MS-124w ... ");
  initializeMidiator();
  printf("ready on serial port %s.\n",device_name);

  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  err = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (err) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
  }
}

#else // normal OSS stuff

#define MAX_MIDI_DEVS 8
#define MIDI_NAME "/dev/midi"

RtMidi :: RtMidi(int device)
{
  int card = 0, err = 0, nChoices = 0;
  midi_in = 0;
  char msg[256];
  char device_name[16];
  bool print_list = FALSE;

  // /dev/midi should be a link to the default midi device under OSS
  strcpy(device_name, MIDI_NAME);

  // The OSS API doesn't really give us a means for probing the
  // capabilities of devices.  Thus, we'll just pursue a brute
  // force method of opening devices until we either find something
  // that doesn't complain or we have to give up.  We'll start with
  // the default device, then try /dev/midi00, /dev/midi01, etc...

  if (device != -1) {
    // check device specified as argument
    sprintf(device_name, "%s%d%d", MIDI_NAME, 0, device);
    // try to open this device
    if((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      // Open device failed ... either busy or doesn't exist
      print_list = TRUE;
      printf("\n");
      card = 1;
    }
    else {
      goto have_good_device;
    }
  }

  while (card < MAX_MIDI_DEVS) {
    // if the default device doesn't work, try some others
    if (card > 0) sprintf(device_name, "%s%d%d", MIDI_NAME, 0, card-1);

    if ((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      // Open device failed ... either busy or doesn't exist
      if (errno == EBUSY && print_list == FALSE)
        fprintf(stderr,"RtMidi: OSS MIDI device (%s) is busy and cannot be opened.\n",
                device_name);
      card++;
      continue;
    }

    if (print_list) {
      close(midi_in);
      printf("MIDI Card %d: %s\n", card-1, device_name);
      nChoices++;
      card++;
      continue;
    }

    // This device appears to be OK
    goto have_good_device;
  }

  if (print_list && nChoices) {
    char choice[16];
    printf("\nType a MIDI card number from above: ");
    fgets(choice, 16, stdin);
    card = atoi(choice);
    printf("\n");
    sprintf(device_name, "%s%d%d", MIDI_NAME, 0, card);
    if ((midi_in = open(device_name, O_RDONLY, 0)) == -1)  {
      sprintf(msg, "RtMidi: Unable to open OSS device (%s) for MIDI input!\n",
              device_name);
      throw StkError(msg, StkError::MIDICARD_CONTROL);
    }
    goto have_good_device;
  }

  // If we got here, no device was found to meet the requested functionality
  sprintf(msg, "RtMidi: no OSS device found for MIDI input!\n");
  throw StkError(msg, StkError::MIDICARD_CAPS);

 have_good_device: // the current device is what we will use

  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  err = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (err) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
  }
}

#endif // end of normal OSS section

#else // ALSA_API

RtMidi :: RtMidi(int device)
{
  int err = 0, nChoices = 0;
  midi_in = 0;
  char msg[256];
  int card, dev;
  int default_card;
  bool print_list = FALSE;
  unsigned int mask;
  snd_ctl_t *chandle;
  struct snd_ctl_hw_info info;
	snd_rawmidi_info_t midiinfo;

  mask = snd_cards_mask();
  if (!mask) {
    sprintf(msg, "RtMidi: no ALSA sound/MIDI cards reported available.\n");
    throw StkError(msg, StkError::MIDICARD_NOT_FOUND);
  }

  if (device == -1) {
    default_card = snd_defaults_rawmidi_card();
  }
  else { // check device specified as argument
    if (!(mask & (1<<device))) {
      default_card = 0;
      print_list = TRUE;
      printf("\n");
    }
    else {
      default_card = device;
    }
  }

  card = default_card; // start with default card
  while (card<SND_CARDS) {
    if (mask & (1<<card)) {
      if ((err = snd_ctl_open(&chandle, card)) < 0) {
        fprintf(stderr,"RtMidi: ALSA error on control open (%d): %s\n",
                card, snd_strerror(err));
        continue;
      }
      if ((err = snd_ctl_hw_info(chandle, &info)) < 0) {
        fprintf(stderr,"RtMidi: ALSA error on control hardware info (%d): %s\n",
                card, snd_strerror(err));
        snd_ctl_close(chandle);
        continue;
      }
      for (dev=0; dev<(int)info.mididevs; dev++) {
        /* get information for each device */
        if ((err = snd_ctl_rawmidi_info(chandle, dev, &midiinfo)) < 0) {
          fprintf(stderr,"RtMidi: ALSA error on control MIDI info (%d): %s\n",
                  card, snd_strerror(err));
          continue;
        }
        if (midiinfo.flags & SND_RAWMIDI_INFO_INPUT) {
          // this device can handle MIDI input
          if (print_list) {
            printf("MIDI Card %d, Device %d: %s\n", card, dev, info.name);
            nChoices++;
            continue;
          }
          goto have_good_device;
        }
        else { // this device wont' work
          continue;
        }
      }
    }
    if (default_card == 0) card++;
    else { // default card != 0, now start with card 0 and keep searching
      if (card == default_card) card = 0; // first time only
      else {
        card++;
        if (card == default_card) card++; // skip over default card
      }
    }
  }

  if (print_list && nChoices) {
    char choice[16];
    printf("\nType a MIDI card number from above: ");
    fgets(choice, 16, stdin);
    card = atoi(choice);
    printf("Select a device for the same card: ");
    fgets(choice, 16, stdin);
    printf("\n");
    dev = atoi(choice);
    goto have_good_device;
  }

  // if we got here, no devices were found to meet the requested functionality
  sprintf(msg, "RtMidi: no ALSA device found for MIDI input!\n");
  throw StkError(msg, StkError::MIDICARD_CAPS);

 have_good_device: // the current value of card and dev are what we will use

  err = snd_rawmidi_open(&midi_in, card, dev, O_RDONLY);
  if (err) {
    sprintf(msg, "RtMidi: Error opening ALSA raw MIDI device: card %d, device %d.\n",
            card, dev);
    throw StkError(msg, StkError::MIDICARD_CONTROL);
  }


  // Set up the circular buffer for the MIDI input messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;

  err = pthread_create(&midi_input_thread, NULL, midiInputThread, NULL);
  if (err) {
    sprintf(msg, "RtMidi: unable to create MIDI input thread.\n");
    throw StkError(msg, StkError::PROCESS_THREAD);
  }
}

#endif

RtMidi :: ~RtMidi()
{
  pthread_cancel(midi_input_thread);
  delete [] midiBuffer;
#if defined(__OSS_API_)
  #if defined(__MIDIATOR_)
    tcdrain(midi_in);
  #endif
  if (midi_in != 0) close(midi_in);
#else // ALSA_API
  if (midi_in != 0)
    snd_rawmidi_close(midi_in);
#endif
}

int RtMidi::nextMessage()
{
  MIDIMESSAGE lastEvent;

  if ( readOffset == writeOffset ) return 0;

  lastEvent = midiBuffer[readOffset];

  readOffset++;
  if ( readOffset >= MIDI_BUFFER_SIZE ) readOffset = 0;

  messageType = (int) (lastEvent.data[0] & 0xf0);
  channel = (int) (lastEvent.data[0] & 0x0f);
  byteTwo = (float) lastEvent.data[1];
  if (messageType == (int) MD_PITCH_BEND)
    byteTwo = (float) lastEvent.data[2] + (byteTwo * NORM_7);
  else
    byteThree = (float) lastEvent.data[2];
  deltaTime = (float) lastEvent.delta_time;

  return messageType;
}

#if defined(__MIDIATOR_)

void initializeMidiator()
{
  struct termios info;	/* serial port configuration info */
  int status; /* Serial port status */
  struct timeval tv;	/* to do a little time delay */
  char msg[256];

	/* Get the current serial port attributes, so we can change
	 * the ones we care about.
   */
	if (tcgetattr(midi_in, &info) < 0) {
    sprintf(msg, "RtMidi: ioctl to get tty info failed (MIDIator support)!\n");
    throw StkError(msg, StkError::MIDICARD_CAPS);
	}

	bzero(&info, sizeof(info));
	info.c_cflag = BAUD_RATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	info.c_iflag &= ~IGNCR;
	info.c_oflag &= ~IGNCR;
	
	/* set input mode (non-canonical, no echo,...) */
	info.c_lflag = 0;
	
	info.c_cc[VTIME]    = 1;   /* inter-character timer unused */
	info.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */
	
	tcflush(midi_in, TCIFLUSH);

	/* Set the attributes */
	tcsetattr(midi_in, TCSANOW, &info);

	/* Startup sequence, as per ron@MIDI_DEV's instructions */
	/* Many thanks to Ron for supporting Linux */

	/* Step 1 */
	/* Power down */
	/* deassert DTR and RTS */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TIOCM_DTR;
	status &= ~TIOCM_RTS;
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 600 ms to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 600000;
	select(0, NULL, NULL, NULL, &tv);

  /* Step 2 */
  /* Power up */
  /* assert break */
	ioctl(midi_in, TIOCMGET, &status);
	status |= TCSBRK;
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 300 ms to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 300000;
	select(0, NULL, NULL, NULL, &tv);

	/* Step 3 */
	/* Set input mode */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TIOCM_DTR;
	status |= TIOCM_RTS;
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

	/* Step 4*/
	/* Set input mode */
	ioctl(midi_in, TIOCMGET, &status);
	status |= TIOCM_DTR;
	status &= TIOCM_RTS;
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

	/* Step 5 */
	/* Set output mode */
	/* Bitval = RTS, clock = DTR */

  /* 1 */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TIOCM_DTR; /* 0 */
	status |= TIOCM_RTS; /* 1 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);
	/****/
	ioctl(midi_in, TIOCMGET, &status);
	status |= TIOCM_DTR; /* 1 rising edge clock */
	status |= TIOCM_RTS; /* 1 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

	/* 1 */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TIOCM_DTR; /* 0 */
	status |= TIOCM_RTS; /* 1 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);
	/****/
	ioctl(midi_in, TIOCMGET, &status);
	status |= TIOCM_DTR; /* 1 rising edge clock */
	status |= TIOCM_RTS; /* 1 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

	/* 0 */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TIOCM_DTR; /* 0 */
	status &= ~TIOCM_RTS; /* 0 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);
	/****/
	ioctl(midi_in, TIOCMGET, &status);
	status |= TIOCM_DTR; /* 1 rising edge clock */
	status &= ~TIOCM_RTS; /* 0 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

	/* Step 6 ... necessary ?*/ 
	/* Set RTS=0,DTR=1 ... but they already are from previous ^ */
	/****/
	ioctl(midi_in, TIOCMGET, &status);
	status |= TIOCM_DTR; /* 1 rising edge clock */
	status &= ~TIOCM_RTS; /* 0 */
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 40 us to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 40;
	select(0, NULL, NULL, NULL, &tv);

  /* Step 7 */
  /* Deassert break */
	ioctl(midi_in, TIOCMGET, &status);
	status &= ~TCSBRK;
	ioctl(midi_in, TIOCMSET, status);
	/* Wait 100 ms to make sure everything is stable */
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	select(0, NULL, NULL, NULL, &tv);
  /*  End Midiator startup sequence -- midi_dev_type = MIDIATOR */
}
#endif // MIDIator

#elif defined(__OS_Win_)

/*************************************/
/*        Windoze MIDI INPUT         */
/*************************************/

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
    midiBuffer[writeOffset] = newMessage;
    writeOffset++;

    if( writeOffset >= MIDI_BUFFER_SIZE )
      writeOffset = 0;
    break;

  default:
    break;
  }
}

HMIDIIN hMidiIn ;   // Handle to Midi Output Device

RtMidi :: RtMidi(int device)
{
  MMRESULT result;
  UINT uDeviceID;
  MIDIINCAPS deviceCaps;
  UINT i;
  char msg[256];

  uDeviceID = midiInGetNumDevs();
  if (uDeviceID < 1) {
    sprintf(msg, "RtMidi: No windoze MIDI device available.\n");
    throw StkError(msg, StkError::MIDICARD_NOT_FOUND);
  }

  /* Our normal scheme is to use the default device if no argument
     is supplied to RtMidi() or if the argument = -1.  However,
     there is no way to specify a default MIDI device under windoze.
     So, I'm going to print the list if device = -1.
  */
  if ( (device != -1) && (device < uDeviceID) ) {
		// try to open device specified as argument
		result = midiInOpen(&hMidiIn, device,
												(DWORD)&midiInputCallback,
												(DWORD)NULL,
												CALLBACK_FUNCTION);
		if (result == MMSYSERR_NOERROR)
      goto have_good_device;
  }

  printf("\nMIDI input interfaces available: %i\n",uDeviceID);
  for (i=0; i<uDeviceID; i++) {
    result = midiInGetDevCaps(i, &deviceCaps, sizeof(MIDIINCAPS));
    printf("   MIDI interface %d is %s\n", i, deviceCaps.szPname);
  }

  if (uDeviceID > 1) {
    char choice[16];
    printf("\nType the MIDI interface to open: ");
    fgets(choice, 16, stdin);
    uDeviceID = (UINT) atoi(choice);
  }
  else uDeviceID -= 1;

  // Open the port and return any errors	
  result = midiInOpen(&hMidiIn, uDeviceID,
											(DWORD)&midiInputCallback,
											(DWORD)NULL,
											CALLBACK_FUNCTION);
  if (result != MMSYSERR_NOERROR) {
    sprintf(msg, "RtMidi: Cannot open Windoze MIDI interface %d.\n",
            uDeviceID);
    throw StkError(msg, StkError::MIDICARD_CONTROL);
  }

 have_good_device: // the current device is what we will use

  // Set up the circular buffer for the Midi Input Messages
  midiBuffer = new MIDIMESSAGE[MIDI_BUFFER_SIZE];
  readOffset = 0;
  writeOffset = 0;
	
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

  if ( readOffset == writeOffset ) return 0;

  lastEvent = midiBuffer[readOffset];
		
  readOffset++;
  if ( readOffset >= MIDI_BUFFER_SIZE ) readOffset = 0;

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

void RtMidi :: printMessage()
{
  printf("type = %d, channel = %d, byte2 = %f, byte3 = %f\n",
		 this->getType(), this->getChannel(), this->getByteTwo(),
		 this->getByteThree());
}

int RtMidi ::  getType()
{
  return messageType;
}

int RtMidi ::  getChannel()
{
  return channel;
}

MY_FLOAT RtMidi :: getByteTwo()
{
  return byteTwo;
}

MY_FLOAT RtMidi :: getByteThree()
{
  return byteThree;
}

MY_FLOAT RtMidi :: getDeltaTime()
{
  return deltaTime;
}


#endif
