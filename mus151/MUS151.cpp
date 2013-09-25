/********  Psychoacoustic Interface Program for MUS151  ************/
/********  Center for Computer Research in Music & Acoustics  ******/
/********  Stanford University, by Gary P. Scavone, 1998  **********/

#include "../STK/WvOut.h"
#include "../STK/RTWvOut.h"
#include "../STK/SKINI11.h"
#include "../STK/SKINI11.msg"
#include "TwoOsc.h"
#include "miditabl.h"

int numStrings = 0;
int notDone = 1;
char **inputString;

// The input command pipe and socket threads are defined in threads.cpp.
#include "threads.h"

/* Error function in case of incorrect command-line argument specifications */
void usage(char *func) {
  printf("\nuseage: %s Instr flag\n",func);
  printf("        where Instr = TwoOsc (only one available for now)\n");
  printf("        and flag = -ip for realtime SKINI input by pipe\n");
  printf("                   (won't work under Win95/98),\n");
  printf("        or flag = -is for realtime SKINI input by socket.\n");
  exit(0);
}

int main(int argc,char *argv[])
{
  long i, j, synlength, useSocket = 0;
  int type;
  int outOne = 0;
  MY_FLOAT settleTime = 0.5;  /* in seconds */
  MY_FLOAT temp, byte3, lastPitch;

  if (argc == 3) {
    if (strcmp(argv[1],"TwoOsc")) usage(argv[0]);
    if (!strcmp(argv[2],"-is")) useSocket = 1;
    else if (strcmp(argv[2],"-ip")) usage(argv[0]);
  } else usage(argv[0]);

  TwoOsc *instrument = new TwoOsc;
  WvOut *output = new RTWvOut(SRATE,1);
  SKINI11 *score = new SKINI11();

  // Start the input thread.
  if (useSocket)
    startSocketThread();
  else
    startPipeThread();
  instrument->noteOn(200.0,0.1);

  // The runtime loop begins here:
  notDone = 1;
  synlength = RT_BUFFER_SIZE;
  while(notDone || numStrings)  {
    if (numStrings > 1) synlength = (long) RT_BUFFER_SIZE / numStrings;
    else synlength = RT_BUFFER_SIZE;
    for ( i=0; i<synlength; i++ )  {
      output->tick(instrument->tick());
    }
    if (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0) {
        switch(type) {

        case __SK_NoteOn_:
          // check to see if velocity is zero ... really a NoteOff
          if (( byte3 = score->getByteThree() ) == 0)
            instrument->noteOff(0.0);
          else { // really a NoteOn
            j = (int) score->getByteTwo();
            lastPitch = __MIDI_To_Pitch[j];
            instrument->noteOn(lastPitch,byte3*NORM_7);
          }
          break;

        case __SK_NoteOff_:
          instrument->noteOff(NORM_7*score->getByteThree());
          break;

        case __SK_ControlChange_:
          instrument->controlChange((int)score->getByteTwo(),
                                    score->getByteThree());
          break;

        case __SK_AfterTouch_:
          instrument->controlChange(128,score->getByteTwo());
          break;

        case __SK_PitchBend_:
          temp = score->getByteTwo();
          j = (int) temp;
          temp -= j;   // floating-point remainder
          lastPitch = __MIDI_To_Pitch[j] * pow(2.0,temp / 12.0) ;
          instrument->setFreq(1, lastPitch); /* change osc1 pitch for now */
          break;
        }
      }
      outOne += 1;
      if (outOne == MAX_IN_STRINGS) outOne = 0;
      numStrings--;
    }
  }

  for (i=0;i<settleTime*SRATE;i++) { /* let the sound settle a little */
    output->tick(instrument->tick());
  }

  delete output;
  delete score;
  delete instrument;

	printf("MUS151 finished.\n");
  return 0;
}
