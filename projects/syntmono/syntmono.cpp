// syntmono.cpp
//
// An example STK program for monophonic voice playback and control.

#include "WvOut.h"
#include "Instrmnt.h"
#include "Reverb.h"
#include "PRCRev.h"
#include "SKINI11.msg"
#include "miditabl.h"

// The input control handler.
#include "Controller.h"

// Miscellaneous command-line parsing and instrument allocation
// functions are defined in utilites.cpp.
#include "utilities.h"

Instrmnt *instrument;
WvOut **output;

int main(int argc, char *argv[])
{
  long i, j, nTicks;
  int type;
  int nOutputs = 0;
  int instrumentNo;
  int controlMask;
  bool done;
  MY_FLOAT reverbTime = 0.5;  // in seconds
  MY_FLOAT temp, byte2, byte3;
  MY_FLOAT sample, lastPitch = 200.0;
  Reverb *reverb;
  Controller *controller;

  // Check the command-line arguments for errors and to determine
  // the number of WvOut objects to be instantiated.
  nOutputs = checkArgs(argc, argv);
  output = (WvOut **) malloc(nOutputs * sizeof(WvOut *));

  // Instantiate the instrument from the command-line argument.
  if ( (instrumentNo=newInstByName(argv[1])) < 0 ) usage(argv[0]);

  // Parse the command-line flags, instantiate WvOut objects, and instantiate
  // the input message controller.
  try {
    controlMask = parseArgs(argc, argv);
    controller = new Controller( controlMask );
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }

  if (controlMask)
    instrument->noteOn(lastPitch, 0.7);

  reverb = new PRCRev(reverbTime);
  reverb->setEffectMix(0.2);

  // The runtime loop begins here:
  done = FALSE;
  while (!done) {

    nTicks = controller->getNextMessage();

    if (nTicks == -1)
      done = TRUE;

    for (i=0; i<nTicks; i++) {
      sample = reverb->tick(instrument->tick());
      for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
    }

    type = controller->getType();
    if (type > 0) {
      // parse the input control message

      byte2 = controller->getByte2();
      byte3 = controller->getByte3();

      switch(type) {

      case __SK_NoteOn_:
        if (byte3 == 0) // velocity is zero ... really a NoteOff
          instrument->noteOff(64.0);
        else { // a NoteOn
          lastPitch = __MIDI_To_Pitch[(int)byte2];
          instrument->noteOn(lastPitch, byte3*NORM_7);
        }
        break;

      case __SK_NoteOff_:
        instrument->noteOff(byte3*NORM_7);
        break;

      case __SK_ControlChange_:
        if (byte2 == 44)
          reverb->setEffectMix(byte3*NORM_7);
        else
          instrument->controlChange((int)byte2, byte3);
        break;

      case __SK_AfterTouch_:
        instrument->controlChange(128, byte2);
        break;

      case __SK_PitchBend_:
        temp = byte2 - (int)byte2;   // floating-point remainder
        lastPitch = __MIDI_To_Pitch[(int)byte2] * pow(2.0, temp/12.0) ;
        instrument->setFreq(lastPitch);
        break;

      case __SK_ProgramChange_:
        if (instrumentNo != (int)byte2) {
          instrument->noteOff(1.0);
          // let the old instrument settle a little
          for (i=0; i<4096; i++)    {
            sample = reverb->tick(instrument->tick());
            for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
          }
          delete instrument;
          if ( (instrumentNo=newInstByNum((int)byte2)) < 0 ) {
            // Default instrument = 0
            instrumentNo = newInstByNum(0);
          }
          instrument->noteOn(lastPitch, 0.2);
        }
        break;
      }
    }
  }

  for (i=0;i<reverbTime*SRATE;i++) { /* let the reverb settle a little */
    sample = reverb->tick(instrument->tick());
    for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
  }

  for ( i=0; i<nOutputs; i++ ) delete output[i];
  free(output);

  delete controller;
  delete instrument;
  delete reverb;

	printf("\nsyntmono finished ... goodbye.\n\n");
  return 0;
}
