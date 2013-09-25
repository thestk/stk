/******************************************/
/*
  Example program to record N channels of data
  by Gary P. Scavone, 2000

  This program is currently written to read
  from a realtime audio input device and to
  write to a WAV output file.  However, it
  is simple to replace the instance of
  RtWvIn with any other WvIn subclass.
  Likewise, WavWvOut can be replaced with any
  other WvOut subclass.
*/
/******************************************/

#include "RtWvIn.h"
#include "WavWvOut.h"

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: recordN N file time fs \n");
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

  int chans = (int) atoi(argv[1]);
  float sample_rate = atof(argv[4]);
  float time = atof(argv[3]);

  // Define and open the realtime input device
  WvIn *input;
  try {
    input = new RtWvIn(chans, sample_rate);
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }
  
  // Define and open the soundfile for output
  WvOut *output;
  try {
    output = new WavWvOut(argv[2],chans);
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }

  // Here's the runtime loop
  int i=0;
  int samples = (int) (time*SRATE);
  while (i<samples) {
    output->mtick(input->mtick());
    i++;
  }

  // Clean up
  delete input;
  delete output;
}
