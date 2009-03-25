/******************************************/
/*
  Example program to play an N channel
  soundfile.

  This program will load WAV, SND, AIF, and
  MAT-file formatted files of various data
  types.  If the audio system does not support
  the number of channels or sample rate of
  the soundfile, the program will stop.

  By Gary P. Scavone, 2000 - 2004.
*/
/******************************************/

#include "WvIn.h"
#include "RtAudio.h"

#include <signal.h>
#include <iostream>

// Eewww ... global variables! :-)
bool done;
StkFrames frames;
static void finish(int ignore){ done = true; }

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nuseage: play file sr <rate>\n";
  std::cout << "    where file = the file to play,\n";
  std::cout << "    where sr = sample rate,\n";
  std::cout << "    and rate = an optional playback rate.\n";
  std::cout << "               (default = 1.0, can be negative)\n\n";
  exit(0);
}

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(char *buffer, int bufferSize, void *dataPointer)
{
  WvIn *input = (WvIn *) dataPointer;
  register StkFloat *samples = (StkFloat *) buffer;

  input->tickFrame( frames );
  for ( unsigned int i=0; i<frames.size(); i++ )
    *samples++ = frames[i];

  if ( input->isFinished() ) {
    done = true;
    return 1;
  }
  else
    return 0;
}

int main(int argc, char *argv[])
{
  // Minimal command-line checking.
  if (argc < 3 || argc > 4) usage();

  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( (StkFloat) atof(argv[2]) );

  // Initialize our WvIn and RtAudio pointers.
  RtAudio *dac = 0;
  WvIn *input = 0;

  // Try to load the soundfile.
  try {
    input = new WvIn( argv[1] );
  }
  catch (StkError &) {
    exit(0);
  }

  // Set input read rate based on the default STK sample rate.
  double rate = 1.0;
  rate = input->getFileRate() / Stk::sampleRate();
  if ( argc == 4 ) rate *= atof(argv[3]);
  input->setRate( rate );

  // Find out how many channels we have.
  int channels = input->getChannels();

  // Define and open the realtime output device.
  // Figure out how many bytes in an StkFloat and setup the RtAudio object.
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  int bufferSize = RT_BUFFER_SIZE;
  try {
    dac = new RtAudio(0, channels, 0, 0, format, (int)Stk::sampleRate(), &bufferSize, 4);
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  // Install an interrupt handler function.
	(void) signal(SIGINT, finish);

  // Resize the StkFrames object appropriately.
  frames.resize( bufferSize, channels );

  try {
    dac->setStreamCallback(&tick, (void *)input);
    dac->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  // Block waiting until callback signals done.
  while ( !done )
    Stk::sleep( 100 );
  
  // By returning a non-zero value in the callback above, the stream
  // is automatically stopped.  But we should still close it.
  try {
    dac->cancelStreamCallback();
    dac->closeStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  delete input;
  delete dac;
  return 0;
}
