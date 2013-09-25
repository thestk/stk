/***************************************************/
/*! \class OnePole
    \brief STK one-pole filter class.

    This protected Filter subclass implements
    a one-pole digital filter.  A method is
    provided for setting the pole position along
    the real axis of the z-plane while maintaining
    a constant peak filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__ONEPOLE_H)
#define __ONEPOLE_H

#include "Filter.h"

class OnePole : protected Filter
{
public:

  //! Default constructor creates a first-order low-pass filter.
  OnePole();

  //! Overloaded constructor which sets the pole position during instantiation.
  OnePole(MY_FLOAT thePole);

  //! Class destructor.
  ~OnePole();

  //! Clears the internal state of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the pole position in the z-plane.
  /*!
    This method sets the pole position along the real-axis of the
    z-plane and normalizes the coefficients for a maximum gain of one.
    A positive pole value produces a low-pass filter, while a negative
    pole value produces a high-pass filter.  This method does not
    affect the filter \e gain value.
  */
  void setPole(MY_FLOAT thePole);

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
