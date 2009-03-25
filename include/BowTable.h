/***************************************************/
/*! \class BowTable
    \brief STK bowed string table class.

    This class implements a simple bowed string
    non-linear function, as described by Smith (1986).

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_BOWTABL_H
#define STK_BOWTABL_H

#include "Function.h"

class BowTable : public Function
{
public:
  //! Default constructor.
  BowTable();

  //! Class destructor.
  ~BowTable();

  //! Set the table offset value.
  /*!
    The table offset is a bias which controls the
    symmetry of the friction.  If you want the
    friction to vary with direction, use a non-zero
    value for the offset.  The default value is zero.
  */
  void setOffset(StkFloat offset);

  //! Set the table slope value.
  /*!
   The table slope controls the width of the friction
   pulse, which is related to bow force.
  */
  void setSlope(StkFloat slope);

  //! Return the function value for \e input.
  /*!
    The function input represents differential
    string-to-bow velocity.
  */
  StkFloat tick( StkFloat input);

  //! Take \e vectorSize inputs from \e vector and replace them with corresponding outputs.
  StkFloat *tick( StkFloat *vector, unsigned int vectorSize );

  //! Take a channel of the StkFrames object as inputs to the function and replace with corresponding outputs.
  /*!
    The \c channel argument should be one or greater (the first
    channel is specified by 1).  An StkError will be thrown if the \c
    channel argument is zero or it is greater than the number of
    channels in the StkFrames object.
  */
  StkFrames& tick( StkFrames& frames, unsigned int channel = 1 );

protected:  
  StkFloat offset_;
  StkFloat slope_;

};

#endif
