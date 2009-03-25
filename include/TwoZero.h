/***************************************************/
/*! \class TwoZero
    \brief STK two-zero filter class.

    This protected Filter subclass implements
    a two-zero digital filter.  A method is
    provided for creating a "notch" in the
    frequency response while maintaining a
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/

#ifndef STK_TWOZERO_H
#define STK_TWOZERO_H

#include "Filter.h"

class TwoZero : protected Filter
{
 public:
  //! Default constructor creates a second-order pass-through filter.
  TwoZero();

  //! Class destructor.
  ~TwoZero();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(StkFloat b0);

  //! Set the b[1] coefficient value.
  void setB1(StkFloat b1);

  //! Set the b[2] coefficient value.
  void setB2(StkFloat b2);

  //! Sets the filter coefficients for a "notch" at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate zeros with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  The coefficients are then
    normalized to produce a maximum filter gain of one (independent of
    the filter \e gain parameter).  The resulting filter frequency
    response has a "notch" or anti-resonance at the given \e
    frequency.  The closer the zeros are to the unit-circle (\e radius
    close to or equal to one), the narrower the resulting notch width.
  */
  void setNotch(StkFloat frequency, StkFloat radius);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  void setGain(StkFloat gain);

  //! Return the current filter gain.
  StkFloat getGain(void) const;

  //! Return the last computed output value.
  StkFloat lastOut(void) const;

  //! Input one sample to the filter and return one output.
  StkFloat tick(StkFloat sample);

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The \c channel argument should be zero or greater (the first
    channel is specified by 0).  An StkError will be thrown if the \c
    channel argument is equal to or greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 0 );
};

#endif
