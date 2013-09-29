/***************************************************/
/*! \class Instrmnt
    \brief STK instrument abstract base class.

    This class provides a common interface for
    all STK instruments.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__INSTRMNT_H)
#define __INSTRMNT_H

#include "Stk.h"
#include <iostream>

class Instrmnt : public Stk
{
 public:
  //! Default constructor.
  Instrmnt();

  //! Class destructor.
  virtual ~Instrmnt();

  //! Start a note with the given frequency and amplitude.
  virtual void noteOn(MY_FLOAT frequency, MY_FLOAT amplitude) = 0;

  //! Stop a note with the given amplitude (speed of decay).
  virtual void noteOff(MY_FLOAT amplitude) = 0;

  //! Set instrument parameters for a particular frequency.
  virtual void setFrequency(MY_FLOAT frequency);

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the last left output value.
  MY_FLOAT lastOutLeft() const;

  //! Return the last right output value.
  MY_FLOAT lastOutRight() const;

  //! Compute one output sample.
  virtual MY_FLOAT tick() = 0;

  //! Computer \e vectorSize outputs and return them in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
  
  //! Perform the control change specified by \e number and \e value (0.0 - 128.0).
  virtual void controlChange(int number, MY_FLOAT value);

  protected:
    MY_FLOAT lastOutput;

};

#endif
