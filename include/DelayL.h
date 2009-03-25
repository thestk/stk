/***************************************************/
/*! \class DelayL
    \brief STK linear interpolating delay line class.

    This Delay subclass implements a fractional-
    length digital delay-line using first-order
    linear interpolation.  A fixed maximum length
    of 4095 and a delay of zero is set using the
    default constructor.  Alternatively, the
    delay and maximum length can be set during
    instantiation with an overloaded constructor.

    Linear interpolation is an efficient technique
    for achieving fractional delay lengths, though
    it does introduce high-frequency signal
    attenuation to varying degrees depending on the
    fractional delay setting.  The use of higher
    order Lagrange interpolators can typically
    improve (minimize) this attenuation characteristic.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DELAYL_H)
#define __DELAYL_H

#include "Delay.h"

class DelayL : public Delay
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  DelayL();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  
  DelayL(MY_FLOAT theDelay, long maxDelay);

  //! Class destructor.
  ~DelayL();

  //! Set the delay-line length.
  /*!
    The valid range for \e theDelay is from 0 to the maximum delay-line length.
  */
  void setDelay(MY_FLOAT theDelay);

  //! Return the current delay-line length.
  MY_FLOAT getDelay(void) const;

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  MY_FLOAT nextOut(void);

  //! Input one sample to the delay-line and return one output.
  MY_FLOAT tick(MY_FLOAT sample);

 protected:  
  MY_FLOAT alpha;
  MY_FLOAT omAlpha;
  MY_FLOAT nextOutput;
  bool doNextOut;
};

#endif
