// Miscellaneous parsing and error functions for use with syntmono.
//
// Gary P. Scavone, 1999.

#include "utilities.h"
#include "Controller.h"

// STK Instrument Classes
#include "Object.h"
#include "Clarinet.h"
#include "Flute.h"
#include "Brass.h"
#include "Bowed.h"
#include "Plucked.h"
#include "Mandolin.h"
#include "ModalBar.h"
#include "Rhodey.h"
#include "Wurley.h"
#include "TubeBell.h"
#include "HeavyMtl.h"
#include "PercFlut.h"
#include "BeeThree.h"
#include "Moog1.h"
#include "FMVoices.h"
#include "VoicForm.h"
#include "DrumSynt.h"
#include "Shakers.h"
#include "BowedBar.h"
#include "BlowHole.h"

// STK WvOut classes
#include "WvOut.h"
#include "SndWvOut.h"
#include "WavWvOut.h"
#include "MatWvOut.h"
#include "AifWvOut.h"
#if defined(__STK_REALTIME_)
  #include "RtWvOut.h"
#endif

#define NUM_INSTS 20

// The order of the following list is important.  The location of a particular
// instrument in the list should correspond to that instrument's ProgramChange
// number (i.e. Clarinet = ProgramChange 0).
char insts[NUM_INSTS][10] = { "Clarinet", "Flute", "Brass", "Bowed", "BlowHole",
                              "Plucked", "Mandolin", "Rhodey", "Wurley", "TubeBell",
                              "HeavyMtl", "PercFlut", "BeeThree", "Moog1",
                              "FMVoices", "VoicForm", "DrumSynt", "Shakers",
                              "BowedBar", "ModalBar", };

int newInstByNum(int instNum)
{
  int temp = instNum;
  extern Instrmnt *instrument;
  
  if     (instNum==0) instrument = new Clarinet(20.0);
  else if (instNum==1) instrument = new Flute(20.0);
  else if (instNum==2) instrument = new Brass(20.0);
  else if (instNum==3) instrument = new Bowed(20.0);
  else if (instNum==4) instrument = new BlowHole(20.0);
  else if (instNum==5) instrument = new Plucked(20.0);
  else if (instNum==6) instrument = new Mandolin(20.0);

  else if (instNum==7) instrument = new Rhodey;
  else if (instNum==8) instrument = new Wurley;
  else if (instNum==9) instrument = new TubeBell;
  else if (instNum==10) instrument = new HeavyMtl;
  else if (instNum==11) instrument = new PercFlut;
  else if (instNum==12) instrument = new BeeThree;
  else if (instNum==13) instrument = new Moog1;
            
  else if (instNum==14) instrument = new FMVoices;
  else if (instNum==15) instrument = new VoicForm;
            
  else if (instNum==16) instrument = new DrumSynt;
  else if (instNum==17) instrument = new Shakers;
  else if (instNum==18) instrument = new BowedBar;
  else if (instNum==19) instrument = new ModalBar;

  else {
    printf("\nUnknown instrument or program change requested!\n");
    temp = -1;
  }

  return temp;
}

int newInstByName(char *instName)
{
  int i = 0, temp = -1, notFound = 1;;

  while (i<NUM_INSTS && notFound) {
    if (!strcmp(instName,insts[i])) {
      notFound = 0;
      temp = newInstByNum(i);
    }
    i++;
  }

  if (temp < 0) 
    printf("\nUnknown instrument or program change requested!\n");

  return temp;
}

void usage(char *function) {
  /* Error function in case of incorrect command-line argument specifications */
  int i, j;

  printf("\nuseage: %s Instrument flag(s) \n", function);
  printf("        where flag = -os <file name> for .snd audio output file,\n");
  printf("        -ow <file name> for .wav audio output file,\n");
  printf("        -om <file name> for .mat audio output file,\n");
  printf("        -oa <file name> for .aif audio output file,\n");
#if defined(__STK_REALTIME_)
  printf("        -or for realtime audio output,\n");
  printf("        -ip for realtime control input by pipe,\n");
  printf("         (won't work under Win95/98),\n");
  printf("        -is for realtime control input by socket,\n");
  printf("        -im for realtime control input by MIDI,\n");
#endif
  printf("        and Instrument = one of these:\n");
  for (i=0;i<NUM_INSTS;i+=8)  {
    for (j=0;j<8 && (i+j) < NUM_INSTS;j++)  {
      printf("%s ",insts[i+j]);
    }
    printf("\n");
  }
  printf("\nSimultaneous multiple output types are supported.\n");
  printf("Likewise, simultaneous control input types are supported.\n");
  printf("SKINI formatted scorefiles can be piped or redirected\n");
  printf("to %s, though realtime control flags should be omitted\n", function);
  printf("when doing so. If the optional <file names> are not\n");
  printf("specified, default names will be indicated.  Each flag\n");
  printf("must include its own '-' sign.\n\n");
  exit(0);
}

int checkArgs(int numArgs, char *args[])
{
  int w, i = 2, j = 0;
  int numOutputs = 0;
  char flags[16] = "";

  if (numArgs < 3 || numArgs > 10) usage(args[0]);

  while (i < numArgs) {
    if (args[i][0] == '-') {
      if (args[i][1] == 'o') {
        if ( (args[i][2] == 'r') || (args[i][2] == 's') ||
             (args[i][2] == 'w') || (args[i][2] == 'm')
             || (args[i][2] == 'a') )
          numOutputs++;
      }
      else if (args[i][1] == 'i') {
        if ( (args[i][2] != 's') && (args[i][2] != 'p') &&
             (args[i][2] != 'm') ) usage(args[0]);
      }
      else usage(args[0]);
      flags[j] = args[i][2];
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
        usage(args[0]);
      }
      w++;
    }
  }

  /* Make sure we have at least one output type */
  if (numOutputs < 1) usage(args[0]);

  return numOutputs;
}

int parseArgs(int numArgs, char *args[])
{
  int i = 2, j = 0;
  int inputMask = 0;
  char fileName[256];
  extern WvOut **output;

  while (i < numArgs) {
    if ( (args[i][0] == '-') && (args[i][1] == 'i') ) {
      switch(args[i][2]) {

      case 'p':
#if defined(__STK_REALTIME_)
        inputMask |= STK_PIPE;
        break;
#else
        usage(args[0]);
#endif

      case 's':
#if defined(__STK_REALTIME_)
        inputMask |= STK_SOCKET;
        break;
#else
        usage(args[0]);
#endif

      case 'm':
#if defined(__STK_REALTIME_)
        inputMask |= STK_MIDI;
        break;
#else
        usage(args[0]);
#endif

      default:
        usage(args[0]);
        break;
      }
    }
    else if ( (args[i][0] == '-') && (args[i][1] == 'o') ) {
      switch(args[i][2]) {

      case 'r':
#if defined(__STK_REALTIME_)
        output[j] = new RtWvOut();
        j++;
        break;
#else
        usage(args[0]);
#endif

      case 'w':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testwav");
        output[j] = new WavWvOut(fileName,1);
        j++;
        break;
          
      case 's':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testsnd");
        output[j] = new SndWvOut(fileName,1);
        j++;
        break;
          
      case 'm':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testmat");
        output[j] = new MatWvOut(fileName,1);
        j++;
        break;

      case 'a':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testaif");
        output[j] = new AifWvOut(fileName,1);
        j++;
        break;
          
      default:
        usage(args[0]);
        break;
      }
    }
    i++;
  }
  return inputMask;
}
