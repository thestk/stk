/******************************************/
/*
  Example program to play an N channel soundfile
  by Gary P. Scavone, 2000

  This program is currently written to load
  a WAV file and play it in realtime.  However,
  it is simple to replace the instance of
  WavWvIn with any other WvIn subclass.
  Likewise, RtWvOut can be replaced with any
  other WvOut subclass.
*/
/******************************************/

#include "RtWvOut.h"
#include "WavWvIn.h"

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: playN N file fs \n");
  printf("    where N = number of channels,\n");
  printf("    file = the .wav file to play,\n");
  printf("    and fs = the sample rate.\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // minimal command-line checking
  if (argc != 4) usage();

  int chans = (int) atoi(argv[1]);

  // Define and load the SND soundfile
  WvIn *input;
  try {
    input = new WavWvIn((char *)argv[2], "oneshot");
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }
  // Set playback rate here
  input->setRate(atof(argv[3])/SRATE);

  // Define and open the realtime output device
  WvOut *output;
  try {
    output = new RtWvOut(chans);
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }

  // Here's the runtime loop
  while (!input->isFinished()) {
    output->mtick(input->mtick());
  }

  // Clean up
  delete input;
  delete output;
}
