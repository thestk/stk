/******************************************/
/*
  Example program to write N sine tones to
  an N channel soundfile.

  By default, the program will write an
  N channel WAV file.  However, it is
  simple to change the file type argument
  in the WvOut constructor.

  By Gary P. Scavone, 2000 - 2002.
*/
/******************************************/

#include "WaveLoop.h"
#include "WvOut.h"
#include <stdlib.h>

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: sine N file time fs\n");
  printf("    where N = number of channels (sines),\n");
  printf("    file = the .wav file to create,\n");
  printf("    time = the amount of time to record (in seconds),\n");
  printf("    and fs = the sample rate (in Hz).\n\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  float base_freq = 220.0;
  int samples;
  int i;

  // Minimal command-line checking.
  if (argc != 5) usage();

  int channels = (int) atoi(argv[1]);
  double time = atof(argv[3]);
  double srate = atof(argv[4]);

  // Initialize our object and data pointers.
  WvOut *output = 0;
  StkFloat *vector = 0;
  WaveLoop **oscs = (WaveLoop **) malloc( channels * sizeof(WaveLoop *) );
  for (i=0; i<channels; i++) oscs[i] = 0;

  // If you want to change the default sample rate (set in Stk.h), do
  // it before instantiating any objects!!
  Stk::setSampleRate( srate );

  // Define and load the rawwave file(s) ... the path is critical.
  try {
    for (i=0; i<channels; i++)
      oscs[i] = new WaveLoop( "../../rawwaves/sinewave.raw", true );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Set oscillator frequency(ies) here ... somewhat random.
  for (i=0; i<channels; i++)
    oscs[i]->setFrequency( base_freq + i*(45.0) );

  // Define and open the soundfile for output.  Other file
  // format options include: WVOUT_SND, WVOUT_AIF, WVOUT_MAT,
  // and WVOUT_RAW.  Other data type options include:
  // STK_SINT8, STK_SINT32, StkFloat32, and StkFloat64.
  try {
    output = new WvOut( argv[2], channels, WvOut::WVOUT_WAV, Stk::STK_SINT16 );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Here's the runtime loop
  samples = (int) ( time * Stk::sampleRate() );
  vector = (StkFloat *) new StkFloat[channels];
  for ( i=0; i<samples; i++ ) {
    for (int j=0; j<channels; j++) {
      vector[j] = oscs[j]->tick();
    }
    output->tickFrame(vector);
  }

 cleanup:
  for (i=0; i<channels; i++)
    delete oscs[i];
  free(oscs);
  delete [] vector;
  delete output;
  return 0;
}
