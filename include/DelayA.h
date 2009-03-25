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

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_DELAYA_H
#define STK_DELAYA_H

#include "Delay.h"

class DelayA : public Delay
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  DelayA();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  /*!
    An StkError will be thrown if the delay parameter is less than
    zero, the maximum delay parameter is less than one, or the delay
    parameter is greater than the maxDelay value.
   */  
  DelayA(StkFloat delay, unsigned long maxDelay);

  //! Class destructor.
  ~DelayA();

  //! Clears the internal state of the delay line.
  void clear();

  //! Set the delay-line length
  /*!
    The valid range for \e theDelay is from 0.5 to the maximum delay-line length.
  */
  void setDelay(StkFloat delay);

  //! Return the current delay-line length.
  StkFloat getDelay(void);

  //! Return the value which will be output by the next call to tick().
  /*!
    This method is valid only for delay settings greater than zero!
   */
  StkFloat nextOut(void);

  //! Input one sample to the delayline and return one output.
  StkFloat tick(StkFloat sample);

  //! Input \e vectorSize samples to the delayline and return an equal number of outputs in \e vector.
  virtual StkFloat *tick(StkFloat *vector, unsigned int vectorSize);

  //! Take a channel of the StkFrames object as inputs to the delayline and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

protected:  
  StkFloat alpha_;
  StkFloat coeff_;
  StkFloat apInput_;
  StkFloat nextOutput_;
  bool doNextOut_;
};

#endif
