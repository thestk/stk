// demo.cpp
//
// An STK program for monophonic voice playback and control.

#include "SKINI.msg"
#include "Messager.h"
#include "WvOut.h"
#include "Instrmnt.h"
#include "PRCRev.h"

// Miscellaneous command-line parsing and instrument allocation
// functions are defined in utilites.cpp ... specific to this program.
#include "utilities.h"

#include <stdlib.h>
#include <math.h>
#include <iostream.h>

int main(int argc, char *argv[])
{
  bool done;
  Instrmnt *instrument = 0;
  Messager *messager = 0;
  WvOut **output = 0;
  Reverb *reverb = 0;
  MY_FLOAT pitch = 220.0;
  MY_FLOAT t60 = 1.0;  // in seconds

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!!
  Stk::setSampleRate( 22050.0 );

  // Check the command-line arguments for errors and to determine
  // the number of WvOut objects to be instantiated (in utilities.cpp).
  int nOutputs = checkArgs(argc, argv);
  output = (WvOut **) calloc(nOutputs, sizeof(WvOut *));

  // Instantiate the instrument from the command-line argument.
  int voice = voiceByName(argv[1], &instrument);
  if ( voice < 0 ) usage(argv[0]);

  // Parse the command-line flags, instantiate WvOut objects, and instantiate
  // the input message controller (in utilities.cpp).
  try {
    int controlMask = parseArgs(argc, argv, output);
    messager = new Messager( controlMask );
  }
  catch (StkError &) {
    goto cleanup;
  }

  reverb = new PRCRev( t60 );
  reverb->setEffectMix(0.2);

  // The runtime loop begins here:
  done = FALSE;
  int nTicks, type, j, i;
  MY_FLOAT temp, byte2, byte3, sample;
  while (!done) {

    // Look for new messages and return a delta time (in samples).
    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();

    for ( i=0; i<nTicks; i++ ) {
      sample = reverb->tick( instrument->tick() );
      for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
    }

    if ( type > 0 ) {
      // Process the new control message.
      byte2 = messager->getByteTwo();
      byte3 = messager->getByteThree();

      switch(type) {

      case __SK_NoteOn_:
        if (byte3 == 0.0) // velocity is zero ... really a NoteOff
          instrument->noteOff( 0.5 );
        else { // a NoteOn
          if ( byte2 < 0.0 || byte2 > 128.0 ) continue;
          pitch = Midi2Pitch[(unsigned int) byte2];
          instrument->noteOn(pitch, byte3 * ONE_OVER_128);
        }
        break;

      case __SK_NoteOff_:
        instrument->noteOff(byte3 * ONE_OVER_128);
        break;

      case __SK_ControlChange_:
        if (byte2 == 44.0)
          reverb->setEffectMix(byte3  * ONE_OVER_128);
        else
          instrument->controlChange( (int)byte2, byte3 );
        break;

      case __SK_AfterTouch_:
        instrument->controlChange( 128, byte2 );
        break;

      case __SK_PitchBend_:
        if ( byte2 < 0.0 || byte2 > 128.0 ) continue;
        temp = byte2 - (int)byte2;   // floating-point remainder
        pitch = Midi2Pitch[(unsigned int)byte2] * pow(2.0, temp/12.0);
        instrument->setFrequency(pitch);
        break;

      case __SK_ProgramChange_:
        if (voice != (int)byte2) {
          instrument->noteOff(1.0);
          // Let the instrument settle a bit.
          for (i=0; i<4096; i++)    {
            sample = reverb->tick( instrument->tick() );
            for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
          }
          delete instrument;
          voice = voiceByNumber( (int)byte2, &instrument );
          if ( voice < 0 ) {
            // Default instrument = 0
            voice = voiceByNumber( 0, &instrument );
          }
          instrument->noteOn(pitch, 0.2);
        }
      }
    }
  }

  // Let the reverb settle a bit.
  nTicks = (long) (t60 * Stk::sampleRate());
  for ( i=0; i<nTicks; i++) {
    sample = reverb->tick( instrument->tick() );
    for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
  }

 cleanup:

  for ( i=0; i<nOutputs; i++ ) delete output[i];
  free(output);

  delete messager;
  delete instrument;

	cout << "\ndemo finished ... goodbye.\n" << endl;
  return 0;
}
