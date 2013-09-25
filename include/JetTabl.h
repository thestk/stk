/***************************************************/
/*! \class JetTabl
    \brief STK jet table class.

    This class implements a flue jet non-linear
    function, computed by a polynomial calculation.
    Contrary to the name, this is not a "table".

    Consult Fletcher and Rossing, Karjalainen,
    Cook, and others for more information.

    by Perry R. Cook and Gary P. Scavone, 1995 - 2002.
*/
/***************************************************/

#if !defined(__JETTABL_H)
#define __JETTABL_H

#include "Stk.h"

class JetTabl : public Stk
{
public:
  //! Default constructor.
  JetTabl();

  //! Class destructor.
  ~JetTabl();

  //! Return the last output value.
  MY_FLOAT lastOut() const;

  //! Return the function value for \e input.
  MY_FLOAT tick(MY_FLOAT input);

  //! Take \e vectorSize inputs and return the corresponding function values in \e vector.
  MY_FLOAT *tick(MY_FLOAT *vector, unsigned int vectorSize);

protected:  
  MY_FLOAT lastOutput;

};

#endif
