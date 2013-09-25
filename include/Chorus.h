/***************************************************/
/*! \class Chorus
    \brief STK chorus effect class.

    This class implements a chorus effect.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__CHORUS_H)
#define __CHORUS_H

#include "Stk.h" 
#include "DelayL.h" 
#include "WaveLoop.h" 

class Chorus : public Stk
{
 public:
  //! Class constructor, taking the longest desired delay length.
  Chorus(MY_FLOAT baseDelay);

  //! Class destructor.
  ~Chorus();

  //! Reset and clear all internal state.
  void clear();

  //! Set modulation depth.
  void setModDepth(MY_FLOAT depth);

  //! Set modulation frequency.
  void setModFrequency(MY_FLOAT frequency);

  //! Set the mixture of input and processed levels in the output (0.0 = input only, 1.0 = processed only). 
  void setEffectMix(MY_FLOAT mix);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Compute one output sample.
  MY_FLOAT tick(MY_FLOAT input);

  //! Take \e vectorSize inputs, compute the same number of outputs and return them in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

 protected:  
  DelayL *delayLine[2];
  WaveLoop *mods[2];
  MY_FLOAT baseLength;
  MY_FLOAT modDepth;
  MY_FLOAT lastOutput[2];
  MY_FLOAT effectMix;

};

#endif

