/***************************************************/
/*! \class DelayA
    \brief STK allpass interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using a first-order
    allpass filter.  A fixed maximum length
    of 4095 and a delay of 0.5 is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    An allpass filter has unity magnitude gain but
    variable phase delay properties, making it useful
    in achieving fractional delays without affecting
    a signal's frequency magnitude response.  In
    order to achieve a maximally flat phase delay
    response, the minimum delay possible in this
    implementation is limited to a value of 0.5.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DelayA_h)
#define __DelayA_h

#include "Delay.h"

class DelayA : public Delay
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  DelayA();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  
  DelayA(MY_FLOAT theDelay, long maxDelay);

  //! Class destructor.
  ~DelayA();

  //! Clears the internal state of the delay line.
  void clear();

  //! Set the delay-line length
  /*!
    The valid range for \e theDelay is from 0.5 to the maximum delay-line length.
  */
  void setDelay(MY_FLOAT theDelay);

  //! Return the current delay-line length.
  MY_FLOAT getDelay(void);

  //! Input one sample to the delay-line and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

protected:  
    MY_FLOAT alpha;
    MY_FLOAT coeff;
    MY_FLOAT apInput;
};

#endif
