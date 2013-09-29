// sineosc.cpp STK tutorial program

#include "WaveLoop.h"
#include "WvOut.h"

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  WaveLoop *input = 0;
  WvOut *output = 0;

  try {
    // Define and load the sine wave file
    input = new WaveLoop( "rawwaves/sinewave.raw", true );

    // Define and open a 16-bit, one-channel WAV formatted output file
    output = new WvOut( "hellosine.wav", 1, WvOut::WVOUT_WAV, Stk::STK_SINT16 );
  }
  catch ( StkError & ) {
    goto cleanup;
  }

  input->setFrequency( 440.0 );

  // Run the oscillator for 40000 samples, writing to the output file
  int i;
  for ( i=0; i<40000; i++ ) {

    try {
      output->tick( input->tick() );
    }
    catch ( StkError & ) {
      goto cleanup;
    }

  }

 cleanup:
  delete input;
  delete output;

  return 0;
}
