/******************************************/
/*
  Example program to output N channels of audio
  data over a TCP network socket connection.

  by Gary P. Scavone, 2000

  This program will load a specified WAV,
  SND, AIFF, STK RAW, or MAT-file formatted
  file.  The output data format is set for
  signed 16-bit integers.  However, it is
  easy to change the TcpWvOut setting to
  any of the other defined StkFormats.
  If using tcpIn, it will be necessary to
  change the expected data format there
  as well.

  The class StrmWvOut first attempts to
  establish a socket connection to a socket
  server running on port 2006.  Therefore,
  this program needs to be started AFTER the
  streaming server.
*/
/******************************************/

#include "WvIn.h"
#include "TcpWvOut.h"
#include <stdlib.h>

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: tcpOut file host <rate>\n");
  printf("    where file = the file to load,\n");
  printf("    host = the hostname where the receiving\n");
  printf("           application is running.\n");
  printf("    and rate = an optional playback rate for the file.\n");
  printf("               (default = 1.0, can be negative)\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // Minimal command-line checking.
  if (argc < 3 || argc > 4) usage();

  // Initialize the object pointers.
  TcpWvOut *output = 0;
  WvIn *input = 0;

  // Load the file.
  try {
    input = new WvIn( (char *)argv[1] );
  }
  catch (StkError &) {
    exit(0);
  }

  // Set the global STK sample rate to the file rate.
  Stk::setSampleRate( input->getFileRate() );

  // Set input read rate.
  double rate = 1.0;
  if ( argc == 4 ) rate = atof(argv[3]);
  input->setRate( rate );

  // Find out how many channels we have.
  int channels = input->getChannels();

  // Define and open the output device
  try {
    output = new TcpWvOut(2006, (char *)argv[2], channels, Stk::STK_SINT16);
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Here's the runtime loop
  while ( !input->isFinished() )
    output->tickFrame( input->tickFrame() );

 cleanup:
  delete input;
  delete output;
  return 0;
}
