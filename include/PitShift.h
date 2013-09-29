/***************************************************/
/*! \class PitShift
    \brief STK simple pitch shifter effect class.

    This class implements a simple pitch shifter
    using delay lines.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__PITSHIFT_H)
#define __PITSHIFT_H

#include "Stk.h" 
#include "DelayL.h" 

class PitShift : public Stk
{
 public:
  //! Class constructor.
  PitShift();

  //! Class destructor.
  ~PitShift();

  //! Reset and clear all internal state.
  void clear();

  //! Set the pitch shift factor (1.0 produces no shift).
  void setShift(MY_FLOAT shift);

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:  
  DelayL *delayLine[2];
  MY_FLOAT lastOutput;
  MY_FLOAT delay[2];
  MY_FLOAT env[2];
  MY_FLOAT effectMix;
  MY_FLOAT rate;

};

#endif

