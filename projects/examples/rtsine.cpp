// rtsine.cpp STK tutorial program

#include "WaveLoop.h"
#include "RtWvOut.h"

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  WaveLoop *input = 0;
  RtWvOut *output = 0;

  try {
    // Define and load the sine wave file
    input = new WaveLoop( "rawwaves/sinewave.raw", TRUE );

    // Define and open the default realtime output device for one-channel playback
    output = new RtWvOut(1);
  }
  catch (StkError &) {
    goto cleanup;
  }

  input->setFrequency(440.0);

  // Play the oscillator for 40000 samples
  int i;
  for ( i=0; i<40000; i++ ) {
    try {
      output->tick(input->tick());
    }
    catch (StkError &) {
      goto cleanup;
    }
  }

 cleanup:
  delete input;
  delete output;

  return 0;
}
