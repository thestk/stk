/******************************************/
/*
  Example program to read N channels of audio
  data that are streamed over a TCP network
  connection.

  by Gary P. Scavone, 2000

  NOTE: This program makes use of blocking audio
  input/output routines.  On systems where the
  underlying audio API is based on a callback scheme
  (Macintosh OS-X, Windows ASIO, and Linux JACK), these
  routines are not fully robust (over/underruns can
  happen with some frequency).  See the STK tutorial
  for example programs using callback schemes and/or
  visit the RtAudio tutorial page
  (http://music.mcgill.ca/~gary/rtaudio/) for more
  information.

  This program is currently written to play
  the input data in realtime.  However, it
  is simple to replace the instance of
  RtWvOut with WvOut for writing to a
  soundfile.

  The streamed data format is assumed to be
  signed 16-bit integers.  However, both
  TcpWvIn and TcpWvOut can be initialized
  to read/write any of the defined StkFormats.

  The class TcpWvIn sets up a socket server
  and waits for a connection.  Therefore,
  this program needs to be started before
  the streaming client.  This program will
  terminate when the socket connection is
  closed.
*/
/******************************************/

#include "TcpWvIn.h"
#include "RtWvOut.h"

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: tcpIn N fs \n");
  printf("    where N = number of channels,\n");
  printf("    and fs = the data sample rate.\n\n");
  exit(0);
}

int main(int argc, char *argv[])
{
  // Minimal command-line checking.
  if (argc != 3) usage();

  Stk::setSampleRate( atof(argv[2]) );
  int channels = (int) atoi(argv[1]);

  // Initialize the object pointers.
  RtWvOut *output = 0;
  TcpWvIn *input = 0;

  // Instantiate the TcpWvIn object.
  try {
    input = new TcpWvIn();
    input->listen( channels, Stk::STK_SINT16 );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Open the realtime output device.
  try {
    output = new RtWvOut(channels);
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Here's the runtime loop.
  while ( input->isConnected() )
    output->tickFrame( input->tickFrame() );

 cleanup:
  delete input;
  delete output;
  return 0;
}
