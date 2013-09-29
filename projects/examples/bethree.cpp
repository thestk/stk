// bethree.cpp STK tutorial program

#include "BeeThree.h"
#include "RtWvOut.h"

int main()
{
  // Set the global sample rate and rawwave path before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::setRawwavePath( "../../rawwaves/" );

  Instrmnt *instrument = 0;
  RtWvOut *output = 0;
  MY_FLOAT frequency, amplitude, scaler;
  long counter, i;

  try {
    // Define and load the BeeThree instrument
    instrument = new BeeThree();

    // Define and open the default realtime output device for one-channel playback
    output = new RtWvOut(1);
  }
  catch (StkError &) {
    goto cleanup;
  }

  scaler = 1.0;
  frequency = 220.0;
  amplitude = 0.5;
  instrument->noteOn( frequency, amplitude );

  // Play the instrument for 80000 samples, changing the frequency every 2000 samples
  counter = 0;
  while ( counter < 80000 ) {
    for ( i=0; i<2000; i++ ) {
      try {
        output->tick( instrument->tick() );
      }
      catch (StkError &) {
        goto cleanup;
      }
    }

    counter += 2000;
    scaler += 0.025;
    instrument->setFrequency( frequency * scaler );
  }

  // Turn the instrument off with maximum decay envelope.
  instrument->noteOff( 1.0 );

 cleanup:
  delete instrument;
  delete output;

  return 0;
}
