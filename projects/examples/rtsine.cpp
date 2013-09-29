// rtsine.cpp STK tutorial program

#include "WaveLoop.h"
#include "RtWvOut.h"

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  WaveLoop *sine = 0;
  RtWvOut *dac = 0;

  try {
    // Define and load the sine wave file
    sine = new WaveLoop( "rawwaves/sinewave.raw", true );

    // Define and open the default realtime output device for one-channel playback
    dac = new RtWvOut(1);
  }
  catch (StkError &) {
    goto cleanup;
  }

  sine->setFrequency(440.0);

  // Play the oscillator for 40000 samples
  int i;
  for ( i=0; i<40000; i++ ) {
    try {
      dac->tick( sine->tick() );
    }
    catch (StkError &) {
      goto cleanup;
    }
  }

 cleanup:
  delete sine;
  delete dac;

  return 0;
}
