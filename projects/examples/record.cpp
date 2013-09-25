/******************************************/
/*
  Example program to record N channels of data
  by Gary P. Scavone, 2000

  This program is currently written to read
  from a realtime audio input device and to
  write to a WAV output file.  However, it
  is simple to replace the FILE_TYPE specifier
  to WvOut with another file type.
*/
/******************************************/

#include "RtWvIn.h"
#include "WvOut.h"

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: record N file time fs \n");
  printf("    where N = number of channels,\n");
  printf("    file = the .wav file to create,\n");
  printf("    time = the amount of time to record (in seconds),\n");
  printf("    and fs = the sample rate.\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // minimal command-line checking
  if (argc != 5) usage();

  int channels = (int) atoi(argv[1]);
  float sample_rate = atof(argv[4]);
  float time = atof(argv[3]);
  long samples, i;

  // Set the global sample rate.
  Stk::setSampleRate( sample_rate );

  // Initialize our WvIn/WvOut pointers.
  RtWvIn *input = 0;
  WvOut *output = 0;

  // Open the realtime input device
  try {
    input = new RtWvIn(channels);
  }
  catch (StkError &) {
    exit(0);
  }
  
  // Open the soundfile for output.
  try {
    output = new WvOut(argv[2], channels, WvOut::WVOUT_WAV);
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Here's the runtime loop
  samples = (long) (time * Stk::sampleRate());
  for ( i=0; i<samples; i++ )
    output->tickFrame( input->tickFrame() );

 cleanup:
  delete input;
  delete output;
  return 0;
}
