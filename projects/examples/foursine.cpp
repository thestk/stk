// foursine.cpp STK tutorial program

#include "WaveLoop.h"
#include "WvOut.h"

int main()
{
  // Set the global sample rate before creating class instances.
  Stk::setSampleRate( 44100.0 );

  int i, j;
  WvOut *output = 0;
  WaveLoop *inputs[4];
  for ( i=0; i<4; i++ ) inputs[i] = 0;

  // Define and load the sine waves
  try {
    for ( i=0; i<4; i++ ) {
      inputs[i] = new WaveLoop( "rawwaves/sinewave.raw", true );
      inputs[i]->setFrequency( 220.0 * (i+1) );
    }
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Define and open a 16-bit, four-channel AIFF formatted output file
  try {
    output = new WvOut( "foursine.aif", 4, WvOut::WVOUT_AIF, Stk::STK_SINT16 );
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Write two seconds of four sines to the output file
  StkFloat frame[4];
  for ( j=0; j<88200; j++ ) {
    for ( i=0; i<4; i++ )
      frame[i] = inputs[i]->tick();

    output->tickFrame( frame );
  }

  // Now write the first sine to all four channels for two seconds
  for ( j=0; j<88200; j++ ) {
    output->tick( inputs[0]->tick() );
  }

 cleanup:
  for ( i=0; i<4; i++ ) delete inputs[i];
  delete output;

  return 0;
}
