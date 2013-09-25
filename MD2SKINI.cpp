/*******************************************/
/*  Simple Realtime MIDI to SKINI Parser   */
/*  Gary P. Scavone, February 1998.        */
/*  Revised for sockets, May & June 1998.  */
/*                                         */
/*  This object takes MIDI from the input  */
/*  stream (via the MIDIIO class),         */
/*  parses it, and turns it into SKINI     */
/*  messages.                              */
/*******************************************/

#include "miditabl.h"
#include "MIDIIO.h"
#include "SKINI11.msg"

int outAHere = 0;

#if defined(__SGI_REALTIME_)

#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>
#include <unistd.h>

pid_t exit_thread;

void newString(void *)
{
  char inputString[128];
  printf("Type 'ex<cr>' to quit.\n");
  while (!outAHere) {
		fgets(inputString, 128, stdin);
		if (inputString[0] == 'e' && inputString[1] == 'x') {
			outAHere = 1;
		}
		else printf("Type 'ex<cr>' to quit.\n");
  }
}

#elif defined(__USS_REALTIME_)

#include <pthread.h>

pthread_t exit_thread;

void *newString(void *)
{
  char inputString[128];
  printf("Type 'ex<cr>' to quit.\n");
  while (!outAHere) {
		fgets(inputString, 128, stdin);
		if (inputString[0] == 'e' && inputString[1] == 'x') {
			outAHere = 1;
		}
		else printf("Type 'ex<cr>' to quit.\n");
  }
}

#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )

#include <process.h>
#include <winsock.h>

unsigned long exit_thread;

void newString(void *)
{
  char inputString[128];
  printf("Type 'ex<cr>' to quit.\n");
  while (!outAHere) {
		fgets(inputString, 128, stdin);
		if (inputString[0] == 'e' && inputString[1] == 'x') {
			outAHere = 1;
		}
		else printf("Type 'ex<cr>' to quit.\n");
  }
}

#endif

void errorf(void) {
  printf("useage: MD2SKINI <file name>\n");
  printf("   where the optional <file name> specifies a file\n");
  printf("   to which the SKINI output stream is written.\n");
  printf("   The SKINI output stream is always written to stdout,\n");
  printf("   whether an output file is specified or not.\n");
  exit(0);
}

void main(int argc,char *argv[])
{
  long j;
  int oneOn = 0;
  MY_FLOAT byte2, byte3;
  int channel;
  int firstMessage = 1;
  int writeFileOut = 0;
  FILE *fileOut;
  MIDIIO *controller;

  if (argc>2) 	{
    errorf();
  }

  if (argc == 2)		{
    fileOut = fopen(argv[1],"wb");
    writeFileOut = 1;
  }

  MY_FLOAT dt=0.0;

  controller = new MIDIIO();

  /* Setup the exit thread. */
#if defined(__SGI_REALTIME_)
  exit_thread = sproc(newString, PR_SALL);
  if (exit_thread == -1)	{
    fprintf(stderr, "Unable to create exit thread.\n");
    printf("Exiting MD2SKINI process.\n");
    exit(0);
  }
#elif defined(__USS_REALTIME_)
  int err = 0;
  err = pthread_create(&exit_thread, NULL, newString, NULL);
  if (err)
    {
      fprintf(stderr, "Unable to create exit thread.\n");
      printf("Exiting MD2SKINI process.\n");
      exit(0);
    }
#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )
  exit_thread = _beginthread(newString, 0, NULL);
  if (exit_thread == -1)	{
    fprintf(stderr, "Unable to create exit thread.\n");
    printf("Exiting MD2SKINI process.\n");
    exit(0);
  }
#endif

  /* Setup the client socket */
#if defined(__SOCKET)

  WORD wVersionRequested = MAKEWORD(1,1);
  WSADATA wsaData;
  SOCKET  theSocket;
  int nRet;

  nRet = WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
    {	
			fprintf(stderr,"\n Wrong version\n");
      outAHere = 1;
      WSACleanup();
			exit(0);
    }

  theSocket = socket(AF_INET,	SOCK_STREAM, IPPROTO_TCP);
  if (theSocket == INVALID_SOCKET)
    {
			fprintf(stderr,"socket open failed\n");
      outAHere = 1;
      WSACleanup();
			exit(0);
    }

  /* Fill in the address structure */
  SOCKADDR_IN saServer;

  /* Modified to always use the loopback address of 127.0.0.1 */
  saServer.sin_family = AF_INET;
  saServer.sin_port = htons(2001);	// Port number from command line
  saServer.sin_addr.S_un.S_addr  = inet_addr( "127.0.0.1" );

  /* connect to the server */
  nRet = connect(theSocket, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
  if (nRet == SOCKET_ERROR)
    {
			fprintf(stderr,"socket connect failed\n");
			closesocket(theSocket);
      outAHere = 1;
      WSACleanup();
			exit(0);
    }
#endif

  /* Write SKINI messages to buffer 's'.  This is the easiest way
     to allow this single executable to work for both socketing
     and printf's to stdout.
  */
  char s[128];

  while (!outAHere) {
    if (controller->nextMessage() > 0) {
      byte3 = controller->getByteThree();
      byte2 = controller->getByteTwo();
      channel = controller->getChannel();
      if (writeFileOut) dt = controller->getDeltaTime();
      if (firstMessage) { /* first MIDI message time stamp is meaningless */
        dt = 0.0;
        firstMessage = 0;
      }

      switch(controller->getType()) {
      case __SK_NoteOn_:
        if (byte3 < 1.0) {
          if (oneOn == 1) {
            sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,64.0);
            if (writeFileOut) {
              fprintf(fileOut,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,64.0);
            } 	 
          }
          oneOn -= 1;
        } else {
          sprintf(s,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
          } 	 
          oneOn += 1;
        }
        break;
			
      case __SK_NoteOff_:
        if (byte3 < 2.0) byte3 = 64.0;
        if (oneOn == 1) {
          sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
        }
        if (writeFileOut) {
          fprintf(fileOut,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
        } 	 
        oneOn -= 1;
        break;

      case __SK_PolyPressure_:
        sprintf(s,"PolyPressure\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
        if (writeFileOut) {
          fprintf(fileOut,"PolyPressure\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
        } 	 
        break;

      case __SK_ControlChange_:
        j = (int) byte2;
        switch(j) {
        case __SK_Volume_:
          sprintf(s,"Volume\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Volume\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_ModWheel_:
          sprintf(s,"ModWheel\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"ModWheel\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Breath_:
          sprintf(s,"Breath\t\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Breath\t\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_FootControl_:
          sprintf(s,"FootControl\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"FootControl\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Portamento_:
          sprintf(s,"Portamento\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Portamento\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Balance_:
          sprintf(s,"Balance\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Balance\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Pan_:
          sprintf(s,"Pan\t\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Pan\t\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Sustain_:
          sprintf(s,"Sustain\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Sustain\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Expression_:
          sprintf(s,"Expression\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"Expression\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        default:
          sprintf(s,"ControlChange\t%.3f  %d  %ld  %.1f\n",0.0,channel,j,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"ControlChange\t%.3f  %d  %ld  %.1f\n",dt,channel,j,byte3);
          } 	 
          break;
        }
        break;

      case __SK_ProgramChange_:
				j = (int) byte2;
				sprintf(s,"ProgramChange\t%.3f  %d  %ld\n",0.0,channel,j);
				if (writeFileOut) {
					fprintf(fileOut,"ProgramChange\t%.3f  %d  %ld\n",dt,channel,j);
				} 	 
        break;

      case __SK_ChannelPressure_:
				sprintf(s,"ChannelPressure\t%.3f  %d  %.1f\n",0.0,channel,byte2);
				if (writeFileOut) {
					fprintf(fileOut,"ChannelPressure\t%.3f  %d  %.1f\n",dt,channel,byte2);
				} 	 
				break;

      case __SK_PitchBend_:
				sprintf(s,"PitchBend\t%.3f  %d  %f\n",0.0,channel,byte2);
        if (writeFileOut) {
          fprintf(fileOut,"PitchBend\t%.3f  %d  %f\n",dt,channel,byte2);
				} 	 
				break;

      default:
				sprintf(s,"// Unknown\t%.3f  %d  %f  %f\n",0.0,channel,byte2,byte3);
				if (writeFileOut) {
					fprintf(fileOut,"// Unknown\t\t%.3f  %d  %f  %f\n",dt,channel,byte2,byte3);
        } 	 
        break;
      }

#if defined(__SOCKET)
			nRet = send(theSocket, s,	strlen(s), 0);
      if (nRet == SOCKET_ERROR)
        {
          fprintf(stderr,"send failed\n");
          closesocket(theSocket);
          outAHere = 1;
          WSACleanup();
          exit(0);
        }
#else
      printf("%s", s);
      fflush(stdout);
#endif
      memset(s, 0, sizeof(s));
#if defined(__OS_Win_)
    } else Sleep ( (DWORD) 2);
#else
    } else usleep( (unsigned long) 2000);
#endif
  }

  sprintf(s,"Exiting MD2SKINI process ... bye!\n");
#if defined(__SOCKET)
  nRet = send(theSocket, s, strlen(s), 0);
  closesocket(theSocket);
  WSACleanup();
#else
  printf("%s", s);
  fflush(stdout);
#endif

  if (writeFileOut) {
    printf("Wrote SKINI output to file %s.\n", argv[1]);
    fclose(fileOut);
  }
  delete controller;

#if defined(__SGI_REALTIME_)
  kill(exit_thread, SIGKILL);
#endif
}

