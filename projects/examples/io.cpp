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
#include <stdio.h>

#if defined(__OS_LINUX_) || defined(__OS_IRIX__)
  #include <sched.h>
#endif

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: io N time \n");
  printf("    where N = number of channels,\n");
  printf("    and time = the amount of time to run (in seconds).\n\n");
  exit(0);
}

int
main(int argc, char *argv[])
{
  // Minimal command-line checking.
  if (argc != 3) usage();

  unsigned int channels = (unsigned int) atoi(argv[1]);
  float time = atof(argv[2]);

  // Open the realtime duplex device.
  RtDuplex *inout = 0;
  try {
    inout = new RtDuplex(channels, Stk::sampleRate(), 0, RT_BUFFER_SIZE, 10);
  }
  catch (StkError &) {
    exit(0);
  }

#if defined(__OS_LINUX__) || defined(__OS_IRIX__)
  // Set schedulling priority to SCHED_FIFO.
  struct sched_param p;
  int min, max, priority;

  if (!getuid() || !geteuid()) {
    min=sched_get_priority_min(SCHED_FIFO);
    max=sched_get_priority_max(SCHED_FIFO);
    priority=min+(max-min)/2;
    p.sched_priority=priority;
    if (sched_setscheduler(0, SCHED_FIFO, &p)==-1) {
	    fprintf(stderr, "\nCould not activate scheduling with priority %d\n", priority);
    }
    seteuid(getuid());
  }
#endif

  // Here's the runtime loop
  unsigned long i, counter = 0;
  MY_FLOAT *newFrame = new MY_FLOAT[channels];
  const MY_FLOAT *lastFrame = inout->lastFrame();
  unsigned long samples = (unsigned long) (time * Stk::sampleRate());
  while ( counter < samples ) {
    for ( i=0; i<channels; i++ )
      newFrame[i] = lastFrame[i];
    lastFrame = inout->tickFrame( newFrame );
    counter++;
#if defined(__OS_LINUX__) || defined(__OS_IRIX__)
    if ( counter % 1024 == 0 )
      sched_yield();
#endif
  }

  // Clean up
  delete [] newFrame;
  delete inout;
  return 0;
}
