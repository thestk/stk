/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_CHORUS_H
#define STK_CHORUS_H

#include "Effect.h" 
#include "DelayL.h" 
#include "WaveLoop.h" 

class Chorus : public Effect
{
 public:
  //! Class constructor, taking the median desired delay length.
  /*!
    An StkError can be thrown if the rawwave path is incorrect.
  */
  Chorus( StkFloat baseDelay = 6000 );

  //! Class destructor.
  ~Chorus();

  //! Reset and clear all internal state.
  void clear();

  //! Set modulation depth.
  void setModDepth(StkFloat depth);

  //! Set modulation frequency.
  void setModFrequency(StkFloat frequency);

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
  DelayL delayLine_[2];
  WaveLoop *mods_[2];
  StkFloat baseLength_;
  StkFloat modDepth_;

};

#endif

