/******************************************/
/*
  Example program to write N sine tones to
  an N channel soundfile
  by Gary P. Scavone, 2000

  This program is currently written to write
  an N channel WAV file.  However, it is
  simple to replace the instance of WavWvOut
  with any other WvOut subclass.
*/
/******************************************/

#include "WavWvOut.h"
#include "RawWvIn.h"

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: sineN N file time \n");
  printf("    where N = number of channels (sines),\n");
  printf("    file = the .wav file to create,\n");
  printf("    and time = the amount of time to record (in seconds).\n\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  // minimal command-line checking
  if (argc != 4) usage();

  int chans = (int) atoi(argv[1]);
  float time = atof(argv[3]);
  int i;

  // Define and load the rawwave file(s) ... the path is critical
  RawWvIn *oscs[chans];
  try {
    for (i=0; i<chans; i++)
      oscs[i] = new RawWvIn("../../rawwaves/sinewave.raw", "looping");
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }
  // Set oscillator frequency(ies) here ... somewhat random
  float base_freq = 220.0;
  for (i=0; i<chans; i++)
    oscs[i]->setFreq(base_freq + i*(45.0));

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
  i=0;
  int samples = (int) (time*SRATE);
  MY_FLOAT *outvec = (MY_FLOAT *) new MY_FLOAT[chans];
  while (i<samples) {
    for (int j=0; j<chans; j++)
      outvec[j] = oscs[j]->tick();
    output->mtick(outvec);
    i++;
  }

  // Clean up
  for (i=0; i<chans; i++)
    delete oscs[i];
  delete [] outvec;
  delete output;
}
