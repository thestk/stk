/***************************************************/
/*! \class JetTable
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2004.
*/
/***************************************************/

#ifndef STK_JETTABL_H
#define STK_JETTABL_H

#include "Function.h"

class JetTable : public Function
{
public:
  //! Default constructor.
  JetTable();

  //! Class destructor.
  ~JetTable();

  //! Return the function value for \e input.
  StkFloat tick(StkFloat input);

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

};

#endif
