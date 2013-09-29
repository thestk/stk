/***************************************************/
/*! \class Effect
    \brief STK abstract effects parent class.

    This class provides common functionality for
    STK effects subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Effect.h"
#include <math.h>

Effect :: Effect()
{
}

Effect :: ~Effect()
{
}

void Effect :: setEffectMix(StkFloat mix)
{
  if ( mix < 0.0 ) {
    errorString_ << "Effect::setEffectMix: mix parameter is less than zero ... setting to zero!";
    handleError( StkError::WARNING );
    effectMix_ = 0.0;
  }
  else if ( mix > 1.0 ) {
    errorString_ << "Effect::setEffectMix: mix parameter is greater than 1.0 ... setting to one!";
    handleError( StkError::WARNING );
    effectMix_ = 1.0;
  }
  else
    effectMix_ = mix;
}

StkFloat Effect :: lastOut() const
{
  return (lastOutput_[0] + lastOutput_[1]) * 0.5;
}

StkFloat Effect :: lastOutLeft() const
{
  return lastOutput_[0];
}

StkFloat Effect :: lastOutRight() const
{
  return lastOutput_[1];
}

StkFloat *Effect :: tick(StkFloat *vector, unsigned int vectorSize)
{
  for (unsigned int i=0; i<vectorSize; i++)
    vector[i] = tick(vector[i]);

  return vector;
}

StkFrames& Effect :: tick( StkFrames& frames, unsigned int channel )
{
  if ( channel == 0 || frames.channels() < channel ) {
    errorString_ << "Effect::tick(): channel argument (" << channel << ") is zero or > channels in StkFrames argument!";
    handleError( StkError::FUNCTION_ARGUMENT );
  }

  if ( frames.channels() == 1 ) {
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[i] = tick( frames[i] );
  }
  else if ( frames.interleaved() ) {
    unsigned int hop = frames.channels();
    unsigned int index = channel - 1;
    for ( unsigned int i=0; i<frames.frames(); i++ ) {
      frames[index] = tick( frames[index] );
      index += hop;
    }
  }
  else {
    unsigned int iStart = (channel - 1) * frames.frames();
    for ( unsigned int i=0; i<frames.frames(); i++ )
      frames[iStart + i] = tick( frames[iStart + i] );
  }

  return frames;
}

bool Effect :: isPrime(int number)
{
  if (number == 2) return true;
  if (number & 1)	{
	  for (int i=3; i<(int)sqrt((double)number)+1; i+=2)
		  if ( (number % i) == 0) return false;
	  return true; // prime
	}
  else return false; // even
}
