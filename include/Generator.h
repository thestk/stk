/***************************************************/
/*! \class Generator
    \brief STK abstract unit generator parent class.

    This class provides common functionality for
    STK unit generator source subclasses.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_GENERATOR_H
#define STK_GENERATOR_H

#include "Stk.h"

class Generator : public Stk
{
 public:
  //! Class constructor.
  Generator();

  //! Class destructor.
  virtual ~Generator();

  //! Return the last output value.
  virtual StkFloat lastOut() const { return lastOutput_; };

  //! Abstract tick function ... must be implemented in subclasses.
  virtual StkFloat tick( void ) = 0;

  //! Compute \e vectorSize outputs and return them in \e vector.
  virtual StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Fill a channel of the StkFrames object with computed outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  virtual StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

 protected:

  StkFloat lastOutput_;

};

#endif

