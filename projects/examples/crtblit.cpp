// crtblit.cpp STK tutorial program

#include "Blit.h"
#include "BlitSaw.h"
#include "RtAudio.h"

unsigned long counter = 0;
StkFloat frequency = 200.0;

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(char *buffer, int bufferSize, void *dataPointer)
{
  BlitSaw *blit = (BlitSaw *) dataPointer;
  register StkFloat *samples = (StkFloat *) buffer;

  for ( int i=0; i<bufferSize; i++ ) {
    *samples++ = blit->tick();

    if ( counter++ % 500 == 0 && frequency < 3000.0 ) {
      frequency *= 1.01;
      blit->setFrequency( frequency );
    }
  }

  return 0;
}

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::showWarnings( true );

  BlitSaw blit;
  RtAudio *dac = 0;

  // Figure out how many bytes in an StkFloat and setup the RtAudio object.
  RtAudioFormat format = ( sizeof(StkFloat) == 8 ) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32;
  int bufferSize = RT_BUFFER_SIZE;
  try {
    dac = new RtAudio(0, 1, 0, 0, format, (int)Stk::sampleRate(), &bufferSize, 4);
  }
  catch (RtError& error) {
    error.printMessage();
    exit(0);
  }

  blit.setFrequency( frequency );
  blit.setHarmonics( 20 );

  try {
    dac->setStreamCallback(&tick, (void *)&blit);
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
