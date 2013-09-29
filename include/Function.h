/***************************************************/
/*! \class Function
    \brief STK abstract function parent class.

    This class provides common functionality for STK classes which
    implement tables or other types of input to output function
    mappings.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#include "Stk.h"

#ifndef STK_FUNCTION_H
#define STK_FUNCTION_H

class Function : public Stk
{
 public:
  //! Class constructor.
  Function();

  //! Class destructor.
  virtual ~Function();

  //! Return the last output value.
  virtual StkFloat lastOut() const { return lastOutput_; };

  //! Abstract tick function ... must be implemented in subclasses.
  virtual StkFloat tick( StkFloat input ) = 0;

  //! Take \e vectorSize inputs from \e vector and replace them with corresponding outputs.
  virtual StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the function and replace with corresponding outputs.
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

