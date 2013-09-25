/*******************************************/
/*  Simple Realtime MIDI to SKINI Parser   */
/*  Gary P. Scavone, February 1998.        */
/*  Revised for sockets, May & June 1998.  */
/*  SKINI/MIDI merge added August 1999.    */
/*                                         */
/*  This object takes MIDI from the input  */
/*  stream (via the MIDIIO class),         */
/*  parses it, and turns it into SKINI     */
/*  messages.                              */
/*******************************************/

#include "MIDIIO.h"
#include "SKINI11.msg"

#if defined(__STK_REALTIME_)

int outAHere = 0;

// Do OS dependent declarations and includes
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

pthread_t exit_thread;

#elif defined(__OS_Win_)

#include <process.h>
#include <winsock.h>

unsigned long exit_thread;

#endif

// The thread function protocols are slightly different
// under Windoze ... but of course!
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))

void *monitorStdin(void *)

#elif defined(__OS_Win_)

void monitorStdin(void *)

#endif

{
  char inputString[128];
  printf("Type 'Exit<cr>' to quit.\n");
  while (!outAHere) {
    fgets(inputString, 128, stdin);
    if (inputString[3] == 't' && inputString[1] == 'x'
        && inputString[2] == 'i' && inputString[0] == 'E') {
      outAHere = 1;
    }
    else {
      printf(inputString);
      fflush(stdout);
    }
  }

#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  pthread_exit(NULL);
  return NULL;
#elif defined(__OS_Win_)
  _endthread();
#endif
}


void usage(void) {
  printf("useage: MD2SKINI <flag(s)>\n\n");
  printf("   With no arguments, MD2SKINI converts MIDI input to SKINI\n");
  printf("   format and sends the output directly to stdout.\n");
  printf("   With flag = -s <hostname>, the output is sent over a socket\n");
  printf("   connection to the optional hostname (default = localhost).\n");
  printf("   With flag = -f <filename>, the output stream is simultaneously\n");
  printf("   written to the file specified by the optional <filename>\n");
  printf("   (default = test.ski).\n\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  long j, i = 1;
  MY_FLOAT byte2, byte3;
  int channel;
  int firstMessage = 1;
  int writeFileOut = 0;
  FILE *fileOut = NULL;
  MIDIIO *controller;
  char hostName[256];
  char fileName[256];
  int useSocket = 0;
  int theSocket = 0;
  struct sockaddr_in saServer;
  static struct timeval timeout = {0, 10000}; // ten millisecond

  if (argc>5) {
    usage();
  }

  // Parse the command-line arguments.
  while (i < argc) {
    if (argv[i][0] == '-') {
      switch(argv[i][1]) {

      case 's':
        if ((i+1 < argc) && argv[i+1][0] != '-') {
          i++;
          strcpy(hostName,argv[i]);
        }
        else strcpy(hostName,"localhost");
        useSocket = 1;
        break;
          
      case 'f':
        if ((i+1 < argc) && argv[i+1][0] != '-') {
          i++;
          strcpy(fileName,argv[i]);
          if (strstr(fileName,".ski") == NULL) strcat(fileName,".ski");
        }
        else strcpy(fileName,"test.ski");
        fileOut = fopen(fileName,"wb");
        writeFileOut = 1;
        break;
          
      default:
        usage();
        break;
      }
    }
    else usage();
    i++;
  }

  MY_FLOAT dt=0.0;
  controller = new MIDIIO();

  // If using sockets, setup the client socket
  if (useSocket) {

#if defined(__OS_Win_)  // Stupid Windoze only stuff
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(1,1);

    WSAStartup(wVersionRequested, &wsaData);
		if (wsaData.wVersion != wVersionRequested) {	
			fprintf(stderr,"\n Wrong Windoze socket library version!\n");
			exit(0);
		}
#endif

		// Create the client-side socket
		theSocket = socket(AF_INET,	SOCK_STREAM, IPPROTO_TCP);
		if (theSocket < 0) {
      fprintf(stderr,"Couldn't create socket ... aborting!\n");
      exit(0);
    }

    struct hostent *hostp;

    if ((hostp = gethostbyname(hostName)) == 0) {
      fprintf(stderr,"%s: unknown host ... aborting!\n", hostName);
      exit(0);
    }

		// Fill in the address structure
    saServer.sin_family = AF_INET;
    memcpy((void *)&saServer.sin_addr, hostp->h_addr, hostp->h_length);
		saServer.sin_port = htons(2001);	// Port number

		// Connect to the server
		if(connect(theSocket, (struct sockaddr *)&saServer, sizeof(saServer)) < 0) {
      fprintf(stderr,"Socket connect failed ... aborting!\n");
#if defined(__OS_Win_)
      closesocket(theSocket);
      WSACleanup();
#else
      close(theSocket);
#endif
      exit(0);
    }
  }

  // Setup the exit thread.
#if (defined(__OS_IRIX_) || defined(__OS_Linux_))
  if (pthread_create(&exit_thread, NULL, monitorStdin, NULL)) {
    fprintf(stderr, "Unable to create exit thread ... aborting.\n");
    exit(0);
  }
#elif defined(__OS_Win_)
  exit_thread = _beginthread(monitorStdin, 0, NULL);
  if (exit_thread == -1)  {
    fprintf(stderr, "Unable to create exit thread ... aborting.\n");
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
          sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,64.0);
          if (writeFileOut) {
            fprintf(fileOut,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,64.0);
          }
        } else {
          sprintf(s,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
          if (writeFileOut) {
            fprintf(fileOut,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
          } 	 
        }
        break;
			
      case __SK_NoteOff_:
        if (byte3 < 2.0) byte3 = 64.0;
        sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
        if (writeFileOut) {
          fprintf(fileOut,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
        } 	 
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

      if (useSocket) {
        if (send(theSocket, s,	strlen(s), 0) < 0) {
          fprintf(stderr,"Socket connection failed ... aborting.\n");
#if defined(__OS_Win_)
          closesocket(theSocket);
          WSACleanup();
#else
          close(theSocket);
#endif
          outAHere = 1;
          exit(0);
        }
      }
      else {
        printf("%s", s);
        fflush(stdout);
      }
      memset(s, 0, sizeof(s));
    } else {
			// With Irix 5.3, you can no longer use the usleep()
			// function.  And in Windoze, you can't use the select()
			// function to do timings.  I love supporting multiple
			// platforms!
#if defined(__OS_Win_)
			Sleep ( (DWORD) 5);
#else
      timeout.tv_sec = 0;
      timeout.tv_usec = 10000; // 0.01 seconds
      select(0, NULL, NULL, NULL, &timeout);
#endif
    }
  }

  sprintf(s,"Exiting MD2SKINI process ... bye!\n");
  if (useSocket) {
    send(theSocket, s, strlen(s), 0);
#if defined(__OS_Win_)
    closesocket(theSocket);
    WSACleanup();
#else
    close(theSocket);
#endif
  }
  else {
    printf("%s", s);
    fflush(stdout);
  }
  if (writeFileOut) {
    printf("Wrote SKINI output to file %s.\n", fileName);
    fclose(fileOut);
  }
  delete controller;
  return 0;
}

#endif
