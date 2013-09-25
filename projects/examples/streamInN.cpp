/******************************************/
/*
  Example program to read N channels of audio
  data that are streamed over an ethernet
  connection.

  by Gary P. Scavone, 2000

  This program is currently written to play
  the input data in realtime.  However, it
  is simple to replace the instance of
  RtWvOut with any other WvOut subclass.

  The class StrmWvIn sets up a socket server
  and waits for a connection.  Thus, this
  program needs to be started before the
  streaming client.  This program will
  terminate when the socket connection is
  closed.
*/
/******************************************/

#include "Object.h"
#include "StrmWvIn.h"
#include "RtWvOut.h"

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: streamInN N fs \n");
  printf("    where N = number of channels,\n");
  printf("    and fs = the sample rate.\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // minimal command-line checking
  if (argc != 3) usage();

  int chans = (int) atoi(argv[1]);

  WvIn *input;
  try {
    input = new StrmWvIn(chans);
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }
  // Set playback rate here
  input->setRate(atof(argv[2])/SRATE);

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
