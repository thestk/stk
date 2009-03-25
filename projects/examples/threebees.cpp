// threebees.cpp STK tutorial program

#include "BeeThree.h"
#include "RtWvOut.h"
#include "Messager.h"
#include "Voicer.h"
#include "SKINI.msg"

int main()
{
  // Set the global sample rate and rawwave path before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::setRawwavePath( "../../rawwaves/" );

  int i;
  RtWvOut *output = 0;
  Messager *messager = 0;
  Voicer *voicer = 0;
  bool done = FALSE;
  Instrmnt *instrument[3];
  for ( i=0; i<3; i++ ) instrument[i] = 0;

  try {
    // Define and load the BeeThree instruments
    for ( i=0; i<3; i++ )
      instrument[i] = new BeeThree();

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

  // Instantiate the voicer for a maximum of three voices.
  voicer = new Voicer( 3 );
  for ( i=0; i<3; i++ )
    voicer->addInstrument( instrument[i] );

  // Play the instrument until the end of the scorefile.
  int nTicks, type;
  MY_FLOAT byte2, byte3;
  while (!done) {

    // Look for new messages and return a delta time (in samples).
    type = messager->nextMessage();
    if (type < 0)
      done = TRUE;

    nTicks = messager->getDelta();
    try {
      for ( i=0; i<nTicks; i++ )
        output->tick( voicer->tick() );
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
        voicer->noteOn( byte2, byte3 );
        break;

      case __SK_NoteOff_:
        voicer->noteOff( byte2, byte3 );
        break;

      case __SK_ControlChange_:
        voicer->controlChange( (int) byte2, byte3 );
        break;

      case __SK_AfterTouch_:
        voicer->controlChange( 128, byte2 );
        break;
      }
    }
  }

 cleanup:
  for ( i=0; i<3; i++ ) delete instrument[i];
  delete output;
  delete messager;
  delete voicer;

  return 0;
}
