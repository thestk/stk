/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_PITSHIFT_H
#define STK_PITSHIFT_H

#include "Effect.h" 
#include "DelayL.h" 

class PitShift : public Effect
{
 public:
  //! Class constructor.
  PitShift();

  //! Class destructor.
  ~PitShift();

  //! Reset and clear all internal state.
  void clear();

  //! Set the pitch shift factor (1.0 produces no shift).
  void setShift(StkFloat shift);

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
  StkFloat delay_[2];
  StkFloat env_[2];
  StkFloat rate_;
  unsigned long delayLength;
  unsigned long halfLength;

};

#endif

