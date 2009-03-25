/***************************************************/
/*! \class Effect
    \brief STK abstract effects parent class.

    This class provides common functionality for
    STK effects subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Stk.h"

#ifndef STK_EFFECT_H
#define STK_EFFECT_H

class Effect : public Stk
{
 public:
  //! Class constructor.
  Effect();

  //! Class destructor.
  virtual ~Effect();

  //! Reset and clear all internal state.
  virtual void clear() = 0;

  //! Set the mixture of input and "effected" levels in the output (0.0 = input only, 1.0 = reverb only). 
  void setEffectMix(StkFloat mix);

  //! Return the last output value.
  StkFloat lastOut() const;

  //! Return the last left output value.
  StkFloat lastOutLeft() const;

  //! Return the last right output value.
  StkFloat lastOutRight() const;

  //! Abstract tick function ... must be implemented in subclasses.
  virtual StkFloat tick( StkFloat input ) = 0;

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  virtual StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the effect and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:

  // Returns true if argument value is prime.
  bool isPrime( int number );

  StkFloat lastOutput_[2];
  StkFloat effectMix_;

};

#endif

