/**************  Test Main Program Individual Voice *********************/

#include "miditabl.h"
#include "WvOut.h"
#include "SndWvOut.h"
#include "WavWvOut.h"
#include "MatWvOut.h"
#ifndef __NeXT_
#include "RTWvOut.h"
#endif
#include "SKINI11.h"
#include "Instrmnt.h"
#include "Marimba.h"
#include "Vibraphn.h"
#include "AgogoBel.h"
#include "Plucked.h"
#include "Mandolin.h"
#include "Clarinet.h"
#include "Flute.h"
#include "Brass.h"
#include "Bowed.h"
#include "Rhodey.h"
#include "Wurley.h"
#include "TubeBell.h"
#include "HeavyMtl.h"
#include "PercFlut.h"
#include "BeeThree.h"
#include "FMVoices.h"
#include "VoicForm.h"
#include "Moog1.h"
#include "Simple.h"
#include "DrumSynt.h"
#include "Shakers.h"
#include "Reverb.h"
#include "NRev.h"
#include "PRCRev.h"
#include "JCRev.h"

#define NUM_INSTS 30

char insts[NUM_INSTS][10] = {   "Marimba", "Vibraphn", "AgogoBel", "Plucked",
                                "Mandolin", "Clarinet", "Flute", "Brass",
                                "Bowed", "Rhodey", "Wurley", "TubeBell",
                                "HeavyMtl", "PercFlut", "BeeThree", "FMVoices",
                                "Moog1", "Simple", "VoicForm", "TKSinger",
                                "Shakers", "Maraca", "Cabasa", "Sekere",
                                "Guiro", "Waterdrp", "Bamboo", "Tambourn",
                                "Sleighbl", "DrumSynt"};

#include "SKINI11.msg"

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
    fgets(inputString[inOne],128,stdin);
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

char inputString[MAX_IN_STRINGS][INSTR_LEN];
pthread_t string_thread;

void *newString(void *)
{
  int inOne = 0;
  while (notDone) {
    fgets(inputString[inOne],128,stdin);
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
  int i,j;

  printf("\nuseage: %s Instr flag(s) \n",func);
  printf("        where flag = -s <file name> for .snd output file,\n");
  printf("        -w <file name> for .wav output file,\n");
  printf("        -m <file name> for .mat output file,\n");
#if !defined(__NeXT_)
  printf("        -r for realtime output,\n");
  printf("        -i for realtime input (versus scorefile),\n");
#endif
  printf("        and Instr = one of these:\n");
  for (i=0;i<NUM_INSTS;i+=8)  {
    for (j=0;j<8 && (i+j) < NUM_INSTS;j++)  {
      printf("%s ",insts[i+j]);
    }
    printf("\n");
  }
  printf("\nSimultaneous multiple output types are supported.\n");
  printf("If the optional <file names> are not specified,\n");
  printf("default names will be indicated.  Each flag must\n");
  printf("include its own '-' sign.\n\n");
  exit(0);
}

void main(int argc,char *argv[])
{
  long i, j, w, synlength;
  int type, rtInput = 0;
  int numOuts = 0, outOne = 0;
  char *fin;
	char flags[15];
  MY_FLOAT reverbTime = 0.5;  /* in seconds */
  MY_FLOAT temp, byte3, lastPitch, outSample;
  char fileName[256];
  WvOut **output;
  Instrmnt *instrument;
  Reverb *reverb;
  SKINI11 *score;
  
  /* Check the command-line arguments for errors and to determine
   * the number of WvOut objects to be instantiated.
   */
  if (argc > 2 && argc < 11) {
    i = 2;
		j = 0;
    while (i < argc) {
      if (argv[i][0] == '-') {
        if ( (argv[i][1] == 'r') || (argv[i][1] == 's') ||
             (argv[i][1] == 'w') || (argv[i][1] == 'm') )
          numOuts++;
        else if (argv[i][1] != 'i') errorf(argv[0]);
				flags[j] = argv[i][1];
				j++;
      }
      i++;
    }
		/* Check for multiple flags of the same type */
		for (i=0; i<=j; i++) {
			w = i+1;
			while (w <= j) {
				if (flags[i] == flags[w]) {
					printf("\nError: Multiple command line flags of the same type specified.\n\n");
					errorf(argv[0]);
				}
				w++;
			}
		}
		/* Check number of outputs */
    if (numOuts > 0) output = (WvOut **) malloc(numOuts * sizeof(WvOut *));
    else errorf(argv[0]);
  }
  else errorf(argv[0]);
  
  /* Parse the command-line flags and instantiate WvOut objects. */
  i = 2;
  j = 0;
  while (i < argc)
    {
      if (argv[i][0] == '-')
        {
          switch(argv[i][1])
            {
            case 'i':
#if !defined(__NeXT_)
              rtInput = 1;
              break;
#else
              errorf(argv[0]);
#endif
        
            case 'r':
#if !defined(__NeXT_)
              output[j] = new RTWvOut();
              j++;
              break;
#else
              errorf(argv[0]);
#endif
        
            case 'w':
              if ((i+1 < argc) && argv[i+1][0] != '-')
                strcpy(fileName,argv[i+1]);
              else strcpy(fileName,"testwav");
              output[j] = new WavWvOut(2,fileName);
              j++;
              break;
          
            case 's':
              if ((i+1 < argc) && argv[i+1][0] != '-')
                strcpy(fileName,argv[i+1]);
              else strcpy(fileName,"testsnd");
              output[j] = new SndWvOut(2,fileName);
              j++;
              break;
          
            case 'm':
              if ((i+1 < argc) && argv[i+1][0] != '-')
                strcpy(fileName,argv[i+1]);
              else strcpy(fileName,"testmat");
              output[j] = new MatWvOut(2,fileName);
              j++;
              break;
          
            default:
              errorf(argv[0]);
              break;
            }
        }
      i++;
    }
  
  /* Parse the Instrument command-line argument. */
  if      (!strcmp(argv[1],"Simple"))   instrument = new Simple;
  else if (!strcmp(argv[1],"Clarinet")) instrument = new Clarinet(50.0);
  else if (!strcmp(argv[1],"Flute"))    instrument = new Flute(50.0);
  else if (!strcmp(argv[1],"Brass"))    instrument = new Brass(50.0);
  else if (!strcmp(argv[1],"Bowed"))    instrument = new Bowed(50.0);
  else if (!strcmp(argv[1],"Plucked"))  instrument = new Plucked(50.0);
  else if (!strcmp(argv[1],"Mandolin")) instrument = new Mandolin(50.0);
  else if (!strcmp(argv[1],"Marimba"))  instrument = new Marimba;
  else if (!strcmp(argv[1],"Vibraphn")) instrument = new Vibraphn;
  else if (!strcmp(argv[1],"AgogoBel")) instrument = new AgogoBel;
  else if (!strcmp(argv[1],"Rhodey"))   instrument = new Rhodey;
  else if (!strcmp(argv[1],"Wurley"))   instrument = new Wurley;
  else if (!strcmp(argv[1],"TubeBell")) instrument = new TubeBell;
  else if (!strcmp(argv[1],"HeavyMtl")) instrument = new HeavyMtl;
  else if (!strcmp(argv[1],"PercFlut")) instrument = new PercFlut;
  else if (!strcmp(argv[1],"BeeThree")) instrument = new BeeThree;
  else if (!strcmp(argv[1],"Moog1"))    instrument = new Moog1;
  else if (!strcmp(argv[1],"FMVoices")) instrument = new FMVoices;
  else if (!strcmp(argv[1],"VoicForm")) instrument = new VoicForm;
  else if (!strcmp(argv[1],"DrumSynt")) instrument = new DrumSynt;
  else if (!strcmp("Shakers",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 0);
  }
  else if (!strcmp("Maraca",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 0);
  }
  else if (!strcmp("Sekere",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 1);
  }
  else if (!strcmp("Cabasa",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 2);
  }
  else if (!strcmp("Bamboo",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 3);
  }
  else if (!strcmp("Waterdrp",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 4);
  }
  else if (!strcmp("Tambourn",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 5);
  }
  else if (!strcmp("Sleighbl",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 6);
  }
  else if (!strcmp("Guiro",argv[1])) {
    instrument = new Shakers;
    instrument->controlChange(__SK_ShakerInst_, 7);
  }
  else {
    errorf(argv[0]);
  }

  score = new SKINI11();
  reverb = new PRCRev(reverbTime);
  reverb->setEffectMix(0.2);
  
  /* If using realtime input, setup the input thread. */
#if defined(__SGI_REALTIME_)
  if (rtInput)        {
    string_thread = sproc(newString, PR_SALL);
    if (string_thread == -1)  {
      fprintf(stderr, "unable to create input thread...aborting.\n");
      exit(-1);
    }
    instrument->noteOn(200.0,0.1);
  }
#elif defined(__USS_REALTIME_)
  if (rtInput)        {
    if (pthread_create(&string_thread, NULL, newString, NULL))
      {
        fprintf(stderr, "unable to create input thread...aborting.\n");
        exit(-1);
      }
    instrument->noteOn(200.0,0.1);
  }
#elif (defined(__WINDS_REALTIME_) || defined(__WINMM_REALTIME_) )
  if (rtInput)        {
    string_thread = _beginthread(newString, 0, NULL);
    if (string_thread == -1)  {
      fprintf(stderr, "Unable to create exit thread.\n");
      printf("Exiting MD2SKINI process.\n");
      exit(0);
    }
    instrument->noteOn(200.0,0.1);
  }
#endif
  
  /* Finally ... the runtime loop begins! */
  notDone = 1;
  while(notDone || numStrings)  {
    if (rtInput) {
      synlength = RT_BUFFER_SIZE;
      for ( i=0; i<synlength; i++ )  {
        if (numOuts > 1) {
          outSample = reverb->tick(instrument->tick());
          for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
        }
        else output[0]->tick(reverb->tick(instrument->tick()));
      }
    }
    else {
      fin = fgets(inputString[0],INSTR_LEN,stdin);
      if (fin == NULL) notDone = 0;
      else {
        numStrings++;
      } 
    }
    while (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0)       {
        synlength = (long) (score->getDelta() * SRATE);
#if defined(_debug_)
        if (!rtInput) printf("Time = %f:   ",output.getTime());
#endif
        for ( i=0; i<synlength; i++ )  {
          if (numOuts > 1) {
            outSample = reverb->tick(instrument->tick());
            for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
          }
          else output[0]->tick(reverb->tick(instrument->tick()));
        }
        synlength = 0;
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
          instrument->setFreq(lastPitch);
        }
        else if (type == __SK_ProgramChange_)   {
          instrument->noteOff(1.0);
          for (i=0;i<4096;i++)    {
            if (numOuts > 1) {
              outSample = reverb->tick(instrument->tick());
              for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
            }
            else output[0]->tick(reverb->tick(instrument->tick()));
          }
          delete instrument;
          j = (int) score->getByteTwo();
          if      (j==0) instrument = new Clarinet(50.0);
          else if (j==1) instrument = new Flute(50.0);
          else if (j==2) instrument = new Brass(50.0);
          else if (j==3) instrument = new Bowed(50.0);
            
          else if (j==4) instrument = new Plucked(50.0);
          else if (j==5) instrument = new Mandolin(50.0);
          else if (j==6) instrument = new Marimba;
          else if (j==7) instrument = new Vibraphn;
          else if (j==8) instrument = new AgogoBel;
          else if (j==9) instrument = new Rhodey;
          else if (j==10) instrument = new Wurley;
          else if (j==11) instrument = new TubeBell;
            
          else if (j==12) instrument = new HeavyMtl;
          else if (j==13) instrument = new PercFlut;
          else if (j==14) instrument = new BeeThree;
          else if (j==15) instrument = new Moog1;
            
          else if (j==16) instrument = new FMVoices;
          else if (j==17) instrument = new VoicForm;
            
          else if (j==18) instrument = new DrumSynt;

          else if (j==19)  {  /* "Maraca" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 0);
          }
          else if (j==20)  {  /* "Sekere" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 1);
          }
          else if (j==21)  {  /* "Cabasa" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 2);
          }
          else if (j==22)  {  /* "Bamboo" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 3);
          }
          else if (j==23)  {  /* "Waterdrp" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 4);
          }
          else if (j==24)  {  /* "Tambourn" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 5);
          }
          else if (j==25)  {  /* "Sleighbl" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 6);
          }
          else if (j==26)  {  /* "Guiro" */
            instrument = new Shakers;
            instrument->controlChange(__SK_ShakerInst_, 7);
          }
          instrument->noteOn(lastPitch, 0.5);
        }
      }
      if (rtInput) {
        outOne += 1;
        if (outOne == MAX_IN_STRINGS) outOne = 0;
      }
      numStrings--;
    }
  }
  for (i=0;i<2*reverbTime*SRATE;i++) { /* let the reverb settle a little */
    if (numOuts > 1) {
      outSample = reverb->tick(instrument->tick());
      for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
    }
    else output[0]->tick(reverb->tick(instrument->tick()));
  }

  for ( i=0; i<numOuts; i++ ) delete output[i];
  free(output);
  delete score;
  delete instrument;
  delete reverb;


#if defined(__SGI_REALTIME_)
  if (rtInput) kill(string_thread, SIGKILL);
#endif
	printf("syntmono finished ... au revoir.\n");
}
