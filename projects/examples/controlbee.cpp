// controlbee.cpp STK tutorial program

#include "BeeThree.h"
#include "RtWvOut.h"
#include "Messager.h"
#include "SKINI.msg"
#include <math.h>

int main()
{
  // Set the global sample rate and rawwave path before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::setRawwavePath( "../../rawwaves/" );

  Instrmnt *instrument = 0;
  RtWvOut *output = 0;
  Messager *messager = 0;
  bool done = FALSE;

  try {
    // Define and load the BeeThree instrument
    instrument = new BeeThree();

    // Define and open the default realtime output device for one-channel playback
    output = new RtWvOut(1);
  }
  catch (StkError &) {
    goto cleanup;
  }

  try {
    // Create a Messager instance to read from a redirected SKINI scorefile.
    messager = new Messager();
  }
  catch (StkError &) {
    goto cleanup;
  }

  // Play the instrument until the end of the scorefile.
  int i, nTicks, type;
  MY_FLOAT byte2, byte3, frequency;
  while (!done) {

    // Look for new messages and return a delta time (in samples).
    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();
    try {
      for ( i=0; i<nTicks; i++ )
        output->tick( instrument->tick() );
    }
    catch (StkError &) {
      goto cleanup;
    }

    if ( type > 0 ) {
      // Process the new control message.
      byte2 = messager->getByteTwo();
      byte3 = messager->getByteThree();

      switch(type) {

      case __SK_NoteOn_:
        frequency = (MY_FLOAT) 220.0 * pow( 2.0, (byte2 - 57.0) / 12.0 );
        instrument->noteOn( frequency, byte3 * ONE_OVER_128 );
        break;

      case __SK_NoteOff_:
        instrument->noteOff( byte3 * ONE_OVER_128 );
        break;

      case __SK_ControlChange_:
        instrument->controlChange( (int) byte2, byte3 );
        break;

      case __SK_AfterTouch_:
        instrument->controlChange( 128, byte2 );
        break;
      }
    }
  }

 cleanup:
  delete instrument;
  delete output;
  delete messager;

  return 0;
}
