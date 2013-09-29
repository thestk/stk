/***************************************************/
/*! \class Filter
    \brief STK filter class.

    This class implements a generic structure which
    can be used to create a wide range of filters.
    It can function independently or be subclassed
    to provide more specific controls based on a
    particular filter type.

    In particular, this class implements the standard
    difference equation:

    a[0]*y[n] = b[0]*x[n] + ... + b[nb]*x[n-nb] -
                a[1]*y[n-1] - ... - a[na]*y[n-na]

    If a[0] is not equal to 1, the filter coeffcients
    are normalized by a[0].

    The \e gain parameter is applied at the filter
    input and does not affect the coefficient values.
    The default gain value is 1.0.  This structure
    results in one extra multiply per computed sample,
    but allows easy control of the overall filter gain.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__FILTER_H)
#define __FILTER_H

#include "Stk.h"

class Filter : public Stk
{
public:
  //! Default constructor creates a zero-order pass-through "filter".
  Filter(void);

  //! Overloaded constructor which takes filter coefficients.
  /*!
    An StkError can be thrown if either \e nb or \e na is less than
    one, or if the a[0] coefficient is equal to zero.
  */
  Filter(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients);

  //! Class destructor.
  virtual ~Filter(void);

  //! Clears all internal states of the filter.
  void clear(void);

  //! Set filter coefficients.
  /*!
    An StkError can be thrown if either \e nb or \e na is less than
    one, or if the a[0] coefficient is equal to zero.  If a[0] is not
    equal to 1, the filter coeffcients are normalized by a[0].
  */
  void setCoefficients(int nb, MY_FLOAT *bCoefficients, int na, MY_FLOAT *aCoefficients);

  //! Set numerator coefficients.
  /*!
    An StkError can be thrown if \e nb is less than one.  Any
    previously set denominator coefficients are left unaffected.
    Note that the default constructor sets the single denominator
    coefficient a[0] to 1.0.
  */
  void setNumerator(int nb, MY_FLOAT *bCoefficients);

  //! Set denominator coefficients.
  /*!
    An StkError can be thrown if \e na is less than one or if the
    a[0] coefficient is equal to zero.  Previously set numerator
    coefficients are unaffected unless a[0] is not equal to 1, in
    which case all coeffcients are normalized by a[0].  Note that the
    default constructor sets the single numerator coefficient b[0]
    to 1.0.
  */
  void setDenominator(int na, MY_FLOAT *aCoefficients);

  //! Set the filter gain.
  /*!
    The gain is applied at the filter input and does not affect the
    coefficient values.  The default gain value is 1.0.
   */
  virtual void setGain(MY_FLOAT theGain);

  //! Return the current filter gain.
  virtual MY_FLOAT getGain(void) const;

  //! Return the last computed output value.
  virtual MY_FLOAT lastOut(void) const;

  //! Input one sample to the filter and return one output.
  virtual MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the filter and return an equal number of outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

protected:
  MY_FLOAT gain;
  int     nB;
  int     nA;
  MY_FLOAT *b;
  MY_FLOAT *a;
  MY_FLOAT *outputs;
  MY_FLOAT *inputs;

};

#endif
