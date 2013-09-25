/***************************************************/
/*! \class Echo
    \brief STK echo effect class.

    This class implements a echo effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ECHO_H)
#define __ECHO_H

#include "Stk.h" 
#include "Delay.h" 

class Echo : public Stk
{
 public:
  //! Class constructor, taking the longest desired delay length.
  Echo(MY_FLOAT longestDelay);

  //! Class destructor.
  ~Echo();

  //! Reset and clear all internal state.
  void clear();

  //! Set the delay line length in samples.
  void setDelay(MY_FLOAT delay);

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:  
  Delay *delayLine;
  long length;
  MY_FLOAT lastOutput;
  MY_FLOAT effectMix;

};

#endif

