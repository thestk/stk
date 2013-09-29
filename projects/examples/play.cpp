/******************************************/
/*
  Example program to play an N channel
  soundfile.

  This program will load WAV, SND, AIF, and
  MAT-file formatted files of various data
  types.  If the audio system does not support
  the number of channels or sample rate of
  the soundfile, the program will stop.

  By Gary P. Scavone, 2000 - 2002.
*/
/******************************************/

#include "RtWvOut.h"
#include "WvIn.h"
#include <stdlib.h>

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: play file <rate>\n");
  printf("    where file = the file to play,\n");
  printf("    and rate = an optional playback rate.\n");
  printf("               (default = 1.0, can be negative)\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // Minimal command-line checking.
  if (argc < 2 || argc > 3) usage();

  // Initialize our WvIn/WvOut pointers.
  RtWvOut *output = 0;
  WvIn *input = 0;

  // Try to load the soundfile.
  try {
    input = new WvIn( argv[1] );
  }
  catch (StkError &) {
    exit(0);
  }

  // Set input read rate based on the default STK sample rate.
  float rate = 1.0;
  rate = input->getFileRate() / Stk::sampleRate();
  if ( argc == 3 ) rate *= atof(argv[2]);
  input->setRate( rate );

  // Find out how many channels we have.
  int channels = input->getChannels();

  // Define and open the realtime output device
  try {
    output = new RtWvOut( channels, Stk::sampleRate(), 0, RT_BUFFER_SIZE, 4 );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Here's the runtime loop.
  while (!input->isFinished()) {
    output->tickFrame( input->tickFrame() );
  }

 cleanup:
  delete input;
  delete output;
  return 0;
}
