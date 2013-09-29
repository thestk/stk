// demo.cpp
//
// An example STK program for voice playback and control.

#include "SKINI.msg"
#include "WvOut.h"
#include "Instrmnt.h"
#include "PRCRev.h"
#include "Voicer.h"

// Miscellaneous command-line parsing and instrument allocation
// functions are defined in utilites.cpp ... specific to this program.
#include "utilities.h"

#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <iostream>

bool done;
static void finish(int ignore){ done = true; }

int main(int argc, char *argv[])
{
  Instrmnt **instrument = 0;
  WvOut **output = 0;
  Messager *messager = 0;
  Reverb *reverb = 0;
  Voicer *voicer = 0;
  int i, nVoices = 1;
  MY_FLOAT volume = 1.0;
  MY_FLOAT t60 = 1.0;  // in seconds

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!  If the sample rate is
  // specified in the command line, it will override this setting.
  Stk::setSampleRate( 44100.0 );

  // Check the command-line arguments for errors and to determine
  // the number of WvOut objects to be instantiated (in utilities.cpp).
  int nOutputs = checkArgs(argc, argv);
  output = (WvOut **) calloc(nOutputs, sizeof(WvOut *));

  // Instantiate the instrument(s) type from the command-line argument
  // (in utilities.cpp).
  nVoices = countVoices(argc, argv);
  instrument = (Instrmnt **) calloc(nVoices, sizeof(Instrmnt *));
  int voice = voiceByName(argv[1], &instrument[0]);
  if ( voice < 0 ) {
    free( output );
    free( instrument );
    usage(argv[0]);
  }
  // If there was no error allocating the first voice, we should be fine for more.
  for ( i=1; i<nVoices; i++ )
    voiceByName(argv[1], &instrument[i]);

  voicer = (Voicer *) new Voicer(nVoices);
  for ( i=0; i<nVoices; i++ )
    voicer->addInstrument( instrument[i] );

  // Parse the command-line flags, instantiate WvOut objects, and
  // instantiate the input message controller (in utilities.cpp).
  try {
    parseArgs(argc, argv, output, &messager);
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Set the number of ticks between realtime messages (default =
  // RT_BUFFER_SIZE).
  messager->setRtDelta( 64 );

  // Set the reverb parameters.
  reverb = new PRCRev( t60 );
  reverb->setEffectMix(0.2);

  // Install an interrupt handler function.
	(void) signal(SIGINT, finish);

  // The runtime loop begins here:
  done = FALSE;
  int nTicks, type, j;
  MY_FLOAT byte2, byte3, sample;
  while (!done) {

    // Look for new messages and return a delta time (in samples).
    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();
    for ( i=0; i<nTicks; i++ ) {
      sample = volume * reverb->tick( voicer->tick() );
      for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
    }

    if ( type > 0 ) {
      // Process the new control message.
      byte2 = messager->getByteTwo();
      byte3 = messager->getByteThree();

      switch(type) {

      case __SK_NoteOn_:
        if (byte3 == 0.0) // velocity is zero ... really a NoteOff
          voicer->noteOff( byte2, 64.0 );
        else // a NoteOn
          voicer->noteOn( byte2, byte3 );
        break;

      case __SK_NoteOff_:
        voicer->noteOff( byte2, byte3 );
        break;

      case __SK_ControlChange_:
        if (byte2 == 44.0)
          reverb->setEffectMix(byte3  * ONE_OVER_128);
        else if (byte2 == 7.0)
          volume = byte3 * ONE_OVER_128;
        else if (byte2 == 49.0)
          voicer->setFrequency( byte3 );
        else
          voicer->controlChange( (int) byte2, byte3 );
        break;

      case __SK_AfterTouch_:
        voicer->controlChange( 128, byte2 );
        break;

      case __SK_PitchChange_:
        voicer->setFrequency( byte2 );
        break;

      case __SK_PitchBend_:
        voicer->pitchBend( byte2 );
        break;

      case __SK_Volume_:
        volume = byte2 * ONE_OVER_128;
        break;

      case __SK_ProgramChange_:
        if ( voice != (int) byte2 ) {
          voicer->silence();
          // Let the instrument(s) settle a bit.
          for ( i=0; i<4096; i++ ) {
            sample = reverb->tick( voicer->tick() );
            for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
          }
          for ( i=0; i<nVoices; i++ ) {
            voicer->removeInstrument( instrument[i] );
            delete instrument[i];
            voice = voiceByNumber( (int)byte2, &instrument[i] );
            if ( voice < 0 )
              voice = voiceByNumber( 0, &instrument[i] );
            voicer->addInstrument( instrument[i] );
          }
        }
      }
    }
  }

  // Let the reverb settle a bit.
  nTicks = (long) (t60 * Stk::sampleRate());
  for ( i=0; i<nTicks; i++) {
    sample = reverb->tick( voicer->tick() );
    for ( j=0; j<nOutputs; j++ ) output[j]->tick(sample);
  }

 cleanup:

  for ( i=0; i<nOutputs; i++ ) delete output[i];
  free(output);

  delete messager;
  delete reverb;
  delete voicer;

  for ( i=0; i<nVoices; i++ ) delete instrument[i];
  free(instrument);

	std::cout << "\nStk demo finished ... goodbye.\n" << std::endl;
  return 0;
}


