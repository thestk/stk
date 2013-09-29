/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Instrmnt.h"

Instrmnt :: Instrmnt()
{
}

Instrmnt :: ~Instrmnt()
{
}

void Instrmnt :: setFrequency(StkFloat frequency)
{
  errorString_ << "Instrmnt::setFrequency: virtual setFrequency function call!";
  handleError( StkError::WARNING );
}

StkFloat Instrmnt :: lastOut() const
{
  return lastOutput_;
}

// Support for stereo output:
StkFloat Instrmnt :: lastOutLeft(void) const
{
  return 0.5 * lastOutput_;
}
                                                                                
StkFloat Instrmnt :: lastOutRight(void) const
{
  return 0.5 * lastOutput_;
}

StkFloat *Instrmnt :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick();

  return vector;
}

StkFrames& Instrmnt :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "Instrmnt::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
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

void Instrmnt :: controlChange(int number, StkFloat value)
{
  errorString_ << "Instrmnt::controlChange: virtual function call!";
  handleError( StkError::WARNING );
}
