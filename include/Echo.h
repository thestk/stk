/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_ECHO_H
#define STK_ECHO_H

#include "Effect.h" 
#include "Delay.h" 

class Echo : public Effect
{
 public:
  //! Class constructor, taking the longest desired delay length (one second default value).
  /*!
    The default delay value is set to 1/2 the maximum delay length.
  */
  Echo( unsigned long maximumDelay = (unsigned long) Stk::sampleRate() );

  //! Class destructor.
  ~Echo();

  //! Reset and clear all internal state.
  void clear();

  //! Set the maximum delay line length in samples.
  void setMaximumDelay( unsigned long delay );

  //! Set the delay line length in samples.
  void setDelay( unsigned long delay );

  //! Compute one output sample.
  StkFloat tick(StkFloat input);

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the effect and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:  
  Delay delayLine_;
  unsigned long length_;

};

#endif

