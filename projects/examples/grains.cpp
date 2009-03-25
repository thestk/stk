// grains.cpp
//
// A simple test program for the STK Granulate class.

#include "Granulate.h"
#include "RtAudio.h"

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(char *buffer, int bufferSize, void *dataPointer)
{
  Granulate *grani = (Granulate *) dataPointer;
  register StkFloat *samples = (StkFloat *) buffer;

  for ( int i=0; i<bufferSize; i++ )
    *samples++ = grani->tick();

  return 0;
}

void usage(void) {
  // Error function in case of incorrect command-line
  // argument specifications.
  printf("\nuseage: grains file N dur ramp offset delay stretch ramdomness\n");
  printf("    where file = a soundfile to granulate,\n");
  printf("    N = the number of grain voices to use,\n");
  printf("    dur = the grain duration (ms),\n");
  printf("    ramp = the envelope percent (0-100),\n");
  printf("    offset = hop time between grains (ms),\n");
  printf("    delay = pause time between grains (ms),\n");
  printf("    stretch = stetch factor (1-1000),\n");
  printf("    and randomness = factor between 0 - 1.0 to control grain parameter randomness.\n\n");
  exit(0);
}

int main( int argc, char *argv[] )
{
  // Minimal command-line checking.
  if (argc != 9) usage();
  unsigned int N = (unsigned int) atoi(argv[2]);
  unsigned int duration = (unsigned int) atoi(argv[3]);
  unsigned int ramp = (unsigned int) atoi(argv[4]);
  unsigned int offset = (unsigned int) atoi(argv[5]);
  unsigned int delay = (unsigned int) atoi(argv[6]);
  unsigned int stretch = (unsigned int) atoi(argv[7]);
  StkFloat random = (StkFloat) atof(argv[8]);

  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  RtAudio *dac = 0;
  Granulate grani;
  grani.setRandomFactor( random );
  grani.setStretch( stretch );
  grani.setGrainParameters( duration, ramp, offset, delay );

  try {
    grani.openFile( argv[1] );
  }
  catch ( StkError& ) {
    exit(0);
  }
  grani.setVoices( N );

  // Figure out how many bytes in an StkFloat and setup the RtAudio object.
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  int bufferSize = RT_BUFFER_SIZE;
  try {
    dac = new RtAudio(0, 1, 0, 0, format, (int)Stk::sampleRate(), &bufferSize, 4);
  }
  catch (RtError& error) {
    error.printMessage();
    goto cleanup;
  }

  try {
    dac->setStreamCallback(&tick, (void *)&grani);
    dac->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }


  // Block waiting here.
  char keyhit;
  std::cout << "\nPlaying ... press <enter> to quit.\n";
  std::cin.get(keyhit);

  // Shut down the callback and output stream.
  try {
    dac->cancelStreamCallback();
    dac->closeStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  delete dac;

  return 0;
}
