// crtsine.cpp STK tutorial program

#include "WaveLoop.h"
#include "RtAudio.h"

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(char *buffer, int bufferSize, void *dataPointer)
{
  WaveLoop *sine = (WaveLoop *) dataPointer;
  register StkFloat *samples = (StkFloat *) buffer;

  for ( int i=0; i<bufferSize; i++ )
    *samples++ = sine->tick();

  return 0;
}

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  WaveLoop *sine = 0;
  RtAudio *dac = 0;

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
    // Define and load the sine wave file
    sine = new WaveLoop( "rawwaves/sinewave.raw", true );
  }
  catch (StkError &) {
    goto cleanup;
  }

  sine->setFrequency(440.0);

  try {
    dac->setStreamCallback(&tick, (void *)sine);
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

  delete sine;
  delete dac;

  return 0;
}
