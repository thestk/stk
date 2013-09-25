/******************************************/
/*
  Example program for realtime input/output
  by Gary P. Scavone, 2000

  This program reads N channels of realtime
  audio input for a specified amount of time
  and immediately play them back in realtime
  (duplex mode).  This program also demonstrates
  the use of FIFO scheduling priority.  To be
  run with such priority, the program must be
  set suid (chmod +s) and owned by root.
*/
/******************************************/

#include "RtDuplex.h"

#if !defined(__OS_Win_)
  #include <sched.h>
#endif

void usage(void) {
  /* Error function in case of incorrect command-line
     argument specifications
  */
  printf("\nuseage: ioN N time \n");
  printf("    where N = number of channels,\n");
  printf("    and time = the amount of time to record (in seconds).\n\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  int i=0;

  // minimal command-line checking
  if (argc != 3) usage();

  int chans = (int) atoi(argv[1]);
  float time = atof(argv[2]);
  float sample_rate = SRATE;
  MY_FLOAT *inSamples;
  MY_FLOAT *lastSamples;

  // allocate the lastSamples array
  lastSamples = (MY_FLOAT *) new MY_FLOAT[chans];
  for (i=0; i<chans; i++)
    lastSamples[i] = 0.0;

  // Define and open the realtime duplex device
  RtDuplex *inout;
  try {
    inout = new RtDuplex(chans, sample_rate);
  }
  catch (StkError& m) {
    m.printMessage();
    exit(0);
  }

#if !defined(__OS_Win_)
  // set schedulling priority to SCHED_FIFO
  struct sched_param p;
  int min, max, priority;

  if (!getuid() || !geteuid()) {
    min=sched_get_priority_min(SCHED_FIFO);
    max=sched_get_priority_max(SCHED_FIFO);
    priority=min+(max-min)/2;
    p.sched_priority=priority;
    if (sched_setscheduler(0, SCHED_FIFO, &p)==-1) {
	    fprintf(stderr, "\ncould not activate scheduling with priority %d\n", priority);
    }
    seteuid(getuid());
  }
#endif

  // Here's the runtime loop

  int j=0;
  int samples = (int) (time*SRATE);
  while (i<samples) {
    inSamples = inout->mtick(lastSamples);
    for (int k=0; k<chans; k++)
      lastSamples[k] = inSamples[k];
    i++;
#if !defined(__OS_Win_)
    j++;
    if (j>=256) {
      sched_yield();
      j = 0;
    }
#endif
  }

  // Clean up
  delete inout;
  delete [] lastSamples;
}
