/***************************************************/
/*! \class Delay
    \brief STK non-interpolating delay line class.

    This protected Filter subclass implements
    a non-interpolating digital delay-line.
    A fixed maximum length of 4095 and a delay
    of zero is set using the default constructor.
    Alternatively, the delay and maximum length
    can be set during instantiation with an
    overloaded constructor.
    
    A non-interpolating delay line is typically
    used in fixed delay-length applications, such
    as for reverberation.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__DELAY_H)
#define __DELAY_H

#include "Filter.h"

class Delay : protected Filter
{
public:

  //! Default constructor creates a delay-line with maximum length of 4095 samples and zero delay.
  Delay();

  //! Overloaded constructor which specifies the current and maximum delay-line lengths.
  Delay(long theDelay, long maxDelay);

  //! Class destructor.
  virtual ~Delay();

  //! Clears the internal state of the delay line.
  void clear();

  //! Set the delay-line length.
  /*!
    The valid range for \e theDelay is from 0 to the maximum delay-line length.
  */
  void setDelay(long theDelay);

  //! Return the current delay-line length.
  long getDelay(void) const;

  //! Calculate and return the signal energy in the delay-line.
  MY_FLOAT energy(void) const;

  //! Return the value at \e tapDelay samples from the delay-line input.
  /*!
    The valid range for \e tapDelay is 1 to the delay-line length.
  */
  MY_FLOAT contentsAt(long tapDelay) const;

  //! Return the last computed output value.
  MY_FLOAT lastOut(void) const;

  //! Input one sample to the delay-line and return one output.
  virtual MY_FLOAT tick(MY_FLOAT sample);

  //! Input \e vectorSize samples to the delay-line and return an equal number of outputs in \e vector.
  virtual MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

protected:
  long inPoint;
  long outPoint;
  long length;
  MY_FLOAT delay;
};

#endif

