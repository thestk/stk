// sineosc.cpp STK tutorial program

#include "WaveLoop.h"
#include "FileWvOut.h"

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  WaveLoop input;
  FileWvOut output;

  try {
    // Load the sine wave file.
    input.openFile( "rawwaves/sinewave.raw", true );

    // Open a 16-bit, one-channel WAV formatted output file
    output.openFile( "hellosine.wav", 1, FileWrite::FILE_WAV, Stk::STK_SINT16 );
  }
  catch ( StkError & ) {
    exit(0);
  }

  input.setFrequency( 440.0 );

  // Run the oscillator for 40000 samples, writing to the output file
  int i;
  for ( i=0; i<40000; i++ ) {

    try {
      output.tick( input.tick() );
    }
    catch ( StkError & ) {
      exit(0);
    }

  }

  return 0;
}
