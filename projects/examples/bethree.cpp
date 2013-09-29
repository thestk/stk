// bethree.cpp STK tutorial program

#include "BeeThree.h"
#include "RtAudio.h"

// The TickData structure holds all the class instances and data that
// are shared by the various processing functions.
struct TickData {
  Instrmnt *instrument;
  StkFloat frequency;
  StkFloat scaler;
  long counter;
  bool done;

  // Default constructor.
  TickData()
    : instrument(0), scaler(1.0), counter(0), done( false ) {}
};

// This tick() function handles sample computation only.  It will be
// called automatically when the system needs a new buffer of audio
// samples.
int tick(char *buffer, int bufferSize, void *dataPointer)
{
  TickData *data = (TickData *) dataPointer;
  register StkFloat *samples = (StkFloat *) buffer;

  for ( int i=0; i<bufferSize; i++ ) {
    *samples++ = data->instrument->tick();
    if ( ++data->counter % 2000 == 0 ) {
      data->scaler += 0.025;
      data->instrument->setFrequency( data->frequency * data->scaler );
    }
  }

  if ( data->counter > 80000 )
    data->done = true;

  return 0;
}

int main()
{
  // Set the global sample rate and rawwave path before creating class instances.
  Stk::setSampleRate( 44100.0 );
  Stk::setRawwavePath( "../../rawwaves/" );

  TickData data;
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
    // Define and load the BeeThree instrument
    data.instrument = new BeeThree();
  }
  catch (StkError &) {
    goto cleanup;
  }

  data.frequency = 220.0;
  data.instrument->noteOn( data.frequency, 0.5 );

  try {
    dac->setStreamCallback(&tick, (void *)&data);
    dac->startStream();
  }
  catch (RtError &error) {
    error.printMessage();
    goto cleanup;
  }

  // Block waiting until callback signals done.
  while ( !data.done )
    Stk::sleep( 100 );
  
  // Shut down the callback and output stream.
  try {
    dac->cancelStreamCallback();
    dac->closeStream();
  }
  catch (RtError &error) {
    error.printMessage();
  }

 cleanup:
  delete data.instrument;
  delete dac;

  return 0;
}
