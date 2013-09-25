/******************************************/
/*
  A very basic example program which
  demonstrates how to play STK's Moog
  instrument.

  By Gary P. Scavone, 2001.
  Thanks to Dirk Heise for the suggestion.

  This particular program uses the Moog
  class, though any Instrmnt subclass will
  work.  If you use a different instrument,
  however, you may need to update the O_FILES
  dependencies in the Makefile to successfully
  compile it.
*/
/******************************************/

#include "WvOut.h"
#include "RtWvOut.h"
#include "Instrmnt.h"
#include "Moog.h"

int main(void)
{
  Instrmnt* instrument = new Moog();
  RtWvOut* output = new RtWvOut(1);

  // Start the instrument with note number 60 and
  // a normalized velocity of 0.9.
  instrument->noteOn(70, 0.9);

  // Run the instrument for 80000/SRATE seconds.
  MY_FLOAT *vector = new MY_FLOAT[16];
  for (int i=0; i<80000; i++) {
    output->tick(instrument->tick());
  }

  delete vector;
  delete output; 
  delete instrument;
  return 0;
}
