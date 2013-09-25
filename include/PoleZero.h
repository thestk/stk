/***************************************************/
/*! \class PoleZero
    \brief STK one-pole, one-zero filter class.

    This protected Filter subclass implements
    a one-pole, one-zero digital filter.  A
    method is provided for creating an allpass
    filter with a given coefficient.  Another
    method is provided to create a DC blocking filter.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__POLEZERO_H)
#define __POLEZERO_H

#include "Filter.h"

class PoleZero : protected Filter
{
 public:

  //! Default constructor creates a first-order pass-through filter.
  PoleZero();

  //! Class destructor.
  ~PoleZero();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(MY_FLOAT b0);

  //! Set the b[1] coefficient value.
  void setB1(MY_FLOAT b1);

  //! Set the a[1] coefficient value.
  void setA1(MY_FLOAT a1);

  //! Set the filter for allpass behavior using \e coefficient.
  /*!
    This method uses \e coefficient to create an allpass filter,
    which has unity gain at all frequencies.  Note that the \e
    coefficient magnitude must be less than one to maintain stability.
  */
  void setAllpass(MY_FLOAT coefficient);

  //! Create a DC blocking filter with the given pole position in the z-plane.
  /*!
    This method sets the given pole position, together with a zero
    at z=1, to create a DC blocking filter.  \e thePole should be
    close to one to minimize low-frequency attenuation.

  */
  void setBlockZero(MY_FLOAT thePole = 0.99);

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
