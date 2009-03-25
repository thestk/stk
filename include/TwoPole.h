/***************************************************/
/*! \class TwoPole
    \brief STK two-pole filter class.

    This protected Filter subclass implements
    a two-pole digital filter.  A method is
    provided for creating a resonance in the
    frequency response while maintaining a nearly
    constant filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_TWOPOLE_H
#define STK_TWOPOLE_H

#include "Filter.h"

class TwoPole : protected Filter
{
 public:

  //! Default constructor creates a second-order pass-through filter.
  TwoPole();

  //! Class destructor.
  ~TwoPole();

  //! Clears the internal states of the filter.
  void clear(void);

  //! Set the b[0] coefficient value.
  void setB0(StkFloat b0);

  //! Set the a[1] coefficient value.
  void setA1(StkFloat a1);

  //! Set the a[2] coefficient value.
  void setA2(StkFloat a2);

  //! Sets the filter coefficients for a resonance at \e frequency (in Hz).
  /*!
    This method determines the filter coefficients corresponding to
    two complex-conjugate poles with the given \e frequency (in Hz)
    and \e radius from the z-plane origin.  If \e normalize is true,
    the coefficients are then normalized to produce unity gain at \e
    frequency (the actual maximum filter gain tends to be slightly
    greater than unity when \e radius is not close to one).  The
    resulting filter frequency response has a resonance at the given
    \e frequency.  The closer the poles are to the unit-circle (\e
    radius close to one), the narrower the resulting resonance width.
    An unstable filter will result for \e radius >= 1.0.  For a better
    resonance filter, use a BiQuad filter. \sa BiQuad filter class
  */
  void setResonance(StkFloat frequency, StkFloat radius, bool normalize = false);

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

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Take a channel of the StkFrames object as inputs to the filter and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );
};

#endif
