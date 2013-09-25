// Miscellaneous parsing and error functions for use with syntmono.
//
// Gary P. Scavone, 1999.

#include "utilities.h"

// STK Instrument Classes
#include "../STK/Object.h"
#include "../STK/Clarinet.h"
#include "../STK/Flute.h"
#include "../STK/Brass.h"
#include "../STK/Bowed.h"
#include "../STK/Plucked.h"
#include "../STK/Mandolin.h"
#include "../STK/Marimba.h"
#include "../STK/Vibraphn.h"
#include "../STK/AgogoBel.h"
#include "../STK/Rhodey.h"
#include "../STK/Wurley.h"
#include "../STK/TubeBell.h"
#include "../STK/HeavyMtl.h"
#include "../STK/PercFlut.h"
#include "../STK/BeeThree.h"
#include "../STK/Moog1.h"
#include "../STK/FMVoices.h"
#include "../STK/VoicForm.h"
#include "../STK/DrumSynt.h"
#include "../STK/Shakers.h"
#include "../STK/BowedBar.h"

// STK WvOut classes
#include "../STK/WvOut.h"
#include "../STK/SndWvOut.h"
#include "../STK/WavWvOut.h"
#include "../STK/MatWvOut.h"
#if defined(__STK_REALTIME_)
  #include "../STK/RTWvOut.h"
#endif

#define NUM_INSTS 21

// The order of the following list is important.  The location of a particular
// instrument in the list should correspond to that instrument's ProgramChange
// number (i.e. Clarinet = ProgramChange 0).
char insts[NUM_INSTS][10] = { "Clarinet", "Flute", "Brass", "Bowed", "Plucked",
                              "Mandolin", "Marimba", "Vibraphn", "AgogoBel", 
                              "Rhodey", "Wurley", "TubeBell", "HeavyMtl",
                              "PercFlut", "BeeThree", "Moog1", "FMVoices",
                              "VoicForm", "DrumSynt", "Shakers", "BowedBar"};

int newInstByNum(int instNum)
{
  int temp = instNum;
  extern Instrmnt *instrument;
  
  if      (instNum==0) instrument = new Clarinet(20.0);
  else if (instNum==1) instrument = new Flute(20.0);
  else if (instNum==2) instrument = new Brass(20.0);
  else if (instNum==3) instrument = new Bowed(20.0);
            
  else if (instNum==4) instrument = new Plucked(20.0);
  else if (instNum==5) instrument = new Mandolin(20.0);
  else if (instNum==6) instrument = new Marimba;
  else if (instNum==7) instrument = new Vibraphn;
  else if (instNum==8) instrument = new AgogoBel;
  else if (instNum==9) instrument = new Rhodey;
  else if (instNum==10) instrument = new Wurley;
  else if (instNum==11) instrument = new TubeBell;
            
  else if (instNum==12) instrument = new HeavyMtl;
  else if (instNum==13) instrument = new PercFlut;
  else if (instNum==14) instrument = new BeeThree;
  else if (instNum==15) instrument = new Moog1;
            
  else if (instNum==16) instrument = new FMVoices;
  else if (instNum==17) instrument = new VoicForm;
            
  else if (instNum==18) instrument = new DrumSynt;
  else if (instNum==19) instrument = new Shakers;
  else if (instNum==20) instrument = new BowedBar;
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

void errorfun(char *func) {
  /* Error function in case of incorrect command-line argument specifications */
  int i,j;

  printf("\nuseage: %s Instr flag(s) \n",func);
  printf("        where flag = -s <file name> for .snd output file,\n");
  printf("        -w <file name> for .wav output file,\n");
  printf("        -m <file name> for .mat output file,\n");
#if defined(__STK_REALTIME_)
  printf("        -r for realtime output,\n");
  printf("        -ip for realtime input by pipe (versus scorefile),\n");
  printf("         (won't work under Win95/98),\n");
  printf("        -is for realtime input by socket (versus scorefile),\n");
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

int checkArgs(int numArgs, char *args[])
{
  int w, i = 2, j = 0;
  int numOutputs = 0;
  char flags[16] = "";

  if (numArgs < 3 || numArgs > 10) errorfun(args[0]);

  while (i < numArgs) {
    if (args[i][0] == '-') {
      if ( (args[i][1] == 'r') || (args[i][1] == 's') ||
           (args[i][1] == 'w') || (args[i][1] == 'm') )
        numOutputs++;
      else if (args[i][1] == 'i') {
        if ( (args[i][2] != 's') && (args[i][2] != 'p') ) errorfun(args[0]);
      }
      else errorfun(args[0]);
      flags[j] = args[i][1];
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
        errorfun(args[0]);
      }
      w++;
    }
  }

  /* Make sure we have at least one output type */
  if (numOutputs < 1) errorfun(args[0]);

  return numOutputs;
}

int parseArgs(int numArgs, char *args[])
{
  int i = 2, j = 0;
  int realtime = 0;
  char fileName[256];
  extern WvOut **output;

  while (i < numArgs) {
    if (args[i][0] == '-') {
      switch(args[i][1]) {

      case 'i':
#if defined(__STK_REALTIME_)
        if (args[i][2] == 's') realtime = 2;
        else realtime = 1;
        break;
#else
        errorfun(args[0]);
#endif

      case 'r':
#if defined(__STK_REALTIME_)
        output[j] = new RTWvOut(SRATE,1);
        j++;
        break;
#else
        errorfun(args[0]);
#endif

      case 'w':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testwav");
        output[j] = new WavWvOut(1,fileName);
        j++;
        break;
          
      case 's':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testsnd");
        output[j] = new SndWvOut(1,fileName);
        j++;
        break;
          
      case 'm':
        if ((i+1 < numArgs) && args[i+1][0] != '-') {
          i++;
          strcpy(fileName,args[i]);
        }
        else strcpy(fileName,"testmat");
        output[j] = new MatWvOut(1,fileName);
        j++;
        break;
          
      default:
        errorfun(args[0]);
        break;
      }
    }
    i++;
  }
  return realtime;
}
