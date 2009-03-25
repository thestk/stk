/***************************************************/
/*! \class Generator
    \brief STK abstract unit generator parent class.

    This class provides common functionality for
    STK unit generator source subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Generator.h"

Generator :: Generator() : Stk()
{
  lastOutput_ = 0.0;
}

Generator :: ~Generator()
{
}

StkFloat *Generator :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

StkFrames& Generator :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "Generator::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick();
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick();
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick();
  }

  return frames;
}
