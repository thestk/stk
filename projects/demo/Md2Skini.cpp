/***************************************************/
/*
  Simple realtime MIDI to SKINI parser.

  This object takes MIDI from the input stream
  (via the RtMidi class), parses it, and turns it
  into SKINI messages.

  by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "RtMidi.h"
#include "Thread.h"
#include "Socket.h"
#include "SKINI.msg"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Exit thread declaration.
extern "C" THREAD_RETURN THREAD_TYPE stdinMonitor(void * ptr);

void usage(void) {
  printf("\nuseage: Md2Skini <flag(s)>\n\n");
  printf("   With no arguments, Md2Skini converts MIDI input to SKINI\n");
  printf("   format and sends the output directly to stdout.\n");
  printf("   With flag = -s <hostname>, the output is sent over a socket\n");
  printf("   connection (port 2001) to the optional hostname (default = localhost).\n");
  printf("   With flag = -f <filename>, the output stream is simultaneously\n");
  printf("   written to the file specified by the optional <filename>\n");
  printf("   (default = test.ski).\n\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  bool done = false, firstMessage = true, writeFile = false, useSocket = false;
  FILE *file = NULL;
  char fileName[256];
  char hostName[128];
  RtMidi *rtmidi = 0;
  Socket *soket = 0;
  Thread *thread = 0;

  if ( argc>5 ) {
    usage();
  }

  // Parse the command-line arguments.
  int i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') {
      switch(argv[i][1]) {

      case 's':
        if ((i+1 < argc) && argv[i+1][0] != '-') {
          i++;
          strncpy(hostName, argv[i], 128);
        }
        else strcpy(hostName, "localhost");
        useSocket = true;
        break;
          
      case 'f':
        if ((i+1 < argc) && argv[i+1][0] != '-') {
          i++;
          strncpy(fileName, argv[i], 252);
          if ( strstr(fileName,".ski") == NULL ) strcat(fileName, ".ski");
        }
        else strcpy(fileName, "test.ski");
        file = fopen(fileName,"wb");
        writeFile = true;
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
  try {
    rtmidi = new RtMidi();
  }
  catch (StkError &) {
    exit(0);
  }

  // If using sockets, setup the client socket
  if (useSocket) {
    try {
      soket = new Socket( 2001, hostName );
    }
    catch (StkError &) {
      exit(0);
    }
  }

  // Start the "exit" thread.
  thread = new Thread();
  if ( !thread->start( (THREAD_FUNCTION)&stdinMonitor, (void *) &done ) ) {
    fprintf(stderr, "Unable to create exit thread ... aborting.\n");
    goto cleanup;
  }

  // Write SKINI messages to buffer 's'.  This is the easiest way to
  // allow this single executable to work for both socketing and
  // printf's to stdout.
  char s[128];
  int channel, j;
  MY_FLOAT byte2, byte3;
  while ( !done ) {
    if (rtmidi->nextMessage() > 0) {
      byte3 = rtmidi->getByteThree();
      byte2 = rtmidi->getByteTwo();
      channel = rtmidi->getChannel();
      if (writeFile) dt = rtmidi->getDeltaTime();
      if (firstMessage) { // first MIDI message time stamp is meaningless
        dt = 0.0;
        firstMessage = false;
      }

      switch(rtmidi->getType()) {
      case __SK_NoteOn_:
        if (byte3 < 1.0) {
          sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,64.0);
          if (writeFile) {
            fprintf(file,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,64.0);
          }
        } else {
          sprintf(s,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
          if (writeFile) {
            fprintf(file,"NoteOn\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
          } 	 
        }
        break;
			
      case __SK_NoteOff_:
        if (byte3 < 2.0) byte3 = 64.0;
        sprintf(s,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
        if (writeFile) {
          fprintf(file,"NoteOff\t\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
        } 	 
        break;

      case __SK_PolyPressure_:
        sprintf(s,"PolyPressure\t%.3f  %d  %.1f  %.1f\n",0.0,channel,byte2,byte3);
        if (writeFile) {
          fprintf(file,"PolyPressure\t%.3f  %d  %.1f  %.1f\n",dt,channel,byte2,byte3);
        } 	 
        break;

      case __SK_ControlChange_:
        j = (int) byte2;
        switch(j) {
        case __SK_Volume_:
          sprintf(s,"Volume\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Volume\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_ModWheel_:
          sprintf(s,"ModWheel\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"ModWheel\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Breath_:
          sprintf(s,"Breath\t\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Breath\t\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_FootControl_:
          sprintf(s,"FootControl\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"FootControl\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Portamento_:
          sprintf(s,"Portamento\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Portamento\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Balance_:
          sprintf(s,"Balance\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Balance\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Pan_:
          sprintf(s,"Pan\t\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Pan\t\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Sustain_:
          sprintf(s,"Sustain\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Sustain\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        case __SK_Expression_:
          sprintf(s,"Expression\t%.3f  %d  %.1f\n",0.0,channel,byte3);
          if (writeFile) {
            fprintf(file,"Expression\t%.3f  %d  %.1f\n",dt,channel,byte3);
          } 	 
          break;
        default:
          sprintf(s,"ControlChange\t%.3f  %d  %d  %.1f\n",0.0,channel,j,byte3);
          if (writeFile) {
            fprintf(file,"ControlChange\t%.3f  %d  %d  %.1f\n",dt,channel,j,byte3);
          } 	 
          break;
        }
        break;

      case __SK_ProgramChange_:
				j = (int) byte2;
				sprintf(s,"ProgramChange\t%.3f  %d  %d\n",0.0,channel,j);
				if (writeFile) {
					fprintf(file,"ProgramChange\t%.3f  %d  %d\n",dt,channel,j);
				} 	 
        break;

      case __SK_ChannelPressure_:
				sprintf(s,"ChannelPressure\t%.3f  %d  %.1f\n",0.0,channel,byte2);
				if (writeFile) {
					fprintf(file,"ChannelPressure\t%.3f  %d  %.1f\n",dt,channel,byte2);
				} 	 
				break;

      case __SK_PitchBend_:
				sprintf(s,"PitchBend\t%.3f  %d  %f\n",0.0,channel,byte2);
        if (writeFile) {
          fprintf(file,"PitchBend\t%.3f  %d  %f\n",dt,channel,byte2);
				} 	 
				break;

      default:
				sprintf(s,"// Unknown\t%.3f  %d  %f  %f\n",0.0,channel,byte2,byte3);
				if (writeFile) {
					fprintf(file,"// Unknown\t\t%.3f  %d  %f  %f\n",dt,channel,byte2,byte3);
        } 	 
        break;
      }

      if (useSocket) {
        if ( soket->writeBuffer( s, strlen(s), 0 ) < 0 ) {
          fprintf(stderr,"Socket connection failed ... aborting.\n");
          goto cleanup;
        }
      }
      else {
        printf("%s", s);
        fflush(stdout);
      }
      memset(s, 0, sizeof(s));
    } else {
      // Sleep for 10 milliseconds
      Stk::sleep( 10 );
    }
  }

  sprintf(s, "Exiting Md2Skini process ... bye!\n");
  if (useSocket)
    soket->writeBuffer( s, strlen(s), 0 );
  else {
    printf("%s", s);
    fflush(stdout);
  }

  if (writeFile) {
    printf("Wrote SKINI output to file %s.\n", fileName);
    fclose(file);
  }

 cleanup:
  done = true;
  delete rtmidi;
  delete soket;
  delete thread;

  return 0;
}

THREAD_RETURN THREAD_TYPE stdinMonitor(void * ptr)
{
  bool *done = (bool *) ptr;
  char inputString[128];
  printf("Type 'Exit<cr>' to quit.\n");
  while ( !*done ) {
    fgets(inputString, 128, stdin);
    if (inputString[3] == 't' && inputString[1] == 'x'
        && inputString[2] == 'i' && inputString[0] == 'E') {
      *done = true;
    }
    else {
      printf(inputString);
      fflush(stdout);
    }
  }
  return 0;
}
