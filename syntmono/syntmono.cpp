// syntmono.cpp
//
// An example STK program for monophonic voice playback and control.

#include "../STK/WvOut.h"
#include "../STK/Instrmnt.h"
#include "../STK/Reverb.h"
#include "../STK/NRev.h"
#include "../STK/PRCRev.h"
#include "../STK/JCRev.h"
#include "../STK/SKINI11.h"
#include "../STK/SKINI11.msg"
#include "miditabl.h"

int numStrings = 0;
int notDone = 1;
char **inputString;

// Miscellaneous command-line parsing and instrument allocation
// functions are defined in utilites.cpp.
#include "utilities.h"

// The input command pipe and socket threads are defined in threads.cpp.
#include "threads.h"

Instrmnt *instrument;
WvOut **output;

void main(int argc,char *argv[])
{
  long i, j, synlength;
  int type, rtInput = 0;
  int numOuts = 0, outOne = 0;
  int instNum;
  char *fin;
  MY_FLOAT reverbTime = 0.5;  // in seconds
  MY_FLOAT temp, byte3, lastPitch, outSample;
  
  // Check the command-line arguments for errors and to determine
  // the number of WvOut objects to be instantiated.
  numOuts = checkArgs(argc, argv);
  output = (WvOut **) malloc(numOuts * sizeof(WvOut *));

  // Instantiate the instrument from the command-line argument.
  if ( (instNum=newInstByName(argv[1])) < 0 ) errorfun(argv[0]);

  // Parse the command-line flags and instantiate WvOut objects.
  rtInput = parseArgs(argc, argv);

  SKINI11 *score = new SKINI11();
  Reverb *reverb = new PRCRev(reverbTime);
  reverb->setEffectMix(0.2);


  //  If using realtime input, start the input thread.
  if (rtInput) {
#if defined(__STK_REALTIME_)
    if (rtInput == 2) startSocketThread();
    else startPipeThread();
    instrument->noteOn(200.0,0.1);
#endif
  }
  else { // Malloc inputString as 1 x STRING_LEN array
    inputString = (char **) malloc(sizeof(char *));
    inputString[0] = (char *) malloc(STRING_LEN * sizeof(char));
  }

  // The runtime loop begins here:
  notDone = 1;
  synlength = RT_BUFFER_SIZE;
  while(notDone || numStrings)  {
    if (rtInput) {
      if (numStrings > 1) synlength = (long) RT_BUFFER_SIZE / numStrings;
      else synlength = RT_BUFFER_SIZE;
      for ( i=0; i<synlength; i++ )  {
        outSample = reverb->tick(instrument->tick());
        for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
      }
    }
    else {
      fin = fgets(inputString[0],STRING_LEN,stdin);
      if (fin == NULL) notDone = 0;
      else {
        numStrings++;
      } 
    }
    if (numStrings) {
      score->parseThis(inputString[outOne]);
      type = score->getType();
      if (type > 0) {
        if (temp = score->getDelta()) { /* SKINI score file */
          synlength = (long) (temp * SRATE);
          for ( i=0; i<synlength; i++ )  {
            outSample = reverb->tick(instrument->tick());
            for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
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
          instrument->setFreq(lastPitch);
        }
        else if (type == __SK_ProgramChange_)   {
          j = (int) score->getByteTwo();
          if (j != instNum) {
            instrument->noteOff(1.0);
            for (i=0;i<4096;i++)    {
              outSample = reverb->tick(instrument->tick());
              for ( int k=0; k<numOuts; k++ ) output[k]->tick(outSample);
            }
            delete instrument;
            if ( (instNum=newInstByNum(j)) < 0 ) {
              // Default instrument = 0
              instNum = newInstByNum(0);
            }
            instrument->noteOn(lastPitch, 0.2);
          }
        }
      }
      if (rtInput) {
        outOne += 1;
        if (outOne == MAX_IN_STRINGS) outOne = 0;
      }
      numStrings--;
    }
  }

  for (i=0;i<reverbTime*SRATE;i++) { /* let the reverb settle a little */
    outSample = reverb->tick(instrument->tick());
    for ( j=0; j<numOuts; j++ ) output[j]->tick(outSample);
  }

  for ( i=0; i<numOuts; i++ ) delete output[i];
  if (!rtInput) { // Free the inputString for non-realtime operation.
    free(inputString[0]);
    free(inputString);
  }
  free(output);
  delete score;
  delete instrument;
  delete reverb;

	printf("syntmono finished ... goodbye.\n");
}
