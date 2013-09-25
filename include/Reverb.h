/***************************************************/
/*! \class Reverb
    \brief STK abstract reverberator parent class.

    This class provides common functionality for
    STK reverberator subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#include "Stk.h"

#if !defined(__REVERB_H)
#define __REVERB_H

class Reverb : public Stk
{
 public:
  //! Class constructor.
  Reverb();

  //! Class destructor.
  virtual ~Reverb();

  //! Reset and clear all internal state.
  virtual void clear() = 0;

  //! Set the mixture of input and "reverberated" levels in the output (0.0 = input only, 1.0 = reverb only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Abstract tick function ... must be implemented in subclasses.
  virtual MY_FLOAT tick(MY_FLOAT input) = 0;

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:

  // Returns true if argument value is prime.
  bool isPrime(int number);

  MY_FLOAT lastOutput[2];
  MY_FLOAT effectMix;

};

#endif // defined(__REVERB_H)

