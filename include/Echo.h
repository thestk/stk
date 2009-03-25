/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements an echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2007.
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

 protected:

  StkFloat computeSample( StkFloat input );

  Delay delayLine_;
  unsigned long length_;

};

#endif

