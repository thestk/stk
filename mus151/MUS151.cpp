/********  Psychoacoustic Interface Program for MUS151  ************/
/********  Center for Computer Research in Music & Acoustics  ******/
/********  Stanford University, by Gary P. Scavone, 1998  **********/

#include "../miditabl.h"
#include "../WvOut.h"
#include "../RTWvOut.h"
#include "../SKINI11.h"
#include "TwoOsc.h"

#include "../SKINI11.msg"

#define INSTR_LEN 60

int numStrings = 0;
int notDone = 1;

#if defined(__SGI_REALTIME_)

#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

char inputString[MAX_IN_STRINGS][INSTR_LEN];
pid_t string_thread;

void newString(void *)
{
  int inOne = 0;
  while (notDone) {
    fgets(inputString[inOne],INSTR_LEN,stdin);
    if (inputString[inOne][0] == 'E' && inputString[inOne][1] == 'x') {
      notDone = 0;
    }
    else {
      numStrings++;
      if (numStrings > MAX_IN_STRINGS) {
        fprintf(stderr,"Losing MIDI data ... try increasing MAX_IN_STRINGS.\n");
        numStrings--;
      }
      inOne++;
      if (inOne == MAX_IN_STRINGS) inOne = 0;
    }
  }
}

#elif defined(__USS_REALTIME_)

#include <pthread.h>
//#include <pthread/mit/pthread.h>

char inputString[MAX_IN_STRINGS][INSTR_LEN];
pthread_t string_thread;

void *newString(void *)
{
  int inOne = 0;
  while (notDone) {
    fgets(inputString[inOne],INSTR_LEN,stdin);
    if (inputString[inOne][0] == 'E' && inputString[inOne][1] == 'x') {
      notDone = 0;
    }
    else {
      numStrings++;
      if (numStrings > MAX_IN_STRINGS) {
        fprintf(stderr,"Losing MIDI data ... try increasing MAX_IN_STRINGS.\n");
        numStrings--;
      }
      inOne++;
      if (inOne == MAX_IN_STRINGS) inOne = 0;
    }
  }
}

#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )

#include <process.h>
#include <winsock.h>

char inputString[MAX_IN_STRINGS][INSTR_LEN];
unsigned long string_thread;

#define SERVICE_PORT 2001

void newString(void *)
{
  int inOne = 0, i=0, m=0, n;
  SOCKET soc_id, accept_id;
  WSADATA wsaData;
  int nRet;
  struct sockaddr_in sockname;
  WORD wVersionRequested = MAKEWORD(1,1);
  char socBuf[INSTR_LEN];

  nRet = WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
    {   
      fprintf(stderr,"\n Wrong version\n");
      exit(0);
    }

  /* Create the server-side socket */
  soc_id = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(soc_id == INVALID_SOCKET) {
    fprintf(stderr,"Couldn't create socket!\n");
    exit(0);
  }

  sockname.sin_family=AF_INET;
  sockname.sin_addr.s_addr=INADDR_ANY;
  sockname.sin_port=htons(SERVICE_PORT);

  /* Bind socket to the appropriate port and interface (INADDR_ANY) */
  if (bind(soc_id,(LPSOCKADDR)&sockname,sizeof(sockname))==SOCKET_ERROR) {
    fprintf(stderr,"Couldn't bind socket!\n");
    exit(0);
  }

  /* Listen for incoming connections */
  printf("Listening for socket on port %d\n", SERVICE_PORT);
  if (listen(soc_id,1)==SOCKET_ERROR) {
    fprintf(stderr,"Couldn't set up listen on socket!\n");
    exit(0);
  }

  /* Accept and service one incoming connection request */
  accept_id=accept(soc_id,NULL,NULL);
  if (accept_id==INVALID_SOCKET) {
    fprintf(stderr,"Couldn't accept incoming connection on socket!\n");
    exit(0);
  }

  memset(socBuf, 0, sizeof(socBuf));

  printf("Socket connection made ... ready to receive SKINI messages.\n");
  while (notDone) {
    i = recv(accept_id, socBuf, INSTR_LEN, 0);
    if (i==0) notDone = 0;
    n = 0;
    while (n < i) {
      inputString[inOne][m++] = socBuf[n];
      if (socBuf[n++] == '\n') {
        if (inputString[inOne][0] == 'E' && inputString[inOne][1] == 'x') {
          notDone = 0;
          n = i;
        }
        else {
          m = 0;
          numStrings++;
          if (numStrings > MAX_IN_STRINGS) {
            fprintf(stderr,"Losing MIDI data ... try increasing MAX_IN_STRINGS.\n");
            numStrings--;
          }
          inOne++;
          if (inOne == MAX_IN_STRINGS) inOne = 0;
        }
        memset(inputString[inOne], 0, INSTR_LEN);
      }
    }
  }
  closesocket(accept_id);
  closesocket(soc_id);
  WSACleanup();
	printf("Socket connection closed.\n");
}

#else
char inputString[1][INSTR_LEN];
#endif

/* Error function in case of incorrect command-line argument specifications */
void errorf(char *func) {
  printf("\nuseage: %s Instr \n",func);
  printf("        where Instr = TwoOsc (only one available for now)\n\n");
  exit(0);
}

void main(int argc,char *argv[])
{
  long i, j, synlength;
  int type, rtInput = 0;
  int outOne = 0;
  char *fin;
  MY_FLOAT settleTime = 0.5;  /* in seconds */
  MY_FLOAT temp, byte3, lastPitch;
  WvOut *output;
  TwoOsc *instrument;
  SKINI11 *score;

  if (argc > 1) {
    if (!strcmp(argv[1],"TwoOsc")) instrument = new TwoOsc;
	else errorf(argv[0]);
  } else errorf(argv[0]);

  output = new RTWvOut();
  score = new SKINI11();

  rtInput = 1; /* We're going to always use realtime input */

  /* If using realtime input, setup the input thread. */
#if defined(__SGI_REALTIME_)
  if (rtInput)        {
    string_thread = sproc(newString, PR_SALL);
    if (string_thread == -1)  {
      fprintf(stderr, "unable to create input thread...aborting.\n");
      exit(-1);
    }
    instrument->noteOn(200.0, 0.2);
  }
#elif defined(__USS_REALTIME_)
  if (rtInput)        {
    if (pthread_create(&string_thread, NULL, newString, NULL))
      {
        fprintf(stderr, "unable to create input thread...aborting.\n");
        exit(-1);
      }
    instrument->noteOn(200.0,0.2);
  }
#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )
  if (rtInput)        {
    string_thread = _beginthread(newString, 0, NULL);
    if (string_thread == -1)  {
      fprintf(stderr, "Unable to create exit thread.\n");
      printf("Exiting MD2SKINI process.\n");
      exit(0);
    }
    instrument->noteOn(200.0,0.2);
  }
#endif

  /* Finally ... the runtime loop begins! */
  notDone = 1;
  synlength = RT_BUFFER_SIZE;
  while(notDone || numStrings)  {
    if (rtInput) {
      if (numStrings > 1) synlength = (long) RT_BUFFER_SIZE / numStrings;
      else synlength = RT_BUFFER_SIZE;
      for ( i=0; i<synlength; i++ )  {
        output->tick(instrument->tick());
      }
    }
    else {
      fin = fgets(inputString[0],INSTR_LEN,stdin);
      if (fin == NULL) notDone = 0;
      else {
        numStrings++;
      } 
    }
    if (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0)       {
        if (temp = score->getDelta()) { /* SKINI score file */
          synlength = (long) (temp * SRATE);
          for ( i=0; i<synlength; i++ )  {
            output->tick(instrument->tick());
          }
          synlength = 0;
        }
        if (type == __SK_NoteOn_ )       {
          if (( byte3 = score->getByteThree() ) == 0)
            instrument->noteOff(byte3*NORM_7);
          else {
            j = (int) score->getByteTwo();
            temp = __MIDI_To_Pitch[j];
            lastPitch = temp;
            instrument->noteOn(temp,byte3*NORM_7);
          }
        }
        else if (type == __SK_NoteOff_) {
					byte3 = score->getByteThree();
          instrument->noteOff(byte3*NORM_7);
        }
        else if (type == __SK_ControlChange_)   {
          j = (int) score->getByteTwo();
					byte3 = score->getByteThree();
          instrument->controlChange(j,byte3);
        }
        else if (type == __SK_AfterTouch_)   {
          j = (int) score->getByteTwo();
          instrument->controlChange(128,j);
        }
        else if (type == __SK_PitchBend_)   {
          temp = score->getByteTwo();
          j = (int) temp;
          temp -= j;
          lastPitch = __MIDI_To_Pitch[j] * pow(2.0,temp / 12.0) ;
          instrument->setFreq(1, lastPitch); /* change osc1 pitch for now */
        }
        else if (type == __SK_ProgramChange_)   {
        }
      }
      if (rtInput) {
        outOne += 1;
        if (outOne == MAX_IN_STRINGS) outOne = 0;
      }
      numStrings--;
    }
  }
  for (i=0;i<settleTime*SRATE;i++) { /* let the sound settle a little */
    output->tick(instrument->tick());
  }

  delete output;
  delete score;
  delete instrument;

#if defined(__SGI_REALTIME_)
  if (rtInput) kill(string_thread, SIGKILL);
#endif
	printf("MUS151 finished.\n");
}
