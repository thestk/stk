/***************************************************/
/*! \class OneZero
    \brief STK one-zero filter class.

    This protected Filter subclass implements
    a one-zero digital filter.  A method is
    provided for setting the zero position
    along the real axis of the z-plane while
    maintaining a constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ONEZERO_H)
#define __ONEZERO_H

#include "Filter.h"

class OneZero : protected Filter
{
 public:

  //! Default constructor creates a first-order low-pass filter.
  OneZero();

  //! Overloaded constructor which sets the zero position during instantiation.
  OneZero(MY_FLOAT theZero);

  //! Class destructor.
  ~OneZero();

  //! Clears the internal state of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the zero position in the z-plane.
  /*!
    This method sets the zero position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive zero value produces a high-pass filter, while a
    negative zero value produces a low-pass filter.  This method does
    not affect the filter \e gain value.
  */
  void setZero(MY_FLOAT theZero);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);
};

#endif
