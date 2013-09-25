/******************************************/
/*
  Example program to output N channels of audio
  data over an ethernet socket connection.

  by Gary P. Scavone, 2000

  This program is currently written to load
  a WAV file for streaming.  However, it is
  simple to replace the instance of WavWvIn
  with any other WvIn subclass.

  The class StrmWvOut first attempts to
  establish a socket connection to a socket
  server running on port 2005.  Thus, this
  program needs to be started after the
  streaming server.
*/
/******************************************/

#include "WavWvIn.h"
#include "StrmWvOut.h"

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: streamOutN N file host fs \n");
  printf("    where N = number of channels,\n");
  printf("    file = the .wav file to load,\n");
  printf("    host = the hostname of the receiving app,\n");
  printf("    and fs = the sample rate.\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // minimal command-line checking
  if (argc != 5) usage();

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
  input->setRate(atof(argv[4])/SRATE);

  // Define and open the realtime output device
  WvOut *output;
  try {
    output = new StrmWvOut(2005, (char *)argv[3], chans);
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
